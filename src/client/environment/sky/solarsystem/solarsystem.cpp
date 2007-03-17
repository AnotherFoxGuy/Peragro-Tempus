
#include <algorithm>
#include <iostream>
#include <string>

#include "solarsystem.h"

#include "client/environment/sky/solarsystem/coordinates/coordinates.h"

#include "client/environment/sky/solarsystem/orbit.h"


SolarSystem::SolarSystem()
	:sun(NULL),moon(NULL),earth(NULL), moonScale(1.), flagOrbits(false),flag_light_travel_time(false)
{
	flagPoint = false;
}

SolarSystem::~SolarSystem()
{
  /*
	for(vector<Planet*>::iterator iter = system_planets.begin(); iter != system_planets.end(); ++iter)
	{
		if (*iter) delete *iter;
		*iter = NULL;
	}
	for(vector<EllipticalOrbit*>::iterator iter = ell_orbits.begin(); iter != ell_orbits.end(); ++iter)
	{
		if (*iter) delete *iter;
		*iter = NULL;
	}
	sun = NULL;
	moon = NULL;
	earth = NULL;
    */

	//if(tex_earth_shadow) delete tex_earth_shadow;
}


// Init and load the solar system data
void SolarSystem::init(/*const InitParser& conf*/)
{

	loadPlanets();	// Load planets data

	// Compute position and matrix of sun and all the satellites (ie planets)
	// for the first initialization assert that center is sun center (only impacts on light speed correction)
	computePositions(get_julian_from_sys());
/*
	setSelected("");	// Fix a bug on macosX! Thanks Fumio!
	setScale(conf.get_double ("stars:star_scale"));  // if reload config
	setFlagMoonScale(conf.get_boolean("viewing", "flag_moon_scaled", conf.get_boolean("viewing", "flag_init_moon_scaled", false)));  // name change
	setMoonScale(conf.get_double ("viewing","moon_scale",5.));
	setFlagPlanets(conf.get_boolean("astro:flag_planets"));
	setFlagHints(conf.get_boolean("astro:flag_planets_hints"));
	setFlagOrbits(conf.get_boolean("astro:flag_planets_orbits"));
	setFlagLightTravelTime(conf.get_boolean("astro:flag_light_travel_time"));
	setFlagTrails(conf.get_boolean("astro", "flag_object_trails", false));
	startTrails(conf.get_boolean("astro", "flag_object_trails", false));	
	setFlagPoint(conf.get_boolean("stars:flag_point_star"));
*/
}

