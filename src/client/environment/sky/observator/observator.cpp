

#include <string>
#include <cstdlib>
#include <algorithm>

#include <cssysdef.h>
#include "ivaria/reporter.h"

#include "client/pointer/pointer.h"


#include "client/environment/sky/utils/vecmath.h"

#include "client/environment/sky/observator/observator.h"
#include "client/environment/sky/solarsystem/solarsystem.h"




Observator::Observator(const SolarSystem &ssystem)
           :ssystem(ssystem), planet(0),
            longitude(0.), latitude(0.), altitude(0)
{
	name = L"Anonymous_Location";
	flag_move_to = 0;
}

Observator::~Observator()
{
}

Vec3d Observator::getCenterVsop87Pos(void) const {
  return planet->get_heliocentric_ecliptic_pos();
}

double Observator::getDistanceFromCenter(void) const {
  return planet->getRadius() + (altitude/(1000*AU));
}

Mat4d Observator::getRotLocalToEquatorial(double jd) const {
  double lat = latitude;
  // TODO: Figure out how to keep continuity in sky as reach poles
  // otherwise sky jumps in rotation when reach poles in equatorial mode
  // This is a kludge
  if( lat > 89.5 )  lat = 89.5;
  if( lat < -89.5 ) lat = -89.5;
  return Mat4d::zrotation((planet->getSiderealTime(jd)+longitude)*(M_PI/180.))
    * Mat4d::yrotation((90.-lat)*(M_PI/180.));
}

Mat4d Observator::getRotEquatorialToVsop87(void) const {
  return planet->getRotEquatorialToVsop87();
}

void Observator::load(const string& file, const string& section)
{
  /*
	InitParser conf;
	conf.load(file);
	if (!conf.find_entry(section))
	{
		cerr << "ERROR : Can't find observator section " << section << " in file " << file << endl;
		assert(0);
	}
	load(conf, section);
    */
}

bool Observator::setHomePlanet(csString english_name) {
  Planet *p = ssystem.searchByEnglishName(english_name);
  if (p) {
    planet = p;
    return true;
  }
  return false;
}


void Observator::load(/*const InitParser& conf, const string& section*/)
{
/*
	name = _(conf.get_str(section, "name").c_str());

	for (string::size_type i=0;i<name.length();++i)
	{
		if (name[i]=='_') name[i]=' ';
	}

    if (!setHomePlanet(conf.get_str(section, "home_planet", "Earth"))) {
      planet = ssystem.getEarth();
    }

	latitude  = StelUtils::get_dec_angle(conf.get_str(section, "latitude"));
	longitude = StelUtils::get_dec_angle(conf.get_str(section, "longitude"));
	altitude = conf.get_int(section, "altitude");

*/
  if (!setHomePlanet("Earth"))
  {
    planet = ssystem.getEarth();
  }
  if(!planet)
  {
    iObjectRegistry* obj_reg = PointerLibrary::getInstance()->getObjectRegistry();
    csRef<iReporter> report = csQueryRegistry<iReporter> (obj_reg);
    report->Report(CS_REPORTER_SEVERITY_ERROR, "pt.observator", "No home planet!");
  }

    latitude  = 48;
    longitude = 02;
    altitude = 83;
}

void Observator::set_landscape_name(const string s) {

	// need to lower case name because config file parser lowercases section names
	string x = s;
	transform(x.begin(), x.end(), x.begin(), ::tolower);
	landscape_name = x;
}

void Observator::save(const string& file, const string& section) const
{
/*
	printf("Saving location %s to file %s\n",StelUtils::wstringToString(name).c_str(), file.c_str());

	InitParser conf;
	conf.load(file);

	setConf(conf,section);

	conf.save(file);
*/
}


// change settings but don't write to files
void Observator::setConf(/*InitParser & conf, const string& section*/) const
{
/*
	conf.set_str(section + ":name", StelUtils::wstringToString(name));
	conf.set_str(section + ":home_planet", planet->getEnglishName());
	conf.set_str(section + ":latitude",
	             StelUtils::wstringToString(
	               StelUtils::printAngleDMS(latitude*M_PI/180.0,
	                                          true, true)));
	conf.set_str(section + ":longitude",
	             StelUtils::wstringToString(
	               StelUtils::printAngleDMS(longitude*M_PI/180.0,
	                                          true, true)));

	conf.set_int(section + ":altitude", altitude);
	conf.set_str(section + ":landscape_name", landscape_name);

	// TODO: clear out old timezone settings from this section
	// if still in loaded conf?  Potential for confusion.
*/
}


// move gradually to a new observation location
void Observator::moveTo(double lat, double lon, double alt, int duration, const wstring& _name)
{
  flag_move_to = 1;

  start_lat = latitude;
  end_lat = lat;

  start_lon = longitude;
  end_lon = lon;

  start_alt = altitude;
  end_alt = alt;

  move_to_coef = 1.0f/duration;
  move_to_mult = 0;

	name = _name;
  //  printf("coef = %f\n", move_to_coef);
}

wstring Observator::get_name(void) const
{
	return name;
}

string Observator::getHomePlanetEnglishName(void) const {
  return planet ? planet->getEnglishName().GetData() : "";
}

wstring Observator::getHomePlanetNameI18n(void) const {
  return planet ? planet->getNameI18n() : L"";
}

// for moving observator position gradually
// TODO need to work on direction of motion...
void Observator::update(int delta_time) {
  if(flag_move_to) {
    move_to_mult += move_to_coef*delta_time;

    if( move_to_mult >= 1) {
      move_to_mult = 1;
      flag_move_to = 0;
    }

    latitude = start_lat - move_to_mult*(start_lat-end_lat);
    longitude = start_lon - move_to_mult*(start_lon-end_lon);
    altitude = int(start_alt - move_to_mult*(start_alt-end_alt));

  }
}

