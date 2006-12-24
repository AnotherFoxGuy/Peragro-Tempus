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

#include "atmosphere.h"
#include "client/environment/sky/utils/time.h"
#include <iutil/objreg.h>

Atmosphere::Atmosphere() : sky_resolution(48), tab_sky(NULL), world_adaptation_luminance(0.f), atm_intensity(0)
{
  // Create the vector array used to store the sky color on the full field of view
  tab_sky = new csVector3*[sky_resolution+1];
  for (int k=0; k<sky_resolution+1 ;k++)
  {
    tab_sky[k] = new csVector3[sky_resolution+1];
  }

  iObjectRegistry* obj_reg = PointerLibrary::getInstance()->getObjectRegistry();
  engine = csQueryRegistry<iEngine> (obj_reg);
  if(!engine) return;
  g3d = csQueryRegistry<iGraphics3D> (obj_reg);
  if(!g3d) return;

  poly = 0;
  colors = 0;
}

Atmosphere::~Atmosphere()
{
  for (int k=0; k<sky_resolution+1 ;k++)
  {
    if (tab_sky[k]) delete [] tab_sky[k];
  }
  if (tab_sky) delete [] tab_sky;

  delete poly;
  delete colors;
}

void Atmosphere::compute_color(double JD, csVector3 sunPos, csVector3 moonPos, float moon_phase,
                               ToneReproductor * eye, Projector* prj,
                               float latitude, float altitude, 
                               float temperature, float relative_humidity)
{

	float min_mw_lum = 0.13f;

	atm_intensity = 1.0f;

	//Vec3d obj;
	skylight_struct2 b2;

	// these are for radii
	double sun_angular_size = atan(696000./AU/sqrt(sunPos.SquaredNorm()));
	double moon_angular_size = atan(1738./AU/sqrt(moonPos.SquaredNorm()));

	double touch_angle = sun_angular_size + moon_angular_size;
	double dark_angle = moon_angular_size - sun_angular_size;

	sunPos.Normalize();
	moonPos.Normalize();

	// determine luminance falloff during solar eclipses
	double separation_angle = acos( sunPos * moonPos );  // angle between them

	// bright stars should be visible at total eclipse
	// TODO: correct for atmospheric diffusion
	// TODO: use better coverage function (non-linear)
	// because of above issues, this algorithm darkens more quickly than reality
	if( separation_angle < touch_angle)
	{
		float min;
		if(dark_angle < 0)
		{
			// annular eclipse
			float asun = sun_angular_size*sun_angular_size;
			min = (asun - moon_angular_size*moon_angular_size)/asun;  // minimum proportion of sun uncovered
			dark_angle *= -1;
		}
		else min = 0.004f;  // so bright stars show up at total eclipse

		if(separation_angle < dark_angle) atm_intensity = min;
		else atm_intensity *= min + (1.-min)*(separation_angle-dark_angle)/(touch_angle-dark_angle);

		//		printf("atm int %f (min %f)\n", atm_intensity, min);
	}

	float sun_pos[3];
	sun_pos[0] = sunPos[0];
	sun_pos[1] = sunPos[1];
	sun_pos[2] = sunPos[2];

	float moon_pos[3];
	moon_pos[0] = moonPos[0];
	moon_pos[1] = moonPos[1];
	moon_pos[2] = moonPos[2];

	sky.set_paramsv(sun_pos, 5.f);

	skyb.set_loc(latitude * M_PI/180., altitude, temperature, relative_humidity);
	skyb.set_sun_moon(moon_pos[2], sun_pos[2]);

	// Calculate the date from the julian day.
	ln_date date;
	get_date(JD, &date);

    skyb.set_date(date.years, date.months, moon_phase);

    float stepX = (float)g3d->GetWidth() / sky_resolution;
    float stepY = (float)g3d->GetHeight() / sky_resolution;
    float viewport_left = (float)0; //getViewportPosX()
    float viewport_bottom = (float)0;  //getViewportPosY()

    csVector3 point(1., 0., 0.);

	// Variables used to compute the average sky luminance
	double sum_lum = 0.;
	unsigned int nb_lum = 0;

	// Compute the sky color for every point above the ground
	for (int x=0; x<=sky_resolution; ++x)
	{
		for(int y=0; y<=sky_resolution; ++y)
		{
          //printf("pos before %s\n", point.Description().GetData());
			prj->unproject_local((double)viewport_left+x*stepX, (double)viewport_bottom+y*stepY,point);
			point.Normalize();
          //printf("pos after %s\n", point.Description().GetData());

			if (point[2]<=0)
			{
				point[2] = -point[2];
				// The sky below the ground is the symetric of the one above :
				// it looks nice and gives proper values for brightness estimation
			}

			b2.pos[0] = point[0];
			b2.pos[1] = point[1];
			b2.pos[2] = point[2];

			// Use the Skylight model for the color
			sky.get_xyY_valuev(b2);

			// Use the Skybright.cpp 's models for brightness which gives better results.
			b2.color[2] = skyb.get_luminance(moon_pos[0]*b2.pos[0]+moon_pos[1]*b2.pos[1]+
			                                 moon_pos[2]*b2.pos[2], sun_pos[0]*b2.pos[0]+sun_pos[1]*b2.pos[1]+
			                                 sun_pos[2]*b2.pos[2], b2.pos[2]);


			sum_lum+=b2.color[2];
			++nb_lum;
			eye->xyY_to_RGB(b2.color);
			tab_sky[x][y].Set(atm_intensity*b2.color[0],atm_intensity*b2.color[1],atm_intensity*b2.color[2]);
		}
	}

	world_adaptation_luminance = 3.75f + 3.5*sum_lum/nb_lum*atm_intensity;
	milkyway_adaptation_luminance = min_mw_lum*(1-atm_intensity) + 30*sum_lum/nb_lum*atm_intensity;

	sum_lum = 0.f;
	nb_lum = 0;
}



