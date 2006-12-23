

#include <functional>
#include <algorithm>
#include <cmath>

#include "solve.h"
#include "orbit.h"

#include "client/environment/sky/utils/time.h"

using namespace std;


EllipticalOrbit::EllipticalOrbit(double pericenterDistance,
                                 double eccentricity,
                                 double inclination,
                                 double ascendingNode,
                                 double argOfPeriapsis,
                                 double meanAnomalyAtEpoch,
                                 double period,
                                 double epoch,
                                 double parent_rot_obliquity,
                                 double parent_rot_ascendingnode) :
    pericenterDistance(pericenterDistance),
    eccentricity(eccentricity),
    inclination(inclination),
    ascendingNode(ascendingNode),
    argOfPeriapsis(argOfPeriapsis),
    meanAnomalyAtEpoch(meanAnomalyAtEpoch),
    period(period),
    epoch(epoch)
{
  const double c_obl = cos(parent_rot_obliquity);
  const double s_obl = sin(parent_rot_obliquity);
  const double c_nod = cos(parent_rot_ascendingnode);
  const double s_nod = sin(parent_rot_ascendingnode);
  rotate_to_vsop87[0] =  c_nod;
  rotate_to_vsop87[1] = -s_nod * c_obl;
  rotate_to_vsop87[2] =  s_nod * s_obl;
  rotate_to_vsop87[3] =  s_nod;
  rotate_to_vsop87[4] =  c_nod * c_obl;
  rotate_to_vsop87[5] = -c_nod * s_obl;
  rotate_to_vsop87[6] =  0.0;
  rotate_to_vsop87[7] =          s_obl;
  rotate_to_vsop87[8] =          c_obl;
}

// Standard iteration for solving Kepler's Equation
struct SolveKeplerFunc1 : public unary_function<double, double>
{
    double ecc;
    double M;

    SolveKeplerFunc1(double _ecc, double _M) : ecc(_ecc), M(_M) {};

    double operator()(double x) const
    {
        return M + ecc * sin(x);
    }
};


// Faster converging iteration for Kepler's Equation; more efficient
// than above for orbits with eccentricities greater than 0.3.  This
// is from Jean Meeus's _Astronomical Algorithms_ (2nd ed), p. 199
struct SolveKeplerFunc2 : public unary_function<double, double>
{
    double ecc;
    double M;

    SolveKeplerFunc2(double _ecc, double _M) : ecc(_ecc), M(_M) {};

    double operator()(double x) const
    {
        return x + (M + ecc * sin(x) - x) / (1 - ecc * cos(x));
    }
};

double sign(double x)
{
    if (x < 0.)
        return -1.;
    else if (x > 0.)
        return 1.;
    else
        return 0.;
}

struct SolveKeplerLaguerreConway : public unary_function<double, double>
{
    double ecc;
    double M;

    SolveKeplerLaguerreConway(double _ecc, double _M) : ecc(_ecc), M(_M) {};

    double operator()(double x) const
    {
        double s = ecc * sin(x);
        double c = ecc * cos(x);
        double f = x - s - M;
        double f1 = 1 - c;
        double f2 = s;
        x += -5 * f / (f1 + sign(f1) * sqrt(abs(16 * f1 * f1 - 20 * f * f2)));

        return x;
    }
};

struct SolveKeplerLaguerreConwayHyp : public unary_function<double, double>
{
    double ecc;
    double M;

    SolveKeplerLaguerreConwayHyp(double _ecc, double _M) : ecc(_ecc), M(_M) {};

    double operator()(double x) const
    {
        double s = ecc * sinh(x);
        double c = ecc * cosh(x);
        double f = s - x - M;
        double f1 = c - 1;
        double f2 = s;
        x += -5 * f / (f1 + sign(f1) * sqrt(abs(16 * f1 * f1 - 20 * f * f2)));

        return x;
    }
};

typedef pair<double, double> Solution;


