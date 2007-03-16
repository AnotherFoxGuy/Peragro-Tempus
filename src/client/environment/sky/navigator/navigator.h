

#ifndef NAVIGATOR_H_
#define NAVIGATOR_H_

#include <cssysdef.h>
#include <csutil/ref.h>
#include <iutil/virtclk.h>
#include <iengine/engine.h>
#include <csgeom/path.h>
#include <iutil/object.h>
#include <csutil/cscolor.h>
#include <ivideo/graph3d.h>

#include "client/environment/sky/utils/time.h"
#include "client/environment/sky/utils/utils.h"

#include "client/environment/sky/utils/vecmath.h"

#include "client/environment/sky/observator/observator.h"

extern const Mat4d mat_j2000_to_vsop87;
extern const Mat4d mat_vsop87_to_j2000;

// Class which manages a navigation context
// Manage date/time, viewing direction/fov, observer position, and coordinate changes
class Navigator
{
public:

	enum VIEWING_MODE_TYPE
	{
		VIEW_HORIZON,
		VIEW_EQUATOR
	};
	// Create and initialise to default a navigation context
	Navigator(Observator* obs);
    virtual ~Navigator();

	virtual void init();

	void updateTime(int delta_time);
	void updateTransformMatrices(void);
    void updateVisionVector(int delta_time);

	// Move to the given position in equatorial or local coordinate depending on _local_pos value
	void moveTo(const Vec3d& _aim, float move_duration = 1., bool _local_pos = false, int zooming = 0);

	// Time controls
	//! Set the current date in Julian Day
	void setJDay(double JD) {JDay=JD;}
	//! Get the current date in Julian Day
	double getJDay(void) const {return JDay;}

	//! Set time speed in JDay/sec
	void setTimeSpeed(double ts) {time_speed=ts;}
	//! Get time speed in JDay/sec
	double getTimeSpeed(void) const {return time_speed;}

	// Flags controls
	void setFlagTraking(int v) {flag_traking=v;}
	int getFlagTraking(void) const {return flag_traking;}
	void setFlagLockEquPos(int v) {flag_lock_equ_pos=v;}
	int getFlagLockEquPos(void) const {return flag_lock_equ_pos;}

	// Get vision direction
	const Vec3d& getPrecEquVision(void) const {return prec_equ_vision;}
	const Vec3d& getLocalVision(void) const {return local_vision;}
	void setLocalVision(const Vec3d& _pos);

	const class Planet *getHomePlanet(void) const {return position->getHomePlanet();}

    // Return the observer heliocentric position
	Vec3d getObserverHelioPos(void) const;

	// Transform vector from local coordinate to equatorial
	Vec3d local_to_earth_equ(const Vec3d& v) const { return mat_local_to_earth_equ*v; }

	// Transform vector from equatorial coordinate to local
	Vec3d earth_equ_to_local(const Vec3d& v) const { return mat_earth_equ_to_local*v; }

	Vec3d earth_equ_to_j2000(const Vec3d& v) const { return mat_earth_equ_to_j2000*v; }
	Vec3d j2000_to_earth_equ(const Vec3d& v) const { return mat_j2000_to_earth_equ*v; }

	// Transform vector from heliocentric coordinate to local
	Vec3d helio_to_local(const Vec3d& v) const { return mat_helio_to_local*v; }

	// Transform vector from heliocentric coordinate to earth equatorial,
    // only needed in meteor.cpp
	Vec3d helio_to_earth_equ(const Vec3d& v) const { return mat_helio_to_earth_equ*v; }

	// Transform vector from heliocentric coordinate to false equatorial : equatorial
	// coordinate but centered on the observer position (usefull for objects close to earth)
	Vec3d helio_to_earth_pos_equ(const Vec3d& v) const { return mat_local_to_earth_equ*mat_helio_to_local*v; }

	// Return the modelview matrix for some coordinate systems
	const Mat4d& get_helio_to_eye_mat(void)  {return mat_helio_to_eye;}
	const Mat4d& get_earth_equ_to_eye_mat(void)  {return mat_earth_equ_to_eye;}
	const Mat4d& get_local_to_eye_mat(void) {return mat_local_to_eye;}
	const Mat4d& get_j2000_to_eye_mat(void)  {return mat_j2000_to_eye;}

	void updateMove(double deltaAz, double deltaAlt);

	void setViewingMode(VIEWING_MODE_TYPE view_mode);
	VIEWING_MODE_TYPE getViewingMode(void) const {return viewing_mode;}

	const Vec3d& getinitViewPos() {return initViewPos;}

private:
	// Update the modelview matrices
	void updateModelViewMat(void);

	// Struct used to store data for auto mov
	typedef struct
	{
		Vec3d start;
	    Vec3d aim;
	    float speed;
	    float coef;
		bool local_pos;				// Define if the position are in equatorial or altazimutal
	}auto_move;


	// Matrices used for every coordinate transfo
	Mat4d mat_helio_to_local;		// Transform from Heliocentric to Observator local coordinate
	Mat4d mat_local_to_helio;		// Transform from Observator local coordinate to Heliocentric
	Mat4d mat_local_to_earth_equ;	// Transform from Observator local coordinate to Earth Equatorial
	Mat4d mat_earth_equ_to_local;	// Transform from Observator local coordinate to Earth Equatorial
	Mat4d mat_helio_to_earth_equ;	// Transform from Heliocentric to earth equatorial coordinate
	Mat4d mat_earth_equ_to_j2000;
	Mat4d mat_j2000_to_earth_equ;

	Mat4d mat_local_to_eye;			// Modelview matrix for observer local drawing
	Mat4d mat_earth_equ_to_eye;		// Modelview matrix for geocentric equatorial drawing
	Mat4d mat_j2000_to_eye;	        // precessed version
	Mat4d mat_helio_to_eye;			// Modelview matrix for heliocentric equatorial drawing

	// Vision variables
	Vec3d local_vision, equ_vision, prec_equ_vision;	// Viewing direction in local and equatorial coordinates
	int flag_traking;				// Define if the selected object is followed
	int flag_lock_equ_pos;			// Define if the equatorial position is locked

	// Automove
	auto_move move;					// Current auto movement
    int flag_auto_move;				// Define if automove is on or off
	int zooming_mode;				// 0 : undefined, 1 zooming, -1 unzooming

	// Time variable
    double time_speed;				// Positive : forward, Negative : Backward, 1 = 1sec/sec
	double JDay;        			// Curent time in Julian day

	Vec3d initViewPos;				// Default viewing direction

	VIEWING_MODE_TYPE viewing_mode;   // defines if view corrects for horizon, or uses equatorial coordinates

    Observator* position;

};

#endif //NAVIGATOR_H_
