

#include <iostream>
#include <sstream>
#include <iomanip>

#include "planet.h"
#include "client/environment/sky/projector/projector.h"
#include "client/environment/sky/navigator/navigator.h"

#include "client/environment/sky/utils/vecmath.h"

float Planet::object_scale = 1.f;
Vec3f Planet::label_color = Vec3f(0.4f,0.4f,0.8f);
Vec3f Planet::orbit_color = Vec3f(1.0f,0.6f,1.0f);
Vec3f Planet::trail_color = Vec3f(1.0f,0.7f,0.7f);

RotationElements::RotationElements() : period(1.), offset(0.), epoch(J2000),
		obliquity(0.), ascendingNode(0.), precessionRate(0.)
{}

Planet::Planet(Planet *parent,
           csString englishName,
           bool flagHalo,
           bool flag_lighting,
           double radius,
           double oblateness,
           Vec3f color,
           float albedo,
           csString tex_map_name,
           csString tex_halo_name,
           pos_func_type _coord_func,
           OsulatingFunctType *osculating_func,
		   bool hidden) :
		englishName(englishName), flagHalo(flagHalo),
        flag_lighting(flag_lighting),
        radius(radius), one_minus_oblateness(1.0-oblateness),
        color(color), albedo(albedo), axis_rotation(0.),
        sphere_scale(1.f),
        lastJD(J2000), last_orbitJD(0), deltaJD(JD_SECOND), orbit_cached(0),
        coord_func(_coord_func), osculating_func(osculating_func),
        parent(parent), hidden(hidden)
{
	//if (parent) parent->satellites.push_back(this);
	ecliptic_pos=Vec3d(0.,0.,0.);
    rot_local_to_parent.Identity();
	mat_local_to_parent.Identity();
	//StelApp::getInstance().getTextureManager().setDefaultParams();
	//StelApp::getInstance().getTextureManager().setWrapMode(GL_REPEAT);
	//tex_map = &StelApp::getInstance().getTextureManager().createTexture(tex_map_name);
	//StelApp::getInstance().getTextureManager().setDefaultParams();
	//if (flagHalo) tex_halo = &StelApp::getInstance().getTextureManager().createTexture(tex_halo_name);

	// 60 day trails
	DeltaTrail = 1;
	// small increment like 0.125 would allow observation of latitude related wobble of moon
	// if decide to show moon trail
	MaxTrail = 60;
	last_trailJD = 0; // for now
	trail_on = 0;
	first_point = 1;
/*
	nameI18 = StelUtils::stringToWstring(englishName);
	if (englishName!="Moon" && englishName!="Pluto") {
	  deltaJD = 0.001*JD_SECOND;
	}
*/
}

Planet::~Planet()
{
  /*
	if (tex_map) delete tex_map;
	tex_map = NULL;
	if (tex_halo) delete tex_halo;
	tex_halo = NULL;
	if (rings) delete rings;
	rings = NULL;
	if (tex_big_halo) delete tex_big_halo;
	tex_big_halo = NULL;
    */
}


double Planet::get_close_fov(const Navigator* nav) const
{
	return atan(radius*sphere_scale*2.f/get_earth_equ_pos(nav).length())*180./M_PI * 4;
}

double Planet::get_satellites_fov(const Navigator * nav) const
{
	// TODO: calculate from satellite orbits rather than hard code
	if (englishName.GetData()=="Jupiter") return atan(0.005f/get_earth_equ_pos(nav).length())*180./M_PI * 4;
	if (englishName.GetData()=="Saturn") return atan(0.005f/get_earth_equ_pos(nav).length())*180./M_PI * 4;
	if (englishName.GetData()=="Mars") return atan(0.0001f/get_earth_equ_pos(nav).length())*180./M_PI * 4;
	if (englishName.GetData()=="Uranus") return atan(0.002f/get_earth_equ_pos(nav).length())*180./M_PI * 4;
	return -1.;
}

double Planet::get_parent_satellites_fov(const Navigator *nav) const {
  if (parent && parent->parent) return parent->get_satellites_fov(nav);
  return -1.0;
}

// Set the orbital elements
void Planet::set_rotation_elements(float _period, float _offset, double _epoch, float _obliquity, float _ascendingNode, float _precessionRate, double _sidereal_period )
{
	re.period = _period;
	re.offset = _offset;
	re.epoch = _epoch;
	re.obliquity = _obliquity;
	re.ascendingNode = _ascendingNode;
	re.precessionRate = _precessionRate;
	re.sidereal_period = _sidereal_period;  // used for drawing orbit lines

	delta_orbitJD = re.sidereal_period/ORBIT_SEGMENTS;
}


// Return the Planet position in rectangular earth equatorial coordinate
Vec3d Planet::get_earth_equ_pos(const Navigator * nav) const
{
	Vec3d v = get_heliocentric_ecliptic_pos();
	return nav->helio_to_earth_pos_equ(v);		// this is earth equatorial but centered
	// on observer's position (latitude, longitude)
	//return navigation.helio_to_earth_equ(&v); this is the real equatorial centered on earth center
}

Vec3d Planet::getObsJ2000Pos(const Navigator *nav) const
{
  Vec3d v(get_heliocentric_ecliptic_pos() - nav->getObserverHelioPos());

  return mat_vsop87_to_j2000.multiplyWithoutTranslation(v);
}


// Compute the position in the parent Planet coordinate system
// Actually call the provided function to compute the ecliptical position
void Planet::computePositionWithoutOrbits(const double date)
{
  if (fabs(lastJD-date)>deltaJD)
  {
    coord_func(date, ecliptic_pos);
    lastJD = date;
  }
}

