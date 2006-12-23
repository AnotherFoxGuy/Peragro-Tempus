


#ifndef _VSOP87_H_
#define _VSOP87_H_


#define VSOP87_MERCURY  0
#define VSOP87_VENUS    1
#define VSOP87_EMB      2
#define VSOP87_MARS     3
#define VSOP87_JUPITER  4
#define VSOP87_SATURN   5
#define VSOP87_URANUS   6
#define VSOP87_NEPTUNE  7

void GetVsop87Coor(double jd,int body,double *xyz);
  /* Return the rectangular coordinates of the given planet
     and the given julian date jd expressed in dynamical time (TAI+32.184s).
     The origin of the xyz-coordinates is the center of the sun.
     The reference frame is "dynamical equinox and ecliptic J2000",
     which is the reference frame in VSOP87 and VSOP87A.
  */

void GetVsop87OsculatingCoor(double jd0,double jd,int body,double *xyz);
  /* The oculating orbit of epoch jd0, evatuated at jd, is returned.
  */


#endif
