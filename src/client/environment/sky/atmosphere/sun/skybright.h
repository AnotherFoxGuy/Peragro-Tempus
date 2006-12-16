

#ifndef _SKYBRIGHT_H_
#define _SKYBRIGHT_H_

class Skybright
{
public:
    Skybright();

	// month : 1=Jan, 12=Dec
	// moon_phase in radian 0=Full Moon, PI/2=First Quadrant/Last Quadran, PI=No Moon
	void set_date(int year, int month, float moon_phase);

	// Set the position parameters
	// Latitude in radian, altitude in m, temperature in deg. C, humidity in %
	void set_loc(float latitude, float altitude, float temperature = 15.f,
		float relative_humidity = 40.f);

	// Set the moon and sun zenith angular distance (cosin given) and precompute what can be
	// This funtion has to be called once before any call to get_luminance()
	// Input : cos_dist_moon_zenith = cos(angular distance between moon and zenith)
	//		   cos_dist_sun_zenith  = cos(angular distance between sun and zenith)
	void set_sun_moon(float cos_dist_moon_zenith, float cos_dist_sun_zenith);

	// Compute the luminance at the given position
	// Inputs : cos_dist_moon = cos(angular distance between moon and the position)
	//			cos_dist_sun  = cos(angular distance between sun  and the position)
	//			cos_dist_zenith = cos(angular distance between zenith and the position)
	float get_luminance(float cos_dist_moon, float cos_dist_sun, float cos_dist_zenith);

private:
	float air_mass_moon;	// Air mass for the Moon
	float air_mass_sun;		// Air mass for the Sun

	double b_total;			// Total brightness
	float b_night;			// Dark night brightness
	float b_twilight;		// Twilight brightness
	float b_daylight;		// Daylight sky brightness
	float b_moon;			// Moon brightness
	float mag_moon;			// Moon magnitude

	float RA;				// Something related with date

	float K;				// Useful coef...

	float C3;				// Term for moon brightness computation
	float C4;				// Term for sky brightness computation

	float SN; 				// Snellen Ratio (20/20=1.0, good 20/10=2.0)

	// Optimisation variables
	float b_night_term;
	float b_moon_term1;
	float b_twilight_term;

};

#endif // _SKYBRIGHT_H_