void Planet::compute_position(const double date)
{
	if (delta_orbitJD > 0 && (fabs(last_orbitJD-date)>delta_orbitJD || !orbit_cached))
	{
		// calculate orbit first (for line drawing)
		double date_increment = re.sidereal_period/ORBIT_SEGMENTS;
		double calc_date;
		//	  int delta_points = (int)(0.5 + (date - last_orbitJD)/date_increment);
		int delta_points;

		if( date > last_orbitJD )
		{
			delta_points = (int)(0.5 + (date - last_orbitJD)/date_increment);
		}
		else
		{
			delta_points = (int)(-0.5 + (date - last_orbitJD)/date_increment);
		}
		double new_date = last_orbitJD + delta_points*date_increment;

		//	  printf( "Updating orbit coordinates for %s (delta %f) (%d points)\n", name.c_str(), delta_orbitJD, delta_points);


		if( delta_points > 0 && delta_points < ORBIT_SEGMENTS && orbit_cached)
		{

			for( int d=0; d<ORBIT_SEGMENTS; d++ )
			{
				if(d + delta_points >= ORBIT_SEGMENTS )
				{
					// calculate new points
					calc_date = new_date + (d-ORBIT_SEGMENTS/2)*date_increment;
					// date increments between points will not be completely constant though

					compute_trans_matrix(calc_date);
                    if (osculating_func) {
                      (*osculating_func)(date,calc_date,ecliptic_pos);
                    } else {
                      coord_func(calc_date, ecliptic_pos);
                    }
					orbit[d] = get_heliocentric_ecliptic_pos();
				}
				else
				{
					orbit[d] = orbit[d+delta_points];
				}
			}

			last_orbitJD = new_date;

		}
		else if( delta_points < 0 && abs(delta_points) < ORBIT_SEGMENTS  && orbit_cached)
		{

			for( int d=ORBIT_SEGMENTS-1; d>=0; d-- )
			{
				if(d + delta_points < 0 )
				{
					// calculate new points
					calc_date = new_date + (d-ORBIT_SEGMENTS/2)*date_increment;

					compute_trans_matrix(calc_date);
                    if (osculating_func) {
                      (*osculating_func)(date,calc_date,ecliptic_pos);
                    } else {
                      coord_func(calc_date, ecliptic_pos);
                    }
					orbit[d] = get_heliocentric_ecliptic_pos();
				}
				else
				{
					orbit[d] = orbit[d+delta_points];
				}
			}

			last_orbitJD = new_date;

		}
		else if( delta_points || !orbit_cached)
		{

			// update all points (less efficient)

			for( int d=0; d<ORBIT_SEGMENTS; d++ )
			{
				calc_date = date + (d-ORBIT_SEGMENTS/2)*date_increment;
				compute_trans_matrix(calc_date);
                if (osculating_func) {
                  (*osculating_func)(date,calc_date,ecliptic_pos);
                } else {
                  coord_func(calc_date, ecliptic_pos);
                }
				orbit[d] = get_heliocentric_ecliptic_pos();
			}

			last_orbitJD = date;
            if (!osculating_func) orbit_cached = 1;
		}


		// calculate actual Planet position
		coord_func(date, ecliptic_pos);

		lastJD = date;

	}
	else if (fabs(lastJD-date)>deltaJD)
	{

		// calculate actual Planet position
		coord_func(date, ecliptic_pos);
		lastJD = date;
	}

}

// Compute the transformation matrix from the local Planet coordinate to the parent Planet coordinate
void Planet::compute_trans_matrix(double jd)
{
	axis_rotation = getSiderealTime(jd);

	// Special case - heliocentric coordinates are on ecliptic,
    // not solar equator...
	if (parent) 
    {
      rot_local_to_parent = Mat4d::zrotation(re.ascendingNode
                                      -re.precessionRate*(jd-re.epoch))
                                      * Mat4d::xrotation(re.obliquity);
	}
    mat_local_to_parent = Mat4d::translation(ecliptic_pos)
                        * rot_local_to_parent;
                        
}

Mat4d Planet::getRotEquatorialToVsop87(void) const 
{
  Mat4d rval = rot_local_to_parent;
  if (parent)
  {
    for (const Planet *p=parent;p->parent;p=p->parent) 
    {
      rval = p->rot_local_to_parent * rval;
    }
  }

  return rval;
}

// Compute the z rotation to use from equatorial to geographic coordinates
double Planet::getSiderealTime(double jd) const
{
	if (englishName.CompareNoCase("Earth")) return get_apparent_sidereal_time(jd);

	double t = jd - re.epoch;
	double rotations = t / (double) re.period;
	double wholeRotations = floor(rotations);
	double remainder = rotations - wholeRotations;

	return remainder * 360. + re.offset;
}

// Get the Planet position in the parent Planet ecliptic coordinate
Vec3d Planet::get_ecliptic_pos() const
{
	return ecliptic_pos;
}

// Return the heliocentric ecliptical position
// used only for earth shadow, lunar eclipse
Vec3d Planet::get_heliocentric_ecliptic_pos() const
{
	Vec3d pos = ecliptic_pos;
	const Planet *p = parent;
	while (p!=NULL
	        && p->parent!=NULL)
	{
		pos += p->ecliptic_pos;
		p = p->parent;
		if (p->parent)
		{
			// a satellite has no satellites
			exit(128);
		}
	}
	return pos;
}

// Compute the distance to the given position in heliocentric coordinate (in AU)
double Planet::compute_distance(const Vec3d& obs_helio_pos)
{
	distance = (obs_helio_pos-get_heliocentric_ecliptic_pos()).length();
	return distance;
}

