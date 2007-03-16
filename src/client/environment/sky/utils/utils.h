

#ifndef _S_UTILITY_H_
#define _S_UTILITY_H_

#include <cssysdef.h>
#include "client/environment/sky/utils/vecmath.h"


namespace StelUtils {

  	//! @brief Convert from spherical coordinates to Rectangular direction
	//! @param lng longitude in radian
	//! @param lat latitude in radian
	void sphe_to_rect(double lng, double lat, Vec3d& v);
	
	//! @brief Convert from spherical coordinates to Rectangular direction
	//! @param lng longitude in radian
	//! @param lat latitude in radian
	void sphe_to_rect(float lng, float lat, Vec3d& v);
	
	//! @brief Convert from spherical coordinates to Rectangular direction
	//! @param lng double* to store longitude in radian
	//! @param lat double* to store latitude in radian
	void rect_to_sphe(double *lng, double *lat, const Vec3d& v);

}

#endif
