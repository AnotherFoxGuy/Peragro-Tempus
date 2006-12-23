

#ifndef _TONE_REPRODUCTOR_H_
#define _TONE_REPRODUCTOR_H_

#define M_PI       3.14159265358979323846
#define M_PI_2     1.57079632679489661923

class ToneReproductor
{
public:
    ToneReproductor();
    virtual ~ToneReproductor();

	// Set the eye adaptation luminance for the display (and precompute what can be)
	// Usual luminance range is 1-100 cd/m^2 for a CRT screen
	// default value = 50 cd/m^2
	void set_display_adaptation_luminance(float display_adaptation_luminance);

	// Set the eye adaptation luminance for the world (and precompute what can be)
	// default value = 40000 cd/m^2 for Skylight
	// Star Light      : 0.001  cd/m^2
	// Moon Light      : 0.1    cd/m^2
	// Indoor Lighting : 100    cd/m^2
	// Sun Light       : 100000 cd/m^2
	void set_world_adaptation_luminance(float world_adaptation_luminance);

	// Set the maximum display luminance : default value = 100 cd/m^2
	// This value is used to scale the RGB range
	void set_max_display_luminance(float maxdL) {one_over_maxdL = 1.f/maxdL;}

	// Set the display gamma : default value = 2.3
	void set_display_gamma(float gamma) {one_over_gamma = 1.f/gamma;}

	// Return adapted luminance from world to display
	float adapt_luminance(float world_luminance) const
	{
		return pow((float)(world_luminance*M_PI*0.0001f),alpha_wa_over_alpha_da) * term2;
	}

	// Convert from xyY color system to RGB
	void xyY_to_RGB(float*);

	// Provide the luminance in cd/m^2 from the magnitude and the surface in arcmin^2
	static float mag_to_luminance(float mag, float surface);

private:
	float Lda;		// Display luminance adaptation (in cd/m^2)
	float Lwa;		// World   luminance adaptation (in cd/m^2)
	float one_over_maxdL;	// 1 / Display maximum luminance (in cd/m^2)
	float one_over_gamma;	// 1 / Screen gamma value

	// Precomputed variables
	float alpha_da;
	float beta_da;
	float alpha_wa;
	float beta_wa;
	float alpha_wa_over_alpha_da;
	float term2;
};

#endif // _TONE_REPRODUCTOR_H_