// Get the phase angle for an observer at pos obs_pos in the heliocentric coordinate (dist in AU)
double Planet::get_phase(Vec3d obs_pos) const {
  const double sq = obs_pos.lengthSquared();
  const Vec3d heliopos = get_heliocentric_ecliptic_pos();
  const double Rq = heliopos.lengthSquared();
  const double pq = (obs_pos - heliopos).lengthSquared();
  const double cos_chi = (pq + Rq - sq)/(2.0*sqrt(pq*Rq));
  return (1.0 - acos(cos_chi)/M_PI) * cos_chi
         + sqrt(1.0 - cos_chi*cos_chi) / M_PI;
}

float Planet::compute_magnitude(Vec3d obs_pos) const {
  const double sq = obs_pos.lengthSquared();
  if (parent == 0) {
      // sun
    return -26.73f + 2.5f*log10f(sq);
  }
  const Vec3d heliopos = get_heliocentric_ecliptic_pos();
  const double Rq = heliopos.lengthSquared();
  const double pq = (obs_pos - heliopos).lengthSquared();
  const double cos_chi = (pq + Rq - sq)/(2.0*sqrt(pq*Rq));
  const double phase = (1.0 - acos(cos_chi)/M_PI) * cos_chi
                       + sqrt(1.0 - cos_chi*cos_chi) / M_PI;
  const float F = 2.0 * albedo * radius * radius * phase / (3.0*pq*Rq);
  const float rval = -26.73f - 2.5f * log10f(F);

  return rval;
}

float Planet::compute_magnitude(const Navigator * nav) const
{
	return compute_magnitude(nav->getObserverHelioPos());
}

void Planet::set_big_halo(csString halotexfile)
{
/*
	StelApp::getInstance().getTextureManager().setDefaultParams();
	tex_big_halo = &StelApp::getInstance().getTextureManager().createTexture(halotexfile);
*/
}

// Return the radius of a circle containing the object on screen
float Planet::get_on_screen_size(const Projector* prj, const Navigator * nav)
{
	double rad;
	if(rings) rad = rings->get_size();
	else rad = radius;

	return atan(rad*sphere_scale*2.f/get_earth_equ_pos(nav).length())*180./M_PI/prj->get_fov()*prj->getViewportHeight();
}

// Draw the Planet and all the related infos : name, circle etc..
double Planet::draw(Projector* prj, Navigator * nav, ToneReproductor* eye, int flag_point, bool stencil)
{
	if (hidden) return 0;

	Mat4d mat = mat_local_to_parent;
	const Planet *p = parent;
	while (p && p->parent)
	{
      mat = Mat4d::translation(p->ecliptic_pos)
		    * mat
		    * p->rot_local_to_parent;
		p = p->parent;
	}

	// This removed totally the Planet shaking bug!!!
	mat = nav->get_helio_to_eye_mat() * mat;

	if (this == nav->getHomePlanet()) {
		if (rings) rings->draw(prj,mat,1000.0);
		return 0;
	}


	// Compute the 2D position and check if in the screen
	float screen_sz = get_on_screen_size(prj, nav);
	float viewport_left = prj->getViewportPosX();
	float viewport_bottom = prj->getViewportPosY();
	if (prj->project_custom(Vec3d(0,0,0), screenPos, mat) &&
	        screenPos[1]>viewport_bottom - screen_sz && screenPos[1]<viewport_bottom + prj->getViewportHeight()+screen_sz &&
	        screenPos[0]>viewport_left - screen_sz && screenPos[0]<viewport_left + prj->getViewportWidth() + screen_sz)
	{
		// Draw the name, and the circle if it's not too close from the body it's turning around
		// this prevents name overlaping (ie for jupiter satellites)
		float ang_dist = 300.f*atan(get_ecliptic_pos().length()/get_earth_equ_pos(nav).length())/prj->get_fov();
		if (ang_dist==0.f) ang_dist = 1.f; // if ang_dist == 0, the Planet is sun..

		// by putting here, only draw orbit if Planet is visible for clarity
		draw_orbit(nav, prj);  // TODO - fade in here also...

		draw_trail(nav, prj);
/* TODO
		if (ang_dist>0.25)
		{
			if (ang_dist>1.f) ang_dist = 1.f;
			draw_hints(nav, prj);
		}

		if (rings && screen_sz>1)
		{
			const double dist = get_earth_equ_pos(nav).length();
			double n,f;
			prj->get_clipping_planes(&n,&f); // Save clipping planes
			  // If z_near is too big, then Saturn and the rings are clipped
			  // in perspective projection
			  // when near the edge of the screen (home_planet=Hyperion).
			  // If z_near is too small, the depth test does not work properly
			  // when seen from great distance.
			double z_near = dist-rings->get_size()*2;
			if (z_near < 0.001) z_near = 0.0000001;
			else if (z_near < 0.05) z_near *= 0.1;
			else if (z_near < 0.5) z_near *= 0.5;
			prj->set_clipping_planes(z_near, dist+rings->get_size()*10);
			glClear(GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);
			draw_sphere(prj,mat,screen_sz);
			rings->draw(prj,mat,screen_sz);
			glDisable(GL_DEPTH_TEST);
			prj->set_clipping_planes(n,f);  // Restore old clipping planes
		}
		else
		{
			if(stencil) glEnable(GL_STENCIL_TEST);
			draw_sphere(prj, mat, screen_sz);
			if(stencil) glDisable(GL_STENCIL_TEST);
		}

		if (tex_halo)
		{
			if (flag_point) draw_point_halo(nav, prj, eye);
			else draw_halo(nav, prj, eye);
		}
		if (tex_big_halo) draw_big_halo(nav, prj, eye);

*/
        }

	double distanceSquared =
		(screenPos[0] - previousScreenPos[0]) *
		(screenPos[0] - previousScreenPos[0]) +
		(screenPos[1] - previousScreenPos[1]) *
		(screenPos[1] - previousScreenPos[1]);
	previousScreenPos = screenPos;
	return distanceSquared;
}