// Init and load the solar system data
void SolarSystem::loadPlanets()
{
  iObjectRegistry* obj_reg = PointerLibrary::getInstance()->getObjectRegistry();
  csRef<iDocumentSystem> docsys = csQueryRegistry<iDocumentSystem> (obj_reg);
  csRef<iVFS> vfs = csQueryRegistry<iVFS> (obj_reg);

  csRef<iFile> buf = vfs->Open("/peragro/xml/environment/ssystem.xml", VFS_FILE_READ);
  if (!buf)
  {
    printf("SolarSystem: ERROR Couldn't open ssystem file!\n");
    return;
  }

  csRef<iDocument> doc;
  csRef<iDocumentNode> node;
  
  if (!docsys) docsys = csPtr<iDocumentSystem> (new csTinyDocumentSystem ());
  doc = docsys->CreateDocument ();
  const char* error = doc->Parse (buf, true);
  if (error != 0)
  {
    printf("SolarSystem: ERROR Couldn't open ssystem file!\n");
    return;
  }

  if (doc)
  {
    node = doc->GetRoot()->GetNode("planets");
    if (!node)
    {
      printf("SolarSystem: ERROR Couldn't open ssystem file!\n");
      return;
    }
  }

  printf("\n==Loading planets==========================\n");
  csRef<iDocumentNodeIterator> it = node->GetNodes ();
  while (it->HasNext ())
  {
    // Standard attributes.
    csRef<iDocumentNode> child = it->Next ();
    csString name                       = child->GetAttributeValue("name");
    csRef<iDocumentNode> hidden         = child->GetNode ("hidden");
    csRef<iDocumentNode> parent         = child->GetNode ("parent");
    csRef<iDocumentNode> halo           = child->GetNode ("halo");
    csRef<iDocumentNode> lighting       = child->GetNode ("lighting");
    csRef<iDocumentNode> radius         = child->GetNode ("radius");
    csRef<iDocumentNode> oblateness     = child->GetNode ("oblateness");
    csRef<iDocumentNode> color          = child->GetNode ("color");
    csRef<iDocumentNode> albedo         = child->GetNode ("albedo");
    csRef<iDocumentNode> tex_map        = child->GetNode ("tex_map");
    csRef<iDocumentNode> tex_halo       = child->GetNode ("tex_halo");
    csRef<iDocumentNode> tex_big_halo   = child->GetNode ("tex_big_halo");
    csRef<iDocumentNode> big_halo_size   = child->GetNode ("big_halo_size");
    csRef<iDocumentNode> coord_func     = child->GetNode ("coord_func");

    // Rotations.
    csRef<iDocumentNode> rot_periode           = child->GetNode ("rot_periode");
    csRef<iDocumentNode> rot_rotation_offset   = child->GetNode ("rot_rotation_offset");
    csRef<iDocumentNode> rot_epoch             = child->GetNode ("rot_epoch");
    csRef<iDocumentNode> rot_obliquity         = child->GetNode ("rot_obliquity");
    csRef<iDocumentNode> rot_equator_ascending = child->GetNode ("rot_equator_ascending");
    csRef<iDocumentNode> rot_precession_rate   = child->GetNode ("rot_precession_rate");
    csRef<iDocumentNode> sidereal_period       = child->GetNode ("sidereal_period");

    // Not a planet tag.
    if (!hidden || !parent) continue;

    printf("Processing %s...", name.GetData());

    // Init some stuff
    Planet* planetparent = 0;
    pos_func_type posfunc;
    OsulatingFunctType* osculating_func = 0;
    EllipticalOrbit* ell_orbit = 0;

    // Parent.
    csString str_parent = parent->GetAttributeValue("value");
    if (!str_parent.CompareNoCase("none"))
    {
      // Look in the other planets for the one named with str_parent
      vector<Planet*>::iterator iter = system_planets.begin();
      while (iter != system_planets.end())
      {
        csString parentname = (*iter)->getEnglishName();
        if (parentname.CompareNoCase(str_parent))
        {
          planetparent = (*iter);
        }
        iter++;
      }
      if (!planetparent)
      {
        printf("ERROR : can't find parent for %s\n", name.GetData());
        continue;
      }
    }
    printf(" Parent: %s", str_parent.GetData());

    // Orbit.
    csString coord_funcstring = "ell_orbit";
    csString coord_funcvalue = coord_func->GetAttributeValue("value");
    if (coord_funcstring.CompareNoCase(coord_funcvalue))
    {
      csRef<iDocumentNode> orbit = child->GetNode("orbit");
      ell_orbit = GetOrbit(planetparent, orbit);
      if (!ell_orbit) 
      {
        printf(", %s: \n", name.GetData());
        continue;
      }
      ell_orbits.push_back(ell_orbit);

      posfunc = pos_func_type(ell_orbit, &EllipticalOrbit::positionAtTimevInVSOP87Coordinates);
    }
    else
    {
      GetPosAndOsc(coord_funcvalue, posfunc, osculating_func);
      if (posfunc.empty())
      {
        printf("ERROR: %s: Couldn't get pos_func_type and OsulatingFunctType! \n", name.GetData());
        continue;
      }
    }

    // Rotation elements.
    double rot_periodevalue           = rot_periode->GetAttributeValueAsFloat("value")/24;
    double rot_rotation_offsetvalue   = rot_rotation_offset->GetAttributeValueAsFloat("value");
    double rot_epochvalue             = rot_epoch->GetAttributeValueAsFloat("value");
    double rot_obliquityvalue         = rot_obliquity->GetAttributeValueAsFloat("value")*M_PI/180;
    double rot_equator_ascendingvalue = rot_equator_ascending->GetAttributeValueAsFloat("value")*M_PI/180;
    double rot_precession_ratevalue   = rot_precession_rate->GetAttributeValueAsFloat("value")*M_PI/(180*36525);
    double sidereal_periodvalue       = sidereal_period->GetAttributeValueAsFloat("value");



    // Values.
    bool hiddenvalue             = hidden->GetAttributeValueAsBool("value");
    bool halovalue               = halo->GetAttributeValueAsBool("value");
    bool lightingvalue           = lighting->GetAttributeValueAsBool("value");
    double radiusvalue           = radius->GetAttributeValueAsFloat("value")/AU;
    double oblatenessvalue       = oblateness->GetAttributeValueAsFloat("value");
    float albedovalue            = albedo->GetAttributeValueAsFloat("value");
    csString tex_mapvalue        = tex_map->GetAttributeValue("value");
    csString tex_halovalue       = tex_halo->GetAttributeValue("value");
    csString tex_big_halovalue   = tex_big_halo->GetAttributeValue("value");
    double big_halo_sizevalue    = big_halo_size->GetAttributeValueAsFloat("value");

    Vec3f colorvalue;
    colorvalue[0] = (color->GetAttributeValueAsFloat("red"));
    colorvalue[1] = (color->GetAttributeValueAsFloat("green")); 
    colorvalue[2] = (color->GetAttributeValueAsFloat("blue")); 


    Planet* planet = new Planet(planetparent,
                                name,
                                halovalue,
                                lightingvalue,
                                radiusvalue,
                                oblatenessvalue,
                                colorvalue,
                                albedovalue,
                                tex_mapvalue,
                                tex_halovalue,
                                posfunc,
                                osculating_func,
		                        hiddenvalue);

    // Set the rotation elements.
    planet->set_rotation_elements(rot_periodevalue,
		                          rot_rotation_offsetvalue,
		                          rot_epochvalue,
		                          rot_obliquityvalue,
		                          rot_equator_ascendingvalue,
		                          rot_precession_ratevalue,
		                          sidereal_periodvalue );

    // In case of a big halo.
    if (!tex_big_halovalue.CompareNoCase("none"))
    {
      planet->set_big_halo(tex_big_halovalue);
      planet->set_halo_size(big_halo_sizevalue);
    }

    // If it's special.
    if (name.CompareNoCase("earth")) earth = planet;
    if (name.CompareNoCase("sun")) sun = planet;
    if (name.CompareNoCase("moon")) moon = planet;


    // Add the planet to the list.
    system_planets.push_back(planet);
    printf(" Done\n");

  } // end while


  printf("================================= %d Planets(s)\n\n", system_planets.size());

}

