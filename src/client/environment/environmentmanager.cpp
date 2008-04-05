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
#include "client/event/environmentevent.h"
#include "client/reporter/reporter.h"
#include "client/event/eventmanager.h"

#include <iengine/engine.h>
#include <iengine/mesh.h>
#include <iengine/scenenode.h>
#include <iengine/movable.h>
#include <iengine/sector.h>
#include <iengine/rview.h>
#include <iengine/camera.h>

namespace PT
{

  EnvironmentManager::EnvironmentManager()
  {
  }

  EnvironmentManager::~EnvironmentManager()
  {
    // Remove callback.
    if (cb.IsValid())
    {
      engine->RemoveEngineFrameCallback(cb);
    }
  }

  bool EnvironmentManager::SetDayTime(iEvent& ev)
  {
    // Calculate the light color.
    unsigned int hour = PT::Events::EnvironmentHelper::GetDayTime(&ev);

    float brightness;
    if (hour < 6 || hour > 20)
    {
      brightness = 0.1;
    }
    else if (hour == 6 || hour == 20)
    {
      brightness = 0.3;
    }
    else if (hour == 7 || hour == 19)
    {
      brightness = 0.6;
    }
    else
    {
      brightness = 1;
    }
    csColor color(brightness);

    csColor ambient(color);
    ambient.Clamp(0.37f, 0.34f, 0.34f);
    if (ambient.blue < 0.2)
      ambient.blue = 0.1f;
    iSector* world = engine->FindSector("World");
    if (world)
      world->SetDynamicAmbientLight(ambient);

    if (color.blue < 0.2)
      color.blue = 0.2f;
    if (sun)
      sun->SetColor(color*1.5);

    if (sky)
    {
      /*
      iObjectRegistry* obj_reg = PointerLibrary::getInstance()->getObjectRegistry();
      csRef<iStringSet> stringSet = csQueryRegistryTagInterface<iStringSet> (obj_reg, "crystalspace.shared.stringset");
      csStringID time = stringSet->Request("timeOfDay");
      csRef<iShaderVariableContext> svc = sky->GetSVContext();
      csRef<csShaderVariable> sv = svc->GetVariableAdd(time);
      sv->SetValue(brightness);*/

      iObjectRegistry* obj_reg = PointerLibrary::getInstance()->getObjectRegistry();
      csRef<iStringSet> stringSet = csQueryRegistryTagInterface<iStringSet> (obj_reg, "crystalspace.shared.stringset");
      csRef<iShaderManager> shaderMgr = csQueryRegistry<iShaderManager> (obj_reg);

      csStringID time = stringSet->Request("timeOfDay");
      csRef<csShaderVariable> sv = shaderMgr->GetVariableAdd(time);
      sv->SetValue(brightness);
    }

    return true;
  } // end SetDayTime()

  bool EnvironmentManager::Initialize()
  {
    engine = csQueryRegistry<iEngine> (PointerLibrary::getInstance()->getObjectRegistry());
    if (!engine) return Report(PT::Error, "Failed to locate 3D engine!");

    Events::EventHandler<EnvironmentManager>* cbDayTime = new Events::EventHandler<EnvironmentManager>(&EnvironmentManager::SetDayTime, this);
    PointerLibrary::getInstance()->getEventManager()->AddListener("environment.daytime", cbDayTime);

    // Create our sun.
    sun = engine->CreateLight("Sun", csVector3(0,40,0),9999999.0f, csColor(1.0f), CS_LIGHT_DYNAMICTYPE_DYNAMIC);

    iSector* world = engine->FindSector("World");
    if (!world)
      world = engine->CreateSector("World");

    world->GetLights()->Add(sun);

    // Update our manager each frame.
    cb.AttachNew(new FrameCallBack(this));
    engine->AddEngineFrameCallback(cb);


    return true;
  } // end Initialize()

  void EnvironmentManager::FrameCallBack::StartFrame(iEngine* engine, iRenderView* rview)
  {
    if (!envmgr)
    {
      engine->RemoveEngineFrameCallback(this);
    }
    else
    {
      if (!envmgr->sky.IsValid())
      {
        envmgr->sky = engine->FindMeshObject("sky");
      }

      iCamera* cam = rview->GetCamera();
      if (cam && envmgr->sun)
        envmgr->sun->SetCenter(cam->GetTransform().GetOrigin()+csVector3(500,2000,0));
    }

  } // end StartFrame()

}