void Planet::draw_sphere(const Projector* prj, const Mat4d& mat, float screen_sz)
{
  /*
	// Adapt the number of facets according with the size of the sphere for optimization
	int nb_facet = (int)(screen_sz * 40/50);	// 40 facets for 1024 pixels diameter on screen
	if (nb_facet<10) nb_facet = 10;
	if (nb_facet>40) nb_facet = 40;

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);

	if (flag_lighting) glEnable(GL_LIGHTING);
	else
	{
		glDisable(GL_LIGHTING);
		glColor3fv(color);
	}
	tex_map->bind();


    // Rotate and add an extra quarter rotation so that the planet texture map
    // fits to the observers position. No idea why this is necessary,
    // perhaps some openGl strangeness, or confusing sin/cos.
    prj->sSphere(radius*sphere_scale, one_minus_oblateness, nb_facet, nb_facet,
                 mat * Mat4d::zrotation(M_PI/180*(axis_rotation + 90.)));

	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);
*/
}

void Planet::draw_halo(const Navigator* nav, const Projector* prj, const ToneReproductor* eye)
{
  /*
	float cmag;
	float rmag;

    float fov_q = prj->get_fov();
    if (fov_q > 60) fov_q = 60;
    else if (fov_q < 0.1) fov_q = 0.1;
    fov_q = 1.f/(fov_q*fov_q);
    rmag = std::sqrt(eye->adapt_luminance(
        std::exp(-0.92103f*(compute_magnitude(nav->getObserverHelioPos())
                         + 12.12331f)) * 108064.73f * fov_q)) * 30.f;

	cmag = 1.f;

	// if size of star is too small (blink) we put its size to 1.2 --> no more blink
	// And we compensate the difference of brighteness with cmag
	if (rmag<1.2f)
	{
		if (rmag<0.3f) return;
		cmag=rmag*rmag/1.44f;
		rmag=1.2f;
	}
	else
	{
  if (rmag>8.f) {
    rmag=8.f+2.f*std::sqrt(1.f+rmag-8.f)-2.f;
  }

	}


	// Global scaling
	rmag*=Planet::object_scale;

	glBlendFunc(GL_ONE, GL_ONE);
	float screen_r = get_on_screen_size(prj, nav);
if (screen_r<1.f) screen_r=1.f;
	cmag *= 0.5*rmag/(screen_r*screen_r*screen_r);
	if (cmag>1.f) cmag = 1.f;

	if (rmag<screen_r)
	{
		cmag*=rmag/screen_r;
		rmag = screen_r;
	}

	prj->set_orthographic_projection();    	// 2D coordinate

	tex_halo->bind();
	glEnable(GL_BLEND);
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glColor3f(color[0]*cmag, color[1]*cmag, color[2]*cmag);
	glTranslatef(screenPos[0], screenPos[1], 0.f);
	glBegin(GL_QUADS);
	glTexCoord2i(0,0);	glVertex3f(-rmag, rmag,0.f);	// Bottom Left
	glTexCoord2i(1,0);	glVertex3f( rmag, rmag,0.f);	// Bottom Right
	glTexCoord2i(1,1);	glVertex3f( rmag,-rmag,0.f);	// Top Right
	glTexCoord2i(0,1);	glVertex3f(-rmag,-rmag,0.f);	// Top Left
	glEnd();

	prj->reset_perspective_projection();		// Restore the other coordinate
*/
}

void Planet::draw_point_halo(const Navigator* nav, const Projector* prj, const ToneReproductor* eye)
{
  /*
	float cmag;
	float rmag;

    float fov_q = prj->get_fov();
    if (fov_q > 60) fov_q = 60;
    fov_q = 1.f/(fov_q*fov_q);
    rmag =
      std::sqrt(eye->adapt_luminance(
        std::exp(-0.92103f*(compute_magnitude(nav->getObserverHelioPos())
                         + 12.12331f)) * 108064.73f * fov_q)) * 6.f;

	cmag = 1.f;

	// if size of star is too small (blink) we put its size to 1.2 --> no more blink
	// And we compensate the difference of brighteness with cmag
	if (rmag<0.3f) return;
	cmag=rmag*rmag/(1.4f*1.4f);
	rmag=1.4f;

	// Global scaling
	//rmag*=Planet::star_scale;

	glBlendFunc(GL_ONE, GL_ONE);
	float screen_r = get_on_screen_size(prj, nav);
	cmag *= rmag/screen_r;
	if (cmag>1.f) cmag = 1.f;

	if (rmag<screen_r)
	{
		cmag*=rmag/screen_r;
		rmag = screen_r;
	}
	prj->set_orthographic_projection();    	// 2D coordinate

	tex_halo->bind();
	glEnable(GL_BLEND);
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glColor3f(color[0]*cmag, color[1]*cmag, color[2]*cmag);
	glTranslatef(screenPos[0], screenPos[1], 0.f);
	glBegin(GL_QUADS);
	glTexCoord2i(0,0);	glVertex3f(-rmag, rmag,0.f);	// Bottom Left
	glTexCoord2i(1,0);	glVertex3f( rmag, rmag,0.f);	// Bottom Right
	glTexCoord2i(1,1);	glVertex3f( rmag,-rmag,0.f);	// Top Right
	glTexCoord2i(0,1);	glVertex3f(-rmag,-rmag,0.f);	// Top Left
	glEnd();

	prj->reset_perspective_projection();		// Restore the other coordinate
*/
}

