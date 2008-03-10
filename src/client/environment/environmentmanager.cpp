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

#include <cstool/csapplicationframework.h>
#include <iengine/mesh.h>
#include <iengine/scenenode.h>
#include <iengine/movable.h>

namespace PT
{

EnvironmentManager::EnvironmentManager()
{
}

EnvironmentManager::~EnvironmentManager()
{
}

bool EnvironmentManager::SetDayTime(PT::Events::Eventp ev)
{
  //TODO: have a nicer way to get to the sun.
  iMeshWrapper* clouds = engine->FindMeshObject("clouds");
  if (!clouds) return true;
  iMovable* move = clouds->GetMovable();
  if (!move->GetSceneNode()->GetChildren().GetSize() == 0) return true;
  iSceneNode* obj = move->GetSceneNode()->GetChildren().Get(0);
  iLight* light = obj->QueryLight();
  if (light)
  {
    using namespace PT::Events;
    EnvironmentDayTimeEvent* envEv = GetEnvironmentEvent<EnvironmentDayTimeEvent*>(ev);

    float brightness;
    if (envEv->hour < 7 || envEv->hour > 20)
    {
      brightness = 0.1;
    }
    else if (envEv->hour == 8 || envEv->hour == 19)
    {
      brightness = 0.6;
    }
    else
    {
      brightness = 1;
    }
    csColor color(brightness);
    light->SetColor(color);
  }
  return true;
}

bool EnvironmentManager::Initialize()
{
  engine = csQueryRegistry<iEngine> (csApplicationFramework::GetObjectRegistry());
  if (!engine) return Report(PT::Error, "Failed to locate 3D engine!");

  Events::EventHandler<EnvironmentManager>* cbDayTime = new Events::EventHandler<EnvironmentManager>(&EnvironmentManager::SetDayTime, this);
  PointerLibrary::getInstance()->getEventManager()->AddListener("environment.daytime", cbDayTime);
  return true;
}

}
