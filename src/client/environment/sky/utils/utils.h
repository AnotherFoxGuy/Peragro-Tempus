

#ifndef _S_UTILITY_H_
#define _S_UTILITY_H_

#include <cssysdef.h>
#include <csgeom/math3d.h>
#include <csgeom/vector4.h>
#include <csgeom/transfrm.h>
#include <csgeom/matrix3.h>


namespace StelUtils {

  	//! @brief Convert from spherical coordinates to Rectangular direction
	//! @param lng longitude in radian
	//! @param lat latitude in radian
	void sphe_to_rect(double lng, double lat, csVector3& v);
	
	//! @brief Convert from spherical coordinates to Rectangular direction
	//! @param lng longitude in radian
	//! @param lat latitude in radian
	void sphe_to_rect(float lng, float lat, csVector3& v);
	
	//! @brief Convert from spherical coordinates to Rectangular direction
	//! @param lng double* to store longitude in radian
	//! @param lat double* to store latitude in radian
	void rect_to_sphe(double *lng, double *lat, const csVector3& v);

}

#endif