void Planet::draw_big_halo(const Navigator* nav, const Projector* prj, const ToneReproductor* eye)
{
 /*
	glBlendFunc(GL_ONE, GL_ONE);
	float screen_r = get_on_screen_size(prj, nav);

	float rmag = big_halo_size/2;

	float cmag = rmag/screen_r;
	if (cmag>1.f) cmag = 1.f;

	if (rmag<screen_r*2)
	{
		cmag*=rmag/(screen_r*2);
		rmag = screen_r*2;
	}

	prj->set_orthographic_projection();    	// 2D coordinate

	tex_big_halo->bind();
	glEnable(GL_BLEND);
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glColor3f(color[0]*cmag, color[1]*cmag, color[2]*cmag);
	glTranslatef(screenPos[0], screenPos[1], 0.f);
	glBegin(GL_QUADS);
	glTexCoord2i(0,0);	glVertex3f(-rmag, rmag,0.f);	// Bottom Left
	glTexCoord2i(1,0);	glVertex3f( rmag, rmag,0.f);	// Bottom Right
	glTexCoord2i(1,1);	glVertex3f( rmag,-rmag,0.f);	// Top Right
	glTexCoord2i(0,1);	glVertex3f(-rmag,-rmag,0.f);	// Top Left
	glEnd();

	prj->reset_perspective_projection();		// Restore the other coordinate
*/
}

Ring::Ring(double radius_min,double radius_max,const string &texname)
     :radius_min(radius_min),radius_max(radius_max)
{
	//tex = &StelApp::getInstance().getTextureManager().createTexture(texname);
}

Ring::~Ring(void) {
	//if (tex) delete tex;
	//tex = NULL;
}

void Ring::draw(const Projector* prj,const Mat4d& mat,double screen_sz)
{
 /*
	screen_sz -= 50;
	screen_sz /= 250.0;
	if (screen_sz < 0.0) screen_sz = 0.0;
	else if (screen_sz > 1.0) screen_sz = 1.0;
	const int slices = 128+(int)((256-128)*screen_sz);
	const int stacks = 8+(int)((32-8)*screen_sz);

	// Normal transparency mode
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glRotatef(axis_rotation + 180.,0.,0.,1.);
	glColor3f(1.0f, 0.88f, 0.82f); // For saturn only..
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);

	tex->bind();

	// TODO: radial texture would look much better

	  // solve the ring wraparound by culling:
	  // decide if we are above or below the ring plane
	const double h = mat.r[ 8]*mat.r[12]
	               + mat.r[ 9]*mat.r[13]
	               + mat.r[10]*mat.r[14];
	prj->sRing(radius_min,radius_max,(h<0.0)?slices:-slices,stacks, mat, 0);
	glDisable(GL_CULL_FACE);

*/
}


// draw orbital path of Planet
void Planet::draw_orbit(const Navigator * nav, const Projector* prj)
{
/*
	if(!orbit_fader.getInterstate()) return;

	Vec3d onscreen;

	if(!re.sidereal_period) return;

	prj->set_orthographic_projection();    // 2D coordinate

	// Normal transparency mode
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_BLEND);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

	glColor4f(orbit_color[0], orbit_color[1], orbit_color[2], orbit_fader.getInterstate());

	int on=0;
	int d;
	for( int n=0; n<=ORBIT_SEGMENTS; n++)
	{

		if( n==ORBIT_SEGMENTS )
		{
			d = 0;  // connect loop
		}
		else
		{
			d = n;
		}

		// special case - use current Planet position as center vertex so that draws
		// on it's orbit all the time (since segmented rather than smooth curve)
		if( n == ORBIT_SEGMENTS/2 )
		{

			if(prj->project_helio(get_heliocentric_ecliptic_pos(), onscreen))
			{
				if(!on) glBegin(GL_LINE_STRIP);
				glVertex3d(onscreen[0], onscreen[1], 0);
				on=1;
			}
			else if( on )
			{
				glEnd();
				on=0;
			}
		}
		else
		{

			if(prj->project_helio(orbit[d],onscreen))
			{
				if(!on) glBegin(GL_LINE_STRIP);
				glVertex3d(onscreen[0], onscreen[1], 0);
				on=1;
			}
			else if( on )
			{
				glEnd();
				on=0;
			}
		}
	}

	if(on) glEnd();


	prj->reset_perspective_projection();		// Restore the other coordinate

	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
 */
}


// draw trail of Planet as seen from earth
void Planet::draw_trail(const Navigator * nav, const Projector* prj)
{
/*
	if(!trail_fader.getInterstate()) return;

	Vec3d onscreen1;
	Vec3d onscreen2;

	//  if(!re.sidereal_period) return;   // limits to planets

	prj->set_orthographic_projection();    // 2D coordinate

	glEnable(GL_BLEND);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

	glColor3fv(trail_color*trail_fader.getInterstate());

	list<TrailPoint>::iterator iter;
	list<TrailPoint>::iterator nextiter;
	list<TrailPoint>::iterator begin = trail.begin();
	//  begin++;

	if(trail.begin() != trail.end())
	{

		nextiter = trail.end();
		nextiter--;

		for( iter=nextiter; iter != begin; iter--)
		{

			nextiter--;
			if( prj->project_earth_equ_line_check( (*iter).point, onscreen1, (*(nextiter)).point, onscreen2) )
			{
				glBegin(GL_LINE_STRIP);
				glVertex3d(onscreen1[0], onscreen1[1], 0);
				glVertex3d(onscreen2[0], onscreen2[1], 0);
				glEnd();
			}
		}
	}

	// draw final segment to finish at current Planet position
	if( !first_point && prj->project_earth_equ_line_check( (*trail.begin()).point, onscreen1, get_earth_equ_pos(nav), onscreen2) )
	{
		glBegin(GL_LINE_STRIP);
		glVertex3d(onscreen1[0], onscreen1[1], 0);
		glVertex3d(onscreen2[0], onscreen2[1], 0);
		glEnd();
	}


	prj->reset_perspective_projection();		// Restore the other coordinate

	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
*/
}

