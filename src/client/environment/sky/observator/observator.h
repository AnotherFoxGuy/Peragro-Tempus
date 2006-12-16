

#ifndef _OBSERVATOR_H_
#define _OBSERVATOR_H_

#include "init_parser.h"
#include "vecmath.h"

class Observator
{
public:

	Observator(const class SolarSystem &ssystem);
	~Observator();
    bool setHomePlanet(const string &english_name);
    const class Planet *getHomePlanet(void) const {return planet;}
    string getHomePlanetEnglishName(void) const;
    wstring getHomePlanetNameI18n(void) const;

    Vec3d getCenterVsop87Pos(void) const;
    double getDistanceFromCenter(void) const;
    Mat4d getRotLocalToEquatorial(double jd) const;
    Mat4d getRotEquatorialToVsop87(void) const;

	void save(const string& file, const string& section) const;
	void setConf(InitParser &conf, const string& section) const;
	void load(const string& file, const string& section);
	void load(const InitParser& conf, const string& section);

	wstring get_name(void) const;

	void set_latitude(double l) {latitude=l;}
	double get_latitude(void) const {return latitude;}
	void set_longitude(double l) {longitude=l;}
	double get_longitude(void) const {return longitude;}
	void set_altitude(int a) {altitude=a;}
	int get_altitude(void) const {return altitude;}
	void set_landscape_name(const string s);
	string get_landscape_name(void) const {return landscape_name;}


	void moveTo(double lat, double lon, double alt, int duration, const wstring& _name);  // duration in ms
	void update(int delta_time);  // for moving observing position

private:
    const class SolarSystem &ssystem;
	wstring name;			// Position name
	const class Planet *planet;
	double longitude;		// Longitude in degree
	double latitude;		// Latitude in degree
	int altitude;			// Altitude in meter
	string landscape_name;

	// for changing position
	bool flag_move_to;
	double start_lat, end_lat;
	double start_lon, end_lon;
	double start_alt, end_alt;
	float move_to_coef, move_to_mult;

};

time_t my_timegm (struct tm *tm);


#endif // _OBSERVATOR_H_
