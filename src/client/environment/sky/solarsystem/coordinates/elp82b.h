


#ifndef _ELP82B_H_
#define _ELP82B_H_


void GetElp82bCoor(double jd,double xyz[3]);

  /* Return the rectangular coordinates of the earths moon
     on the given julian date jd expressed in dynamical time (TAI+32.184s).
     The origin of the xyz-coordinates is the center of the earth.
     The reference frame is "dynamical equinox and ecliptic J2000",
     which is the reference frame in VSOP87 and VSOP87A.

     According to vsop87.doc VSOP87 coordinates can be transformed to FK5 by
       X       cos(psi) -sin(psi) 0   1        0         0   X
       Y     = sin(psi)  cos(psi) 0 * 0 cos(eps) -sin(eps) * Y
       Z FK5          0         0 1   0 sin(eps)  cos(eps)   Z VSOP87
     with psi = -0.0000275 degrees = -0.099 arcsec and
     eps = 23.4392803055556 degrees = 23d26m21.4091sec.

     http://ssd.jpl.nasa.gov/horizons_doc.html#frames says:
       "J2000" selects an Earth Mean-Equator and dynamical Equinox of
       Epoch J2000.0 inertial reference system, where the Epoch of J2000.0
       is the Julian date 2451545.0. "Mean" indicates nutation effects are
       ignored in the frame definition. The system is aligned with the
       IAU-sponsored J2000 frame of the Radio Source Catalog of the
       International Earth Rotational Service (ICRF).
       The ICRF is thought to differ from FK5 by at most 0.01 arcsec.

     From this I conclude that in the context of stellarium
     ICRF, J2000 and FK5 are the same, while the transformation
     ICRF <-> VSOP87 must be done with the matrix given above.
   */



#endif