// update trail points as needed
void Planet::update_trail(const Navigator* nav)
{
	if(!trail_on) return;

	double date = nav->getJDay();

	int dt=0;
	if(first_point || (dt=abs(int((date-last_trailJD)/DeltaTrail))) > MaxTrail)
	{
		dt=1;
		// clear old trail
		trail.clear();
		first_point = 0;
	}

	// Note that when jump by a week or day at a time, loose detail on trails
	// particularly for moon (if decide to show moon trail)

	// add only one point at a time, using current position only
	if(dt)
	{
		last_trailJD = date;
		TrailPoint tp;
		Vec3d v = get_heliocentric_ecliptic_pos();
		//      trail.push_front( nav->helio_to_earth_equ(v) );  // centered on earth
		tp.point = nav->helio_to_earth_pos_equ(v);
		tp.date = date;
		trail.push_front( tp );

		//      if( trail.size() > (unsigned int)MaxTrail ) {
		if( trail.size() > (unsigned int)MaxTrail )
		{
			trail.pop_back();
		}
	}

	// because sampling depends on speed and frame rate, need to clear out
	// points if trail gets longer than desired

	list<TrailPoint>::iterator iter;
	list<TrailPoint>::iterator end = trail.end();

	for( iter=trail.begin(); iter != end; iter++)
	{
		if( fabs((*iter).date - date)/DeltaTrail > MaxTrail )
		{
			trail.erase(iter, end);
			break;
		}
	}
}

// Start/stop accumulating new trail data (clear old data)
void Planet::startTrail(bool b)
{
	if (b)
	{
		first_point = 1;
		//  printf("trail for %s: %f\n", name.c_str(), re.sidereal_period);
		// only interested in trails for planets
		if(re.sidereal_period > 0) trail_on = 1;
	}
	else
	{
		trail_on = 0;
	}
}

void Planet::update(int delta_time)
{
	//hint_fader.update(delta_time);
	//orbit_fader.update(delta_time);
	//trail_fader.update(delta_time);
}

///////////////////////////////////////////////////////////////////
/* puts a large angle in the correct range 0 - 360 degrees */
double range_degrees(double d)
{
	d = fmod( d, 360.);
	if(d<0.) d += 360.;
	return d;
}

/* puts a large angle in the correct range 0 - 2PI radians */
double range_radians (double r)
{
	r = fmod( r, 2.*M_PI );
	if (r<0.) r += 2.*M_PI;
	return r;
}


#define TERMS 63
#define LN_NUTATION_EPOCH_THRESHOLD 0.1


/* Nutation is a period oscillation of the Earths rotational axis around it's mean position.*/

// Contains Nutation in longitude, obliquity and ecliptic obliquity.
// Angles are expressed in degrees.
struct ln_nutation
{
	double longitude;	/*!< Nutation in longitude */
	double obliquity;	/*!< Nutation in obliquity */
	double ecliptic;	/*!< Obliquity of the ecliptic */
};

struct nutation_arguments
{
	double D;
	double M;
	double MM;
	double F;
	double O;
};

struct nutation_coefficients
{
	double longitude1;
	double longitude2;
	double obliquity1;
	double obliquity2;
};

/* arguments and coefficients taken from table 21A on page 133 */