double EllipticalOrbit::eccentricAnomaly(double M) const
{
    if (eccentricity == 0.0)
    {
        // Circular orbit
        return M;
    }
    else if (eccentricity < 0.2)
    {
        // Low eccentricity, so use the standard iteration technique
        Solution sol = solve_iteration_fixed(SolveKeplerFunc1(eccentricity, M), M, 5);
        return sol.first;
    }
    else if (eccentricity < 0.9)
    {
        // Higher eccentricity elliptical orbit; use a more complex but
        // much faster converging iteration.
        Solution sol = solve_iteration_fixed(SolveKeplerFunc2(eccentricity, M), M, 6);
        // Debugging
        // printf("ecc: %f, error: %f mas\n",
        //        eccentricity, radToDeg(sol.second) * 3600000);
        return sol.first;
    }
    else if (eccentricity < 1.0)
    {
        // Extremely stable Laguerre-Conway method for solving Kepler's
        // equation.  Only use this for high-eccentricity orbits, as it
        // requires more calcuation.
        double E = M + 0.85 * eccentricity * sign(sin(M));
        Solution sol = solve_iteration_fixed(SolveKeplerLaguerreConway(eccentricity, M), E, 8);
        return sol.first;
    }
    else if (eccentricity == 1.0)
    {
        // Nearly parabolic orbit; very common for comets
        // TODO: handle this
        return M;
    }
    else
    {
        // Laguerre-Conway method for hyperbolic (ecc > 1) orbits.
        double E = log(2 * M / eccentricity + 1.85);
        Solution sol = solve_iteration_fixed(SolveKeplerLaguerreConwayHyp(eccentricity, M), E, 30);
        return sol.first;
    }
}


csVector3 EllipticalOrbit::positionAtE(double E) const
{
    double x, z;

    if (eccentricity < 1.0)
    {
        double a = pericenterDistance / (1.0 - eccentricity);
        x = a * (cos(E) - eccentricity);
        z = a * sqrt(1 - eccentricity * eccentricity) * -sin(E);
    }
    else if (eccentricity > 1.0)
    {
        double a = pericenterDistance / (1.0 - eccentricity);
        x = -a * (eccentricity - cosh(E));
        z = -a * sqrt(eccentricity * eccentricity - 1) * -sinh(E);
    }
    else
    {
        // TODO: Handle parabolic orbits
        x = 0.0;
        z = 0.0;
    }

    csReversibleTransform R = (yrotation(ascendingNode) *
                   xrotation(inclination) *
                   yrotation(argOfPeriapsis));

    return R * csVector3(x, 0, z);
}


// Return the offset from the center
csVector3 EllipticalOrbit::positionAtTime(double t) const
{
    t = t - epoch;
    double meanMotion = 2.0 * M_PI / period;
    double meanAnomaly = meanAnomalyAtEpoch + t * meanMotion;
    double E = eccentricAnomaly(meanAnomaly);

    return positionAtE(E);
}

void EllipticalOrbit::positionAtTimevInVSOP87Coordinates(double JD, double* v) const
{
  csVector3 pos = positionAtTime(JD);
  v[0] = rotate_to_vsop87[0]*pos[2] + rotate_to_vsop87[1]*pos[0] + rotate_to_vsop87[1]*pos[1];
  v[1] = rotate_to_vsop87[3]*pos[2] + rotate_to_vsop87[4]*pos[0] + rotate_to_vsop87[5]*pos[1];
  v[2] = rotate_to_vsop87[6]*pos[2] + rotate_to_vsop87[7]*pos[0] + rotate_to_vsop87[8]*pos[1];
}

double EllipticalOrbit::getPeriod() const
{
    return period;
}


double EllipticalOrbit::getBoundingRadius() const
{
    // TODO: watch out for unbounded parabolic and hyperbolic orbits
    return pericenterDistance * ((1.0 + eccentricity) / (1.0 - eccentricity));
}


void EllipticalOrbit::sample(double start, double t, int nSamples,
                             OrbitSampleProc& proc) const
{
    double dE = 2 * M_PI / (double) nSamples;
    for (int i = 0; i < nSamples; i++)
        proc.sample(positionAtE(dE * i));
}



csVector3 CachingOrbit::positionAtTime(double jd) const
{
    if (jd != lastTime)
    {
        lastTime = jd;
        lastPosition = computePosition(jd);
    }
    return lastPosition;
}


void CachingOrbit::sample(double start, double t, int nSamples,
                          OrbitSampleProc& proc) const
{
    double dt = t / (double) nSamples;
    for (int i = 0; i < nSamples; i++)
        proc.sample(positionAtTime(start + dt * i));
}


