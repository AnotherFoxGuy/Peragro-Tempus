
#ifndef _MARS_SAT_H_
#define _MARS_SAT_H_


#define MARS_SAT_PHOBOS 0
#define MARS_SAT_DEIMOS 1

void GetMarsSatCoor(double jd,int body,double *xyz);
  /* Return the rectangular coordinates of the given satellite
     and the given julian date jd expressed in dynamical time (TAI+32.184s).
     The origin of the xyz-coordinates is the center of the planet.
     The reference frame is "dynamical equinox and ecliptic J2000",
     which is the reference frame in VSOP87 and VSOP87A.
  */


#endif