const static struct nutation_arguments arguments[TERMS] = {
	{0.0,	0.0,	0.0,	0.0,	1.0},
	{-2.0,	0.0,	0.0,	2.0,	2.0},
	{0.0,	0.0,	0.0,	2.0,	2.0},
	{0.0,	0.0,	0.0,	0.0,	2.0},
	{0.0,	1.0,	0.0,	0.0,	0.0},
	{0.0,	0.0,	1.0,	0.0,	0.0},
	{-2.0,	1.0,	0.0,	2.0,	2.0},
	{0.0,	0.0,	0.0,	2.0,	1.0},
	{0.0,	0.0,	1.0,	2.0,	2.0},
	{-2.0,	-1.0,	0.0,	2.0,	2.0},
	{-2.0,	0.0,	1.0,	0.0,	0.0},
	{-2.0,	0.0,	0.0,	2.0,	1.0},
	{0.0,	0.0,	-1.0,	2.0,	2.0},
	{2.0,	0.0,	0.0,	0.0,	0.0},
	{0.0,	0.0,	1.0,	0.0,	1.0},
	{2.0,	0.0,	-1.0,	2.0,	2.0},
	{0.0,	0.0,	-1.0,	0.0,	1.0},
	{0.0,	0.0,	1.0,	2.0,	1.0},
	{-2.0,	0.0,	2.0,	0.0,	0.0},
	{0.0,	0.0,	-2.0,	2.0,	1.0},
	{2.0,	0.0,	0.0,	2.0,	2.0},
	{0.0,	0.0,	2.0,	2.0,	2.0},
	{0.0,	0.0,	2.0,	0.0,	0.0},
	{-2.0,	0.0,	1.0,	2.0,	2.0},
	{0.0,	0.0,	0.0,	2.0,	0.0},
	{-2.0,	0.0,	0.0,	2.0,	0.0},
	{0.0,	0.0,	-1.0,	2.0,	1.0},
	{0.0,	2.0,	0.0,	0.0,	0.0},
	{2.0,	0.0,	-1.0,	0.0,	1.0},
	{-2.0,	2.0,	0.0,	2.0,	2.0},
	{0.0,	1.0,	0.0,	0.0,	1.0},
	{-2.0,	0.0,	1.0,	0.0,	1.0},
	{0.0,	-1.0,	0.0,	0.0,	1.0},
	{0.0,	0.0,	2.0,	-2.0,	0.0},
	{2.0,	0.0,	-1.0,	2.0,	1.0},
	{2.0,	0.0,	1.0,	2.0,	2.0},
	{0.0,	1.0,	0.0,	2.0,	2.0},
	{-2.0,	1.0,	1.0,	0.0,	0.0},
	{0.0,	-1.0,	0.0,	2.0,	2.0},
	{2.0,	0.0,	0.0,	2.0,	1.0},
	{2.0,	0.0,	1.0,	0.0,	0.0},
	{-2.0,	0.0,	2.0,	2.0,	2.0},
	{-2.0,	0.0,	1.0,	2.0,	1.0},
	{2.0,	0.0,	-2.0,	0.0,	1.0},
	{2.0,	0.0,	0.0,	0.0,	1.0},
	{0.0,	-1.0,	1.0,	0.0,	0.0},
	{-2.0,	-1.0,	0.0,	2.0,	1.0},
	{-2.0,	0.0,	0.0,	0.0,	1.0},
	{0.0,	0.0,	2.0,	2.0,	1.0},
	{-2.0,	0.0,	2.0,	0.0,	1.0},
	{-2.0,	1.0,	0.0,	2.0,	1.0},
	{0.0,	0.0,	1.0,	-2.0,	0.0},
	{-1.0,	0.0,	1.0,	0.0,	0.0},
	{-2.0,	1.0,	0.0,	0.0,	0.0},
	{1.0,	0.0,	0.0,	0.0,	0.0},
	{0.0,	0.0,	1.0,	2.0,	0.0},
	{0.0,	0.0,	-2.0,	2.0,	2.0},
	{-1.0,	-1.0,	1.0,	0.0,	0.0},
	{0.0,	1.0,	1.0,	0.0,	0.0},
	{0.0,	-1.0,	1.0,	2.0,	2.0},
	{2.0,	-1.0,	-1.0,	2.0,	2.0},
	{0.0,	0.0,	3.0,	2.0,	2.0},
	{2.0,	-1.0,	0.0,	2.0,	2.0}};

const static struct nutation_coefficients coefficients[TERMS] = {
	{-171996.0,	-174.2,	92025.0,8.9},
	{-13187.0,	-1.6,  	5736.0,	-3.1},
	{-2274.0, 	 0.2,  	977.0,	-0.5},
	{2062.0,   	0.2,    -895.0,    0.5},
	{1426.0,    -3.4,    54.0,    -0.1},
	{712.0,    0.1,    -7.0,    0.0},
	{-517.0,    1.2,    224.0,    -0.6},
	{-386.0,    -0.4,    200.0,    0.0},
	{-301.0,    0.0,    129.0,    -0.1},
	{217.0,    -0.5,    -95.0,    0.3},
	{-158.0,    0.0,    0.0,    0.0},
	{129.0,	0.1,	-70.0,	0.0},
	{123.0,	0.0,	-53.0,	0.0},
	{63.0,	0.0,	0.0,	0.0},
	{63.0,	1.0,	-33.0,	0.0},
	{-59.0,	0.0,	26.0,	0.0},
	{-58.0,	-0.1,	32.0,	0.0},
	{-51.0,	0.0,	27.0,	0.0},
	{48.0,	0.0,	0.0,	0.0},
	{46.0,	0.0,	-24.0,	0.0},
	{-38.0,	0.0,	16.0,	0.0},
	{-31.0,	0.0,	13.0,	0.0},
	{29.0,	0.0,	0.0,	0.0},
	{29.0,	0.0,	-12.0,	0.0},
	{26.0,	0.0,	0.0,	0.0},
	{-22.0,	0.0,	0.0,	0.0},
	{21.0,	0.0,	-10.0,	0.0},
	{17.0,	-0.1,	0.0,	0.0},
	{16.0,	0.0,	-8.0,	0.0},
	{-16.0,	0.1,	7.0,	0.0},
	{-15.0,	0.0,	9.0,	0.0},
	{-13.0,	0.0,	7.0,	0.0},
	{-12.0,	0.0,	6.0,	0.0},
	{11.0,	0.0,	0.0,	0.0},
	{-10.0,	0.0,	5.0,	0.0},
	{-8.0,	0.0,	3.0,	0.0},
	{7.0,	0.0,	-3.0,	0.0},
	{-7.0,	0.0,	0.0,	0.0},
	{-7.0,	0.0,	3.0,	0.0},
	{-7.0,	0.0,	3.0,	0.0},
	{6.0,	0.0,	0.0,	0.0},
	{6.0,	0.0,	-3.0,	0.0},
	{6.0,	0.0,	-3.0,	0.0},
	{-6.0,	0.0,	3.0,	0.0},
	{-6.0,	0.0,	3.0,	0.0},
	{5.0,	0.0,	0.0,	0.0},
	{-5.0,	0.0,	3.0,	0.0},
	{-5.0,	0.0,	3.0,	0.0},
	{-5.0,	0.0,	3.0,	0.0},
	{4.0,	0.0,	0.0,	0.0},
	{4.0,	0.0,	0.0,	0.0},
	{4.0,	0.0,	0.0,	0.0},
	{-4.0,	0.0,	0.0,	0.0},
	{-4.0,	0.0,	0.0,	0.0},
	{-4.0,	0.0,	0.0,	0.0},
	{3.0,	0.0,	0.0,	0.0},
	{-3.0,	0.0,	0.0,	0.0},
	{-3.0,	0.0,	0.0,	0.0},
	{-3.0,	0.0,	0.0,	0.0},
	{-3.0,	0.0,	0.0,	0.0},
	{-3.0,	0.0,	0.0,	0.0},
	{-3.0,	0.0,	0.0,	0.0},
	{-3.0,	0.0,	0.0,	0.0}};

