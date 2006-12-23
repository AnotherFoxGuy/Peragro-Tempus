

#ifndef _L1_H_
#define _L1_H_


#define L1_IO            0
#define L1_EUROPA        1
#define L1_GANYMEDE      2
#define L1_CALLISTO      3

void GetL1Coor(double jd,int body,double *xyz);
  /* Return the rectangular coordinates of the given satellite
     and the given julian date jd expressed in dynamical time (TAI+32.184s).
     The origin of the xyz-coordinates is the center of the planet.
     The reference frame is "dynamical equinox and ecliptic J2000",
     which is the reference frame in VSOP87 and VSOP87A.
  */


#endif