void SolarSystem::GetPosAndOsc(csString name,  pos_func_type &posfunc, OsulatingFunctType* &osculating_func)
{
  if (name.CompareNoCase("sun_special"))
    posfunc = pos_func_type(get_sun_helio_coordsv);

  if (name.CompareNoCase("mercury_special"))
  {
    posfunc = pos_func_type(get_mercury_helio_coordsv);
    osculating_func = &get_mercury_helio_osculating_coords;
  }

  if (name.CompareNoCase("venus_special")) 
  {
    posfunc = pos_func_type(get_venus_helio_coordsv);
    osculating_func = &get_venus_helio_osculating_coords;
  }

  if (name.CompareNoCase("earth_special")) 
  {
    posfunc = pos_func_type(get_earth_helio_coordsv);
    osculating_func = &get_earth_helio_osculating_coords;
  }

  if (name.CompareNoCase("lunar_special"))
    posfunc = pos_func_type(get_lunar_parent_coordsv);

  if (name.CompareNoCase("mars_special")) 
  {
    posfunc = pos_func_type(get_mars_helio_coordsv);
    osculating_func = &get_mars_helio_osculating_coords;
  }

  if (name.CompareNoCase("phobos_special"))
    posfunc = pos_func_type(get_phobos_parent_coordsv);

  if (name.CompareNoCase("deimos_special"))
    posfunc = pos_func_type(get_deimos_parent_coordsv);

  if (name.CompareNoCase("jupiter_special")) 
  {
    posfunc = pos_func_type(get_jupiter_helio_coordsv);
    osculating_func = &get_jupiter_helio_osculating_coords;
  }

  if (name.CompareNoCase("europa_special"))
    posfunc = pos_func_type(get_europa_parent_coordsv);

  if (name.CompareNoCase("calisto_special"))
    posfunc = pos_func_type(get_callisto_parent_coordsv);

  if (name.CompareNoCase("io_special"))
    posfunc = pos_func_type(get_io_parent_coordsv);

  if (name.CompareNoCase("ganymede_special"))
    posfunc = pos_func_type(get_ganymede_parent_coordsv);

  if (name.CompareNoCase("saturn_special")) 
  {
    posfunc = pos_func_type(get_saturn_helio_coordsv);
    osculating_func = &get_saturn_helio_osculating_coords;
  }

  if (name.CompareNoCase("mimas_special"))
    posfunc = pos_func_type(get_mimas_parent_coordsv);

  if (name.CompareNoCase("enceladus_special"))
    posfunc = pos_func_type(get_enceladus_parent_coordsv);

  if (name.CompareNoCase("tethys_special"))
    posfunc = pos_func_type(get_tethys_parent_coordsv);

  if (name.CompareNoCase("dione_special"))
    posfunc = pos_func_type(get_dione_parent_coordsv);

  if (name.CompareNoCase("rhea_special"))
    posfunc = pos_func_type(get_rhea_parent_coordsv);

  if (name.CompareNoCase("titan_special"))
    posfunc = pos_func_type(get_titan_parent_coordsv);

  if (name.CompareNoCase("iapetus_special"))
    posfunc = pos_func_type(get_iapetus_parent_coordsv);

  if (name.CompareNoCase("hyperion_special"))
    posfunc = pos_func_type(get_hyperion_parent_coordsv);

  if (name.CompareNoCase("uranus_special")) 
  {
    posfunc = pos_func_type(get_uranus_helio_coordsv);
    osculating_func = &get_uranus_helio_osculating_coords;
  }

  if (name.CompareNoCase("miranda_special"))
    posfunc = pos_func_type(get_miranda_parent_coordsv);

  if (name.CompareNoCase("ariel_special"))
    posfunc = pos_func_type(get_ariel_parent_coordsv);

  if (name.CompareNoCase("umbriel_special"))
    posfunc = pos_func_type(get_umbriel_parent_coordsv);

  if (name.CompareNoCase("titania_special"))
    posfunc = pos_func_type(get_titania_parent_coordsv);

  if (name.CompareNoCase("oberon_special"))
    posfunc = pos_func_type(get_oberon_parent_coordsv);

  if (name.CompareNoCase("neptune_special")) 
  {
    posfunc = pos_func_type(get_neptune_helio_coordsv);
    osculating_func = &get_neptune_helio_osculating_coords;
  }
}

