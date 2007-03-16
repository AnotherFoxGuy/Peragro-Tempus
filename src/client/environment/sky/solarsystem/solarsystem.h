
#ifndef _SOLARSYSTEM_H_
#define _SOLARSYSTEM_H_


#include <vector>
#include <functional>

#include "planet.h"

#include "client/environment/sky/projector/projector.h"
#include "client/environment/sky/navigator/navigator.h"

#include "client/pointer/pointer.h"

#include <csutil/ref.h>
#include <iutil/objreg.h>
#include <csutil/xmltiny.h>
#include <iutil/document.h>
#include <iutil/vfs.h>

class EllipticalOrbit;

//class InitParser;

class SolarSystem
{
public:
    SolarSystem();
    virtual ~SolarSystem();

	///////////////////////////////////////////////////////////////////////////
	// Methods defined in the StelModule class
	virtual void init(/*const InitParser& conf*/);
	virtual string getModuleID() const {return "ssystem";}
	//! Return the maximum squared distance in pixels that any planet has travelled since the last update.
	virtual double draw(Projector *prj, const Navigator *nav, ToneReproductor *eye);
	virtual void update(Navigator* nav, double deltaTime);
	virtual void updateI18n();
	virtual void updateSkyCulture() {;}
	
	
	///////////////////////////////////////////////////////////////////////////
	// Properties setters and getters

	//! Activate/Deactivate light travel time correction
	void setFlagLightTravelTime(bool b) {flag_light_travel_time = b;}
	bool getFlagLightTravelTime(void) const {return flag_light_travel_time;}	
	
	//! Set/Get orbits lines color
	void setOrbitsColor(const Vec3f& c) {Planet::set_orbit_color(c);}
	Vec3f getOrbitsColor(void) const {return Planet::getOrbitColor();}
	
	//! Set/Get planets trails color
	void setTrailsColor(const Vec3f& c)  {Planet::set_trail_color(c);}
	Vec3f getTrailsColor(void) const {return Planet::getTrailColor();}
	
	//! Set/Get base planets display scaling factor 
	void setScale(float scale) {Planet::setScale(scale);}
	float getScale(void) const {return Planet::getScale();}
	
	//! Activate/Deactivate display of planets halos as GLPoints
	void setFlagPoint(bool b) {flagPoint = b;}
	bool getFlagPoint() {return flagPoint;}
	
	//! Set/Get if Moon display is scaled
	void setFlagMoonScale(bool b)
      {if (!b) getMoon()->set_sphere_scale(1);
       else getMoon()->set_sphere_scale(moonScale);
       flagMoonScale = b;}
	bool getFlagMoonScale(void) const {return flagMoonScale;}
	
	//! Set/Get Moon display scaling factor 
	void setMoonScale(float f) {moonScale = f; if (flagMoonScale) getMoon()->set_sphere_scale(moonScale);}
	float getMoonScale(void) const {return moonScale;}		
	
	///////////////////////////////////////////////////////////////////////////
	//! Compute the position and transform matrix for every elements of the solar system.
    //! observerPos is needed for light travel time computation.
    //! @param observerPos position of the observer in heliocentric ecliptic frame.
	void computePositions(double date, const Vec3d& observerPos = Vec3d(0,0,0));

	//! Return the matching planet pointer if exists or NULL
	Planet* searchByEnglishName(csString planetEnglishName) const;
	
	Planet* getSun(void) const {return sun;}
	Planet* getEarth(void) const {return earth;}
	Planet* getMoon(void) const {return moon;}
	
	//! Start/stop accumulating new trail data (clear old data)
	void startTrails(bool b);
	
	void updateTrails(const Navigator* nav);
	
	//! Get list of all the translated planets name
	vector<wstring> getNamesI18(void);
	
	
private:
	// Compute the transformation matrix for every elements of the solar system.
    // observerPos is needed for light travel time computation
    void computeTransMatrices(double date, const Vec3d& observerPos = Vec3d(0,0,0));

	// Load the bodies data from a file
	void loadPlanets();

	Planet* sun;
	Planet* moon;
	Planet* earth;

	// solar system related settings
	float object_scale;  // should be kept synchronized with star scale...

	bool flagMoonScale;
	float moonScale;	// Moon scale value
	
	// Whether to display halo as GLpoints
	bool flagPoint;
	
    // Vector containing all the bodies of the system
	vector<Planet*> system_planets;		

    // Pointers on created elliptical orbits
	vector<EllipticalOrbit*> ell_orbits;

	bool near_lunar_eclipse(const Navigator* nav, Projector* prj);
	
	// draw earth shadow on moon for lunar eclipses
	void draw_earth_shadow(const Navigator * nav, Projector * prj);  

	// And sort them from the furthest to the closest to the observer
	struct bigger_distance : public binary_function<Planet*, Planet*, bool>
	{
		bool operator()(Planet* p1, Planet* p2) { return p1->get_distance() > p2->get_distance(); }
	};

	//STexture *tex_earth_shadow;  // for lunar eclipses

	// Master settings
	bool flagOrbits;
	bool flag_light_travel_time;


    EllipticalOrbit* GetOrbit(Planet* parent, csRef<iDocumentNode> orbit);
    void GetPosAndOsc(csString name,  pos_func_type &posfunc, OsulatingFunctType* &osculating_func);
};


#endif // _SOLARSYSTEM_H_