// Draw the atmosphere using the precalc values stored in tab_sky
void Atmosphere::draw(int delta_time)
{
  float stepX = (float)g3d->GetWidth() / sky_resolution;
  float stepY = (float)g3d->GetHeight() / sky_resolution;
  float viewport_left = (float)0; //getViewportPosX()
  float viewport_bottom = (float)0;  //getViewportPosY()

  int number = sky_resolution * (sky_resolution);
  number *= 4;

  csSimpleRenderMesh mesh;
  if (poly) delete poly;
  poly = new csVector3[number];
  if (colors) delete colors;
  colors = new csVector4[number];

  for (int y2=0; y2<sky_resolution; ++y2)
  {
    for(int x2=0; x2<sky_resolution; ++x2)
    {
      int id = ((y2*sky_resolution)+x2)*4;

      poly[id+0].Set((int)(viewport_left+x2*stepX),(int)(viewport_bottom+y2*stepY),0);
      poly[id+1].Set((int)(viewport_left+(x2+1)*stepX),(int)(viewport_bottom+y2*stepY),0);
      poly[id+2].Set((int)(viewport_left+(x2+1)*stepX),(int)(viewport_bottom+(y2+1)*stepY),0);
      poly[id+3].Set((int)(viewport_left+x2*stepX),(int)(viewport_bottom+(y2+1)*stepY),0);

      colors[id+0].Set(tab_sky[x2][y2]);
      colors[id+1].Set(tab_sky[x2+1][y2]);
      colors[id+2].Set(tab_sky[x2+1][y2+1]);
      colors[id+3].Set(tab_sky[x2][y2+1]);
      /*
      colors[id+0].Set(tab_sky[x2][y2]);
      colors[id+1].Set(tab_sky[x2][y2]);
      colors[id+2].Set(tab_sky[x2][y2+1]);
      colors[id+3].Set(tab_sky[x2][y2]);
      */
    }
  }

  mesh.meshtype = CS_MESHTYPE_QUADS;
  mesh.indexCount = number;
  mesh.vertexCount = number;
  mesh.vertices = poly;
  mesh.colors = colors;
  mesh.mixmode = CS_FX_ALPHA;
  mesh.colors = colors;


  g3d->DrawSimpleMesh (mesh, csSimpleMeshScreenspace);

}