EllipticalOrbit* SolarSystem::GetOrbit(Planet* parent, csRef<iDocumentNode> orbit)
{
  if (!orbit) 
  {
    printf("ERROR: ell_orbit given but no orbit specified");
    return 0;
  }

  csRef<iDocumentNode> Period           = orbit->GetNode ("Period");
  csRef<iDocumentNode> Epoch            = orbit->GetNode ("Epoch");
  csRef<iDocumentNode> SemiMajorAxis    = orbit->GetNode ("SemiMajorAxis");
  csRef<iDocumentNode> Eccentricity     = orbit->GetNode ("Eccentricity");
  csRef<iDocumentNode> Inclination      = orbit->GetNode ("Inclination");
  csRef<iDocumentNode> AscendingNode    = orbit->GetNode ("AscendingNode");
  csRef<iDocumentNode> LongOfPericenter = orbit->GetNode ("LongOfPericenter");
  csRef<iDocumentNode> MeanLongitude    = orbit->GetNode ("MeanLongitude");

  if (!Period || !Epoch || !SemiMajorAxis || !Eccentricity || !Inclination || !AscendingNode || !LongOfPericenter || !MeanLongitude) 
  {
    printf("ERROR: ell_orbit given but missing orbit parameters");
    return 0;
  }

  // Read the orbital elements
  double period             = Period->GetAttributeValueAsFloat("value");
  double epoch              = Epoch->GetAttributeValueAsFloat("value");
  double semi_major_axis    = SemiMajorAxis->GetAttributeValueAsFloat("value")/AU;
  double eccentricity       = Eccentricity->GetAttributeValueAsFloat("value");
  double inclination        = Inclination->GetAttributeValueAsFloat("value")*M_PI/180.;
  double ascending_node     = AscendingNode->GetAttributeValueAsFloat("value")*M_PI/180.;
  double long_of_pericenter = LongOfPericenter->GetAttributeValueAsFloat("value")*M_PI/180.;
  double mean_longitude     = MeanLongitude->GetAttributeValueAsFloat("value")*M_PI/180.;

  double arg_of_pericenter = long_of_pericenter - ascending_node;
  double anomaly_at_epoch = mean_longitude - (arg_of_pericenter + ascending_node);
  double pericenter_distance = semi_major_axis * (1.0 - eccentricity);

  const double parent_rot_obliquity = parent->get_parent() ? parent->getRotObliquity() : 0.0;
  const double parent_rot_asc_node = parent->get_parent()  ? parent->getRotAscendingnode() : 0.0;

  EllipticalOrbit* ellOrbit = new EllipticalOrbit(  pericenter_distance,
			                                        eccentricity,
			                                        inclination,
			                                        ascending_node,
			                                        arg_of_pericenter,
			                                        anomaly_at_epoch,
			                                        period,
			                                        epoch,
			                                        parent_rot_obliquity,
			                                        parent_rot_asc_node);

  return ellOrbit;

}

