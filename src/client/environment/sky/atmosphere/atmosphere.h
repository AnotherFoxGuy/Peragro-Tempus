/*
    Copyright (C) 2005 Development Team of Peragro Tempus

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


#ifndef ATMOSTPHERE_H_
#define ATMOSTPHERE_H_

#include <cmath>


#include <cssysdef.h>
#include <csutil/ref.h>
#include <iutil/virtclk.h>
#include <iengine/engine.h>
#include <csgeom/path.h>
#include <csgeom/math3d.h>
#include <iutil/object.h>
#include <csutil/cscolor.h>
#include <ivideo/graph3d.h>
#include <csgeom/vector4.h>

#include "client/pointer/pointer.h"

#include "client/environment/sky/atmosphere/sun/skylight.h"
#include "client/environment/sky/utils/tone_reproductor.h"
#include "client/environment/sky/atmosphere/sun/skybright.h"
#include "client/environment/sky/projector/projector.h"

using namespace std;

class Projector;
struct iObjectRegistry;

class Atmosphere
{
public:
    Atmosphere();
    virtual ~Atmosphere();
	void compute_color(double JD, csVector3 sunPos, csVector3 moonPos, float moon_phase, 
                       ToneReproductor * eye, Projector* prj,
		               float latitude = 48.f, float altitude = 83.f,
		               float temperature = 15.f, float relative_humidity = 40.f);
	void draw(int delta_time);
    void update(int delta_time){}

	
	float get_intensity(void) {return atm_intensity; }  // tells you actual atm intensity due to eclipses + fader
	float get_world_adaptation_luminance(void) const {return world_adaptation_luminance;}
	float get_milkyway_adaptation_luminance(void) const {return milkyway_adaptation_luminance;}
private:
	Skylight sky;
	Skybright skyb;
	int sky_resolution;
	csVector3 ** tab_sky;	// For Atmosphere calculation
	int startY;			// intern variable used to store the Horizon Y screen value
	float world_adaptation_luminance;
	float milkyway_adaptation_luminance;
	float atm_intensity;

private:
  csRef<iEngine> engine;
  csRef<iGraphics3D> g3d;

  csVector3* poly;
  csVector4* colors;

};

#endif // ATMOSTPHERE_H_
