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

#include "client/environment/sky/skymanager.h"

#include <iutil/objreg.h>

#include <propclass/defcam.h>

#include "client/entity/ptentitymanager.h"


SkyMGR::SkyMGR ()
{
  timer = 1001;
  deltaAz = 0;
  deltaAlt = 0;
  deltapitch = 0;
  deltayaw = 0;
}

SkyMGR::~SkyMGR ()
{
}

bool SkyMGR::Initialize ()
{
  iObjectRegistry* obj_reg = PointerLibrary::getInstance()->getObjectRegistry();

   engine = csQueryRegistry<iEngine> (obj_reg);
  if(!engine) return false;

  vfs = csQueryRegistry<iVFS> (obj_reg);
  if(!vfs) return false;

  g3d = csQueryRegistry<iGraphics3D> (obj_reg);
  if(!g3d) return false;

  vc = csQueryRegistry<iVirtualClock> (obj_reg);
  if(!vc) return false;

  atmosphere = new Atmosphere();
  tone_converter = new ToneReproductor();

  // Init the solar system first
  ssystem = new SolarSystem();
  ssystem->init();

  // Observator
  observatory = new Observator(*ssystem);
  observatory->load();

  navigation = new Navigator(observatory);
  navigation->init();

  projection = new Projector();
  projection->init();

  //TODO remove this
  navigation->setTimeSpeed(JD_HOUR);
  Update(1);

  return true;
}

void SkyMGR::updateMove(int delta_time)
{
    ptEntityManager* entitymgr = PointerLibrary::getInstance()->getEntityManager();
    iPcDefaultCamera* cam = entitymgr->getOwnCamera();

    if(!cam) return;

    deltaAlt = cam->GetPitch() - deltapitch;
    deltaAz = cam->GetYaw() - deltayaw;

    deltapitch = cam->GetPitch();
    deltayaw = cam->GetYaw();

    deltaAlt *= 1.333;
    deltaAz *= 1.333;

    iCelEntity* celentity = entitymgr->getOwnCelEntity();
    csRef<iPcLinearMovement> pclinmove = CEL_QUERY_PROPCLASS_ENT(celentity, iPcLinearMovement);
    if (pclinmove.IsValid())
    {
      float rot = 0;
      csVector3 campos;
      iSector* camsector = 0;

      // TODO
      pclinmove->GetLastFullPosition(campos, rot, camsector);
      //navigation->setLocalVision(campos);
      //printf("pos before %s\n", campos.Description().GetData());
      //navigation->setLocalVision(navigation->earth_equ_to_local(campos));
      //printf("pos after %s\n", navigation->earth_equ_to_local(campos).Description().GetData());
    }

    double move_speed = 0.00025;
	double depl=move_speed*delta_time*cam->GetCamera()->GetFOVAngle();
	if (deltaAz<0)
	{
		deltaAz = -depl/30;
		if (deltaAz<-0.2)
			deltaAz = -0.2;
	}
	else
	{
		if (deltaAz>0)
		{
			deltaAz = (depl/30);
			if (deltaAz>0.2)
				deltaAz = 0.2;
		}
	}
	if (deltaAlt<0)
	{
		deltaAlt = -depl/30;
		if (deltaAlt<-0.2)
			deltaAlt = -0.2;
	}
	else
	{
		if (deltaAlt>0)
		{
			deltaAlt = depl/30;
			if (deltaAlt>0.2)
				deltaAlt = 0.2;
		}
	}
	if(deltaAz != 0 || deltaAlt != 0)
	{
		navigation->updateMove(deltaAz, deltaAlt);
	}
	else
	{
		// must perform call anyway, but don't record!
		navigation->updateMove(deltaAz, deltaAlt);
	}
}

void SkyMGR::Update (int delta_time)
{
  delta_time *= 4;  // make time running faster

  observatory->update(delta_time);
  navigation->updateTime(delta_time);

  // Position of sun and all the satellites (ie planets)
  ssystem->computePositions(navigation->getJDay(),
                            navigation->getHomePlanet()->get_heliocentric_ecliptic_pos());

  // Transform matrices between coordinates systems
  navigation->updateTransformMatrices();
  // Direction of vision
  navigation->updateVisionVector(delta_time);

  // update faders and Planet trails (call after nav is updated)
  ssystem->update(navigation, (double)delta_time/1000);

  // Move the view direction and/or fov
  updateMove(delta_time);

  atmosphere->update(delta_time);

  // Compute the sun position in local coordinate
  Vec3d temp(0.,0.,0.);
  Vec3d sunPos = navigation->helio_to_local(temp);
  printf("Sun %7.4f %7.4f %7.4f    ", sunPos[0], sunPos[1], sunPos[2]);


  // Compute the moon position in local coordinate
  //csVector3 moon = ssystem->getMoon()->get_heliocentric_ecliptic_pos();
  Vec3d moon = ssystem->getMoon()->get_heliocentric_ecliptic_pos();
  Vec3d moonPos = navigation->helio_to_local(moon);
  printf("moon %7.4f %7.4f %7.4f    ", moonPos[0], moonPos[1], moonPos[2]);

  // Give the updated standard projection matrices to the projector.
  projection->set_modelview_matrices(navigation->get_earth_equ_to_eye_mat(),
	                                 navigation->get_helio_to_eye_mat(),
	                                 navigation->get_local_to_eye_mat(),
	                                 navigation->get_j2000_to_eye_mat());


  /*
   // Compute the atmosphere color and intensity
  atmosphere->compute_color(navigation->getJDay(), sunPos, moonPos,
    ssystem->getMoon()->get_phase(ssystem->getEarth()->get_heliocentric_ecliptic_pos()),
    tone_converter, projection, observatory->get_latitude(), observatory->get_altitude(),
    15.f, 40.f);	// Temperature = 15c, relative humidity = 40%
  */

 // Compute the atmosphere color and intensity
  atmosphere->compute_color(navigation->getJDay(), sunPos, moonPos, 1,
                            tone_converter, projection, 
                            observatory->get_latitude(), observatory->get_altitude(), 
                            15.f, 40.f);
 
  tone_converter->set_world_adaptation_luminance(atmosphere->get_world_adaptation_luminance());


  sunPos.Normalize();
  //moonPos.Normalize();

  // compute global sky brightness 
  if(sunPos[2] < -0.1/1.5 )
    sky_brightness = 0.01f;
  else
    sky_brightness = (0.01 + 1.5*(sunPos[2]+0.1/1.5));

  sky_brightness *= (atmosphere->get_intensity()+0.1);

  // Set the ambient light
  float value = sky_brightness+0.05;
  engine->SetAmbientLight(csColor(value, value, value));

  printf("ambient %7.4f\n", value);
	
}

void SkyMGR::Handle ()
{
  timer += 1;
  if (timer > 70)
  {
    timer = 0;

    double JDay = navigation->getJDay();
    ln_date date;
    get_date(JDay,  &date);
    printf("Time is: %d:%d:%f  %d/%d/%d\n", 
      date.hours, date.minutes, date.seconds, date.days, date.months, date.years);
  }

  csTicks ticks = vc->GetElapsedTicks();

   Update(ticks);

   atmosphere->draw(1);

}