// Compute the position for every elements of the solar system.
// The order is not important since the position is computed relatively to the mother body
void SolarSystem::computePositions(double date, const Vec3d& observerPos) {
  if (flag_light_travel_time) {
    for (vector<Planet*>::const_iterator iter(system_planets.begin());
         iter!=system_planets.end();iter++) {
      (*iter)->computePositionWithoutOrbits(date);
    }
    for (vector<Planet*>::const_iterator iter(system_planets.begin());
         iter!=system_planets.end();iter++) {
      const double light_speed_correction =
        ((*iter)->get_heliocentric_ecliptic_pos()-observerPos).length()
        * (AU / (SPEED_OF_LIGHT * 86400));
      (*iter)->compute_position(date-light_speed_correction);
    }
  } else {
    for (vector<Planet*>::const_iterator iter(system_planets.begin());
         iter!=system_planets.end();iter++) {
      (*iter)->compute_position(date);
    }
  }
  
  computeTransMatrices(date, observerPos);
}

// Compute the transformation matrix for every elements of the solar system.
// The elements have to be ordered hierarchically, eg. it's important to compute earth before moon.
void SolarSystem::computeTransMatrices(double date, const Vec3d& observerPos) {
  if (flag_light_travel_time) {
    for (vector<Planet*>::const_iterator iter(system_planets.begin());
         iter!=system_planets.end();iter++) {
      const double light_speed_correction =
        ((*iter)->get_heliocentric_ecliptic_pos()-observerPos).length()
        * (AU / (SPEED_OF_LIGHT * 86400));
      (*iter)->compute_trans_matrix(date-light_speed_correction);
    }
  } else {
    for (vector<Planet*>::const_iterator iter(system_planets.begin());
         iter!=system_planets.end();iter++) {
      (*iter)->compute_trans_matrix(date);
    }
  }
}

// Draw all the elements of the solar system
// We are supposed to be in heliocentric coordinate
double SolarSystem::draw(Projector * prj, const Navigator * nav, ToneReproductor* eye)
{
  return 0;
}

Planet* SolarSystem::searchByEnglishName(csString planetEnglishName) const
{
//printf("SolarSystem::searchByEnglishName(\"%s\"): start\n",
//       planetEnglishName.c_str());
	// side effect - bad?
//	transform(planetEnglishName.begin(), planetEnglishName.end(), planetEnglishName.begin(), ::tolower);

	vector<Planet*>::const_iterator iter = system_planets.begin();
	while (iter != system_planets.end())
	{
//printf("SolarSystem::searchByEnglishName(\"%s\"): %s\n",
//       planetEnglishName.c_str(),
//       (*iter)->getEnglishName().c_str());
		if( (*iter)->getEnglishName().GetData() == planetEnglishName.GetData() ) return (*iter);  // also check standard ini file names
		++iter;
	}
//printf("SolarSystem::searchByEnglishName(\"%s\"): not found\n",
//       planetEnglishName.c_str());
	return NULL;
}

// Update i18 names from english names according to passed translator
void SolarSystem::updateI18n()
{
  /*
	Translator& trans = StelApp::getInstance().getLocaleMgr().getSkyTranslator();
	vector<Planet*>::iterator iter;
	for( iter = system_planets.begin(); iter < system_planets.end(); iter++ )
	{
		(*iter)->translateName(trans);
	}
	planet_name_font = StelApp::getInstance().getFontManager().getStandardFont(trans.getTrueLocaleName(), fontSize);
    */
}

vector<wstring> SolarSystem::getNamesI18(void)
{
  vector<wstring> names;
/*
	
	vector < Planet * >::iterator iter;

	for (iter = system_planets.begin(); iter != system_planets.end(); ++iter)
		names.push_back((*iter)->getNameI18n());
*/
  return names;
}


void SolarSystem::updateTrails(const Navigator* nav)
{
	vector<Planet*>::iterator iter;
	for( iter = system_planets.begin(); iter < system_planets.end(); iter++ )
	{
		(*iter)->update_trail(nav);
	}
}

