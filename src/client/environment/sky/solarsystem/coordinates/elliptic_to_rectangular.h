

#ifndef _ELLIPTIC_TO_RECTANGULAR_
#define _ELLIPTIC_TO_RECTANGULAR_


/*
   Given the orbital elements at some time t0 calculate the
   rectangular coordinates at time (t0+dt).

   mu = G*(m1+m2) .. gravitational constant of the two body problem
   a .. semi major axis
   n = mean motion = 2*M_PI/(orbit period)

   elem[0] .. unused (eigther a or n)
   elem[1] .. L
   elem[2] .. K=e*cos(Omega+omega)
   elem[3] .. H=e*sin(Omega+omega)
   elem[4] .. Q=sin(i/2)*cos(Omega)
   elem[5] .. P=sin(i/2)*sin(Omega)

   Omega = longitude of ascending node
   omega = argument of pericenter
   L = mean longitude = Omega + omega + M
   M = mean anomaly
   i = inclination
   e = excentricity

   Units are suspected to be: Julian days, AU, rad
*/

void EllipticToRectangularN(double mu,const double elem[6],double dt,
                            double xyz[]);
void EllipticToRectangularA(double mu,const double elem[6],double dt,
                            double xyz[]);


#endif
