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

#include "environmentmanager.h"
#include "client/pointer/pointer.h"
#include "common/event/environmentevent.h"
#include "common/reporter/reporter.h"
#include "common/event/eventmanager.h"

#include <iengine/engine.h>
#include <iengine/mesh.h>
#include <iengine/scenenode.h>
#include <iengine/movable.h>
#include <iengine/sector.h>
#include <iengine/rview.h>
#include <iengine/camera.h>
#include <ivideo/shader/shader.h>
#include <iutil/virtclk.h>

namespace PT
{
  namespace Environment
  {
    EnvironmentManager::EnvironmentManager()
    {
      sun_alpha = 3.21f;//2.91f; //horizontal
      sun_theta = 0.206f;//0.256f; //vertical

      clock = 0;
    }

    EnvironmentManager::~EnvironmentManager()
    {
      // Remove callback.
      if (cb.IsValid())
      {
        engine->RemoveEngineFrameCallback(cb);
      }

      if (clock) delete clock;
    }

    bool EnvironmentManager::Initialize()
    {
      clock = new Clock();
      clock->Initialize();

      iObjectRegistry* object_reg = PointerLibrary::getInstance()->getObjectRegistry();
      engine = csQueryRegistry<iEngine> (object_reg);
      if (!engine) return Report(PT::Error, "Failed to locate 3D engine!");

      // Create our sun.
      sun = engine->CreateLight("Sun", csVector3(0,40,0),9999999999.0f, csColor(1.0f), CS_LIGHT_DYNAMICTYPE_DYNAMIC);

      iSector* world = engine->FindSector("World");
      if (!world)
        world = engine->CreateSector("World");

      world->GetLights()->Add(sun);

      shaderMgr = csQueryRegistry<iShaderManager> (object_reg);
      strings = csQueryRegistryTagInterface<iShaderVarStringSet> (object_reg, "crystalspace.shader.variablenameset");
      string_sunDirection = strings->Request ("sun direction");

      // Update our manager each frame.
      cb.AttachNew(new FrameCallBack(this));
      engine->AddEngineFrameCallback(cb);


      return true;
    } // end Initialize()

    void EnvironmentManager::Update(iCamera* cam)
    {
      if (!clock) return;

      // Move the sun relative to the player.
      if (cam && sun)
        sun->SetCenter(cam->GetTransform().GetOrigin()+csVector3(500,2000,0));

      static float lastStep = clock->GetTimeDecimal();
      float step = clock->GetTimeDecimal();
      // Don't update if the time has not changed much.
      if ((step - lastStep) < 0.0001f && (step - lastStep) > -0.0001f) return;
      lastStep = step;

      //=[ Sun position ]===================================
      float temp = step * 2.0f;
      if (temp > 1.0f) temp = 2.0f - temp;

      sun_theta = ((2.0f * temp) - 1.0f) * 0.85;

      sun_alpha = 1.605f * sin(-step * 2.0f * PI) - 3.21f;

      // Update the values.
      csVector3 sun_vec;
      sun_vec.x = cos(sun_theta) * sin(sun_alpha);
      sun_vec.y = sin(sun_theta);
      sun_vec.z = cos(sun_theta) * cos(sun_alpha);
      csShaderVariable* var = shaderMgr->GetVariableAdd(string_sunDirection);
      var->SetValue(sun_vec);

      // Set the sun position.
      csVector3 sun_pos = sun_vec * 1500.0f;
      if (sun)
      {
        //sun->GetMovable()->SetPosition(sun_pos);
        //sun->GetMovable()->UpdateMove();
      }

      //=[ Sun brightness ]===================================
      // This is just "Lambert's cosine law" shifted so midday is 0, and
      // multiplied by 1.9 instead of 2 to extend the daylight after sunset to
      // approximate twilight.
      float brightness = cos((step - 0.5f) * PI * 1.9f);
      csColor sunlight(brightness * 1.5f);
      sunlight.ClampDown();
      // The ambient color is adjusted to give a slightly more yellow colour at
      // midday, graduating to a purplish blue at midnight. Adjust "min_light"
      // to make it playable at night.
      const float min_light = 0.3f;
      float amb = cos((step - 0.5f) * PI * 2.2f);
      csColor ambient((amb*0.125f)+0.075f+min_light, (amb*0.15f)+0.05f+min_light,
        (amb*0.1f)+0.08f+min_light);
      ambient.ClampDown();

      // Update the values.
      iSector* world = engine->FindSector("World");
      if (world) world->SetDynamicAmbientLight(ambient);
      if (sun) sun->SetColor(sunlight);

      //=[ Clouds ]========================================
      //<shadervar type="vector3" name="cloudcol">0.98,0.59,0.46</shadervar>
      //float brightnessc = pow(sin(step * PI), 3);
      float brightnessc = (amb * 0.6f) + 0.4f;
      CS::ShaderVarStringID time = strings->Request("timeOfDay");
      csRef<csShaderVariable> sv = shaderMgr->GetVariableAdd(time);
      sv->SetValue(brightnessc);

    } // end Update()

    void EnvironmentManager::FrameCallBack::StartFrame(iEngine* engine, iRenderView* rview)
    {
      if (!envmgr)
      {
        engine->RemoveEngineFrameCallback(this);
      }
      else
      {
        ///TODO: <camera /> is broken in CS atm, work around.
        if (!envmgr->sky.IsValid())
          envmgr->sky = engine->FindMeshObject("sky");
        else
          envmgr->UpdateCameraMesh(envmgr->sky, rview);

        if (!envmgr->clouds.IsValid())
          envmgr->clouds = engine->FindMeshObject("clouds");
        else
          envmgr->UpdateCameraMesh(envmgr->clouds, rview);
        //end TODO

        envmgr->Update(rview->GetCamera());
      }

    } // end StartFrame()

    void EnvironmentManager::UpdateCameraMesh(iMeshWrapper* m, iRenderView* rview)
    {
      iMovable* mov = m->GetMovable ();
      // Temporarily move the object to the current camera.
      csReversibleTransform &mov_trans = mov->GetTransform ();
      // @@@ TEMPORARY: now CS_ENTITY_CAMERA only works at 0,0,0 position.
      csVector3 old_pos = mov_trans.GetOrigin ();
      mov_trans.SetOrigin (csVector3 (0));
      iCamera *orig_cam = rview->GetOriginalCamera ();
      csOrthoTransform &orig_trans = orig_cam->GetTransform ();
      csVector3 v = orig_trans.GetO2TTranslation ();
      mov_trans.SetOrigin (mov_trans.GetOrigin () + v);
      csVector3 diff = old_pos - mov_trans.GetOrigin ();
      if (!(diff < .00001f))
        mov->UpdateMove ();
    } // end UpdateCameraMesh()

  } // Environment namespace
} // PT namespace