void SolarSystem::startTrails(bool b)
{
	vector<Planet*>::iterator iter;
	for( iter = system_planets.begin(); iter < system_planets.end(); iter++ )
	{
		(*iter)->startTrail(b);
	}
}

// draws earth shadow overlapping the moon using stencil buffer
// umbra and penumbra are sized separately for accuracy
void SolarSystem::draw_earth_shadow(const Navigator * nav, Projector * prj)
{
/*
	csVector3 e = getEarth()->get_ecliptic_pos();
	csVector3 m = getMoon()->get_ecliptic_pos();  // relative to earth
	csVector3 mh = getMoon()->get_heliocentric_ecliptic_pos();  // relative to sun
	float mscale = getMoon()->get_sphere_scale();

	// shadow location at earth + moon distance along earth vector from sun
	csVector3 en = e;
	en.Normalize();
	csVector3 shadow = en * (e.Norm() + m.Norm());

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor3f(1,1,1);

	// find shadow radii in AU
	double r_penumbra = shadow.length()*702378.1/AU/e.length() - 696000/AU;
	double r_umbra = 6378.1/AU - m.length()*(689621.9/AU/e.length());

	// find vector orthogonal to sun-earth vector using cross product with
	// a non-parallel vector
	csVector3 rpt = shadow^csVector3(0,0,1);
	rpt.normalize();
	csVector3 upt = rpt*r_umbra*mscale*1.02;  // point on umbra edge
	rpt *= r_penumbra*mscale;  // point on penumbra edge

	// modify shadow location for scaled moon
	csVector3 mdist = shadow - mh;
	if(mdist.length() > r_penumbra + 2000/AU) return;   // not visible so don't bother drawing

	shadow = mh + mdist*mscale;
	r_penumbra *= mscale;

	nav->switchToHeliocentric();
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_EQUAL, 0x1, 0x1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	Mat4d mat = nav->get_helio_to_eye_mat();

	// shadow radial texture
	tex_earth_shadow->bind();

	csVector3 r, s;

	// umbra first
	glBegin(GL_TRIANGLE_FAN);
	glTexCoord2f(0,0);
	prj->sVertex3( shadow[0],shadow[1], shadow[2], mat);

	for (int i=0; i<=100; i++)
	{
		r = Mat4d::rotation(shadow, 2*M_PI*i/100.) * upt;
		s = shadow + r;

		glTexCoord2f(0.6,0);  // position in texture of umbra edge
		prj->sVertex3( s[0],s[1], s[2], mat);
	}
	glEnd();


	// now penumbra
	csVector3 u, sp;
	glBegin(GL_TRIANGLE_STRIP);
	for (int i=0; i<=100; i++)
	{
		r = Mat4d::rotation(shadow, 2*M_PI*i/100.) * rpt;
		u = Mat4d::rotation(shadow, 2*M_PI*i/100.) * upt;
		s = shadow + r;
		sp = shadow + u;

		glTexCoord2f(0.6,0);
		prj->sVertex3( sp[0],sp[1], sp[2], mat);

		glTexCoord2f(1.,0);  // position in texture of umbra edge
		prj->sVertex3( s[0],s[1], s[2], mat);
	}
	glEnd();

	glDisable(GL_STENCIL_TEST);
*/
}


void SolarSystem::update(Navigator* nav, double delta_time)
{
  vector<Planet*>::iterator iter = system_planets.begin();
  while (iter != system_planets.end())
  {
    (*iter)->update_trail(nav);
    (*iter)->update((int)(delta_time*1000));
    iter++;
  }
}


// is a lunar eclipse close at hand?
bool SolarSystem::near_lunar_eclipse(const Navigator * nav, Projector *prj)
{
	// TODO: could replace with simpler test

	Vec3d e = getEarth()->get_ecliptic_pos();
	Vec3d m = getMoon()->get_ecliptic_pos();  // relative to earth
	Vec3d mh = getMoon()->get_heliocentric_ecliptic_pos();  // relative to sun

	// shadow location at earth + moon distance along earth vector from sun
	Vec3d en = e;
	en.Normalize();
	Vec3d shadow = en * (e.length() + m.length());

	// find shadow radii in AU
	double r_penumbra = shadow.length()*702378.1/AU/e.length() - 696000/AU;

	// modify shadow location for scaled moon
	Vec3d mdist = shadow - mh;
	if(mdist.length() > r_penumbra + 2000/AU) return 0;   // not visible so don't bother drawing

	return 1;
}
