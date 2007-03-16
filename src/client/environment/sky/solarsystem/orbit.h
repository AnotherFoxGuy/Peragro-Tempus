
#ifndef _ORBIT_H_
#define _ORBIT_H_

#include "client/environment/sky/utils/vecmath.h"


class OrbitSampleProc;

class Orbit
{
public:
	virtual ~Orbit() {;}
    virtual Vec3d positionAtTime(double) const = 0;
    virtual double getPeriod() const = 0;
    virtual double getBoundingRadius() const = 0;
    virtual void sample(double, double, int, OrbitSampleProc&) const = 0;
};


class EllipticalOrbit : public Orbit
{
public:
    EllipticalOrbit(double pericenterDistance,
                    double eccentricity,
                    double inclination,
                    double ascendingNode,
                    double argOfPeriapsis,
                    double meanAnomalyAtEpoch,
                    double period,
                    double epoch, // = 2451545.0,
                    double parent_rot_obliquity, // = 0.0,
                    double parent_rot_ascendingnode // = 0.0
                    );

    // Compute the orbit for a specified Julian date and return a "stellarium compliant" function
//    void positionAtTime(double JD, double * X, double * Y, double * Z) const;
//    void positionAtTimev(double JD, double* v);

      // Compute position for a specified Julian date and return coordinates
      // given in "dynamical equinox and ecliptic J2000"
      // which is the reference frame for VSOP87
      // In order to rotate to VSOP87
      // parent_rot_obliquity and parent_rot_ascendingnode must be supplied.
    void positionAtTimevInVSOP87Coordinates(double JD, double* v) const;

      // Original one
    Vec3d positionAtTime(double) const;
    double getPeriod() const;
    double getBoundingRadius() const;
    virtual void sample(double, double, int, OrbitSampleProc&) const;

private:
    double eccentricAnomaly(double) const;
    Vec3d positionAtE(double) const;

    double pericenterDistance;
    double eccentricity;
    double inclination;
    double ascendingNode;
    double argOfPeriapsis;
    double meanAnomalyAtEpoch;
    double period;
    double epoch;
    double rotate_to_vsop87[9];
};


class OrbitSampleProc
{
 public:
	virtual ~OrbitSampleProc() {;}
    virtual void sample(const Vec3d&) = 0;
};



// Custom orbit classes should be derived from CachingOrbit.  The custom
// orbits can be expensive to compute, with more than 50 periodic terms.
// Celestia may need require position of a Planet more than once per frame; in
// order to avoid redundant calculation, the CachingOrbit class saves the
// result of the last calculation and uses it if the time matches the cached
// time.
class CachingOrbit : public Orbit
{
public:
    CachingOrbit() : lastTime(1.0e-30) {};

    virtual Vec3d computePosition(double jd) const = 0;
    virtual double getPeriod() const = 0;
    virtual double getBoundingRadius() const = 0;

    Vec3d positionAtTime(double jd) const;

    virtual void sample(double, double, int, OrbitSampleProc& proc) const;

private:
    mutable Vec3d lastPosition;
    mutable double lastTime;
};



#endif // _ORBIT_H_
