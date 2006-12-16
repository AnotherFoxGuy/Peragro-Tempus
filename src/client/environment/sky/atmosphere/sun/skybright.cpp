

#include <cstdio>
#include <cmath>

#define M_PI       3.14159265358979323846
#define M_PI_2     1.57079632679489661923
#define M_PI_4     0.785398163397448309616

#ifndef HAVE_POW10
# define pow10(x) pow(10,(x))
#endif

#include "skybright.h"

Skybright::Skybright() : SN(1.f)
{
	set_date(2003, 8, 0);
	set_loc(M_PI_4, 1000., 25.f, 40.f);
	set_sun_moon(0.5, 0.5);
}

// month : 1=Jan, 12=Dec
// moon_phase in radian 0=Full Moon, PI/2=First Quadrant/Last Quadran, PI=No Moon
void Skybright::set_date(int year, int month, float moon_phase)
{
	mag_moon = -12.73f + 1.4896903f * fabs(moon_phase) + 0.04310727f * pow(moon_phase, 4.f);

	RA = (month - 3.f) * 0.52359878f;

	// Term for dark sky brightness computation
	b_night_term = 1.0e-13 + 0.3e-13 * std::cos(0.56636f * (year-1992.f));
}


void Skybright::set_loc(float latitude, float altitude, float temperature, float relative_humidity)
{
	float sign_latitude = (latitude>=0.f) * 2.f - 1.f;

	// extinction Coefficient for V band
	float KR = 0.1066f * std::exp(-altitude/8200.f);
	float KA = 0.1f * std::exp(-altitude/1500.f) * std::pow(1.f - 0.32f/std::log(relative_humidity/100.f) ,1.33f) *
		(1.f + 0.33f * sign_latitude * std::sin(RA));
	float KO = 0.031f * ( 3.f + 0.4f * (latitude * std::cos(RA) - std::cos(3.f*latitude)) )/3.f;
	float KW = 0.031f * 0.94f * (relative_humidity/100.f) * std::exp(temperature/15.f) * std::exp(-altitude/8200.f);
	K = KR + KA + KO + KW;
}

// Set the moon and sun zenith angular distance (cosin given)
// and precompute what can be
void Skybright::set_sun_moon(float cos_dist_moon_zenith, float cos_dist_sun_zenith)
{
	// Air mass for Moon
	if (cos_dist_moon_zenith<0) air_mass_moon = 40.f;
	else air_mass_moon = 1.f / (cos_dist_moon_zenith+0.025f*std::exp(-11.f*cos_dist_moon_zenith));

	// Air mass for Sun
	if (cos_dist_sun_zenith<0) air_mass_sun = 40;
	else air_mass_sun = 1.f / (cos_dist_sun_zenith+0.025f*std::exp(-11.f*cos_dist_sun_zenith));

	b_moon_term1 = pow10(-0.4f * (mag_moon + 54.32f));

	C3 = pow10(-0.4f*K*air_mass_moon);	// Term for moon brightness computation

	b_twilight_term = -6.724f + 22.918312f * (M_PI_2-std::acos(cos_dist_sun_zenith));

	C4 = pow10(-0.4f*K*air_mass_sun);	// Term for sky brightness computation
}


// Compute the luminance at the given position
// Inputs : cos_dist_moon = cos(angular distance between moon and the position)
//			cos_dist_sun  = cos(angular distance between sun  and the position)
//			cos_dist_zenith = cos(angular distance between zenith and the position)
float Skybright::get_luminance(float cos_dist_moon, float cos_dist_sun, float cos_dist_zenith)
{

    // catch rounding errors here or end up with white flashes in some cases
    if(cos_dist_moon < -1.f ) cos_dist_moon = -1.f;
	if(cos_dist_moon > 1.f ) cos_dist_moon = 1.f;
	if(cos_dist_sun < -1.f ) cos_dist_moon = -1.f;
	if(cos_dist_sun > 1.f ) cos_dist_sun = 1.f;
	if(cos_dist_zenith < -1.f ) cos_dist_zenith = -1.f;
	if(cos_dist_zenith > 1.f ) cos_dist_zenith = 1.f;

	float dist_moon = std::acos(cos_dist_moon);
	float dist_sun = std::acos(cos_dist_sun);

	// Air mass
	float X = 1.f / (cos_dist_zenith + 0.025f*std::exp(-11.f*cos_dist_zenith));
	float bKX = pow10(-0.4f * K * X);

	// Dark night sky brightness
	b_night = 0.4f+0.6f/std::sqrt(0.04f + 0.96f * cos_dist_zenith*cos_dist_zenith);
	b_night *= b_night_term * bKX;

	// Moonlight brightness
	float FM = 18886.28 / (dist_moon*dist_moon + 0.0007f) + pow10(6.15f - (dist_moon+0.001) * 1.43239f);
	FM += 229086.77f * ( 1.06f + cos_dist_moon*cos_dist_moon );
	b_moon = b_moon_term1 * (1.f - bKX) * (FM * C3 + 440000.f * (1.f - C3));

	//Twilight brightness
	b_twilight = pow10(b_twilight_term + 0.063661977f * std::acos(cos_dist_zenith)/K) *
		(1.7453293f / dist_sun) * (1.f-bKX);

	// Daylight brightness
	float FS = 18886.28f / (dist_sun*dist_sun + 0.0007f) + pow10(6.15f - (dist_sun+0.001)* 1.43239f);
	FS += 229086.77f * ( 1.06f + cos_dist_sun*cos_dist_sun );
	b_daylight = 9.289663e-12 * (1.f - bKX) * (FS * C4 + 440000.f * (1.f - C4));

	// 27/08/2003 : Decide increase moonlight for more halo effect...
	b_moon *= 2.;

	// Total sky brightness
	b_daylight>b_twilight ? b_total = b_night + b_twilight + b_moon : b_total = b_night + b_daylight + b_moon;

	return (b_total<0.f) ? 0.f : b_total * 900900.9f * M_PI * 1e-4 * 3239389*2;
	//5;	// In cd/m^2 : the 32393895 is empirical term because the
	// lambert -> cd/m^2 formula seems to be wrong...
}
/*
250 REM  Visual limiting magnitude
260 BL=B(3)/1.11E-15 : REM in nanolamberts*/

	// Airmass for each component
	//cos_dist_zenith = std::cos(dist_zenith);
	//float gaz_mass = 1.f / ( cos_dist_zenith + 0.0286f * std::exp(-10.5f * cos_dist_zenith) );
	//float aerosol_mass = 1.f / ( cos_dist_zenith + 0.0123f * std::exp(-24.5f * cos_dist_zenith) );
	//float ozone_mass = 1.f / std::sqrt( 0.0062421903f - cos_dist_zenith * cos_dist_zenith / 1.0062814f );
	// Total extinction for V band
	//float DM = KR*gaz_mass + KA*aerosol_mass + KO*ozone_mass + KW*gaz_mass;



	/*
	// Visual limiting magnitude
	if (BL>1500.0)
	{
		C1 = 4.466825e-9;
		C2 = 1.258925e-6;
	}
	else
	{
		C1 = 1.584893e-10;
		C2 = 0.012589254;
	}

	float TH = C1*std::pow(1.f+sqrt(C2*BL),2.f); // in foot-candles
	float MN = -16.57-2.5*log10f(TH)-DM+5.0*log10f(SN); // Visual Limiting Magnitude
	*/