/* cache values */
static double c_JD = 0.0, c_longitude = 0.0, c_obliquity = 0.0, c_ecliptic = 0.0;


/* Calculate nutation of longitude and obliquity in degrees from Julian Ephemeris Day
* params : JD Julian Day, nutation Pointer to store nutation.
* Chapter 21 pg 131-134 Using Table 21A */
void get_nutation (double JD, struct ln_nutation * nutation)
{

	double D,M,MM,F,O,T,T2,T3;
	double coeff_sine, coeff_cos;
	int i;

	/* should we bother recalculating nutation */
	if (fabs(JD - c_JD) > LN_NUTATION_EPOCH_THRESHOLD)
	{
		/* set the new epoch */
		c_JD = JD;

		/* set ecliptic */
		c_ecliptic = 23.0 + 26.0 / 60.0 + 27.407 / 3600.0;

		/* calc T */
		T = (JD - 2451545.0)/36525;
		T2 = T * T;
		T3 = T2 * T;

		/* calculate D,M,M',F and Omega */
		D = 297.85036 + 445267.111480 * T - 0.0019142 * T2 + T3 / 189474.0;
		M = 357.52772 + 35999.050340 * T - 0.0001603 * T2 - T3 / 300000.0;
		MM = 134.96298 + 477198.867398 * T + 0.0086972 * T2 + T3 / 56250.0;
		F = 93.2719100 + 483202.017538 * T - 0.0036825 * T2 + T3 / 327270.0;
		O = 125.04452 - 1934.136261 * T + 0.0020708 * T2 + T3 / 450000.0;

		/* convert to radians */
		D *= M_PI/180.;
		M *= M_PI/180.;
		MM *= M_PI/180.;
		F *= M_PI/180.;
		O *= M_PI/180.;

		/* calc sum of terms in table 21A */
		for (i=0; i< TERMS; i++)
		{
			/* calc coefficients of sine and cosine */
			coeff_sine = (coefficients[i].longitude1 + (coefficients[i].longitude2 * T));
			coeff_cos = (coefficients[i].obliquity1 + (coefficients[i].obliquity2 * T));

			/* sum the arguments */
			if (arguments[i].D != 0)
			{
				c_longitude += coeff_sine * (sin (arguments[i].D * D));
				c_obliquity += coeff_cos * (cos (arguments[i].D * D));
			}
			if (arguments[i].M != 0)
			{
				c_longitude += coeff_sine * (sin (arguments[i].M * M));
				c_obliquity += coeff_cos * (cos (arguments[i].M * M));
			}
			if (arguments[i].MM != 0)
			{
				c_longitude += coeff_sine * (sin (arguments[i].MM * MM));
				c_obliquity += coeff_cos * (cos (arguments[i].MM * MM));
			}
			if (arguments[i].F != 0)
			{
				c_longitude += coeff_sine * (sin (arguments[i].F * F));
				c_obliquity += coeff_cos * (cos (arguments[i].F * F));
			}
			if (arguments[i].O != 0)
			{
				c_longitude += coeff_sine * (sin (arguments[i].O * O));
				c_obliquity += coeff_cos * (cos (arguments[i].O * O));
			}
		}

		/* change to degrees */
		c_longitude /= 36000000.;
		c_obliquity /= 36000000.;

		c_ecliptic += c_obliquity;
	}

	/* return results */
	nutation->longitude = c_longitude;
	nutation->obliquity = c_obliquity;
	nutation->ecliptic = c_ecliptic;
}

/* Calculate the mean sidereal time at the meridian of Greenwich of a given date.
 * returns apparent sidereal time (degree).
 * Formula 11.1, 11.4 pg 83 */
double get_mean_sidereal_time (double JD)
{
    double sidereal;
    double T;

    T = (JD - 2451545.0) / 36525.0;

    /* calc mean angle */
    sidereal = 280.46061837 + (360.98564736629 * (JD - 2451545.0)) + (0.000387933 * T * T) - (T * T * T / 38710000.0);

    /* add a convenient multiple of 360 degrees */
    sidereal = range_degrees (sidereal);

    return sidereal;
}


/* Calculate the apparent sidereal time at the meridian of Greenwich of a given date.
 * returns apparent sidereal time (degree).
 * Formula 11.1, 11.4 pg 83 */
double get_apparent_sidereal_time (double JD)
{
   double correction, sidereal;
   struct ln_nutation nutation;

   /* get the mean sidereal time */
   sidereal = get_mean_sidereal_time (JD);

   /* add corrections for nutation in longitude and for the true obliquity of
   the ecliptic */
   get_nutation (JD, &nutation);

   correction = (nutation.longitude * cos (nutation.obliquity*M_PI/180.));

   sidereal += correction;

   return (sidereal);
}
