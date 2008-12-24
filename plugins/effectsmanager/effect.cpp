/*
    Copyright (C) 2008 Development Team of Peragro Tempus

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

#include "effect.h"

#include <iengine/engine.h>
#include <iengine/texture.h>
#include <iengine/movable.h>
#include <iengine/scenenode.h>
#include <iutil/object.h>

void Effect::Report(int severity, const char* msg, ...)
{
  va_list arg;
  va_start (arg, msg);
  csReportV(object_reg, severity, "peragro.effect", msg, arg);
  va_end (arg);
}

Effect::Effect(iObjectRegistry* obj_reg, PT::Data::Effect* eff, csVector3 position, iSector* sector)
  : iCacheUser(obj_reg, "peragro.effectscache"), 
  effectType(PositionEffectType),
  pos(position), sec(sector),
  object_reg(obj_reg),
  timeleft(eff->GetDuration()), meshName(eff->GetMeshName()), offset(eff->GetOffset())
{
  Load(meshName);
} // end Effect()

Effect::Effect(iObjectRegistry* obj_reg, PT::Data::Effect* eff, iMeshWrapper* parent)
  : iCacheUser(obj_reg, "peragro.effectscache"), 
  effectType(ParentEffectType),
  parentMesh(parent),
  object_reg(obj_reg),
  timeleft(eff->GetDuration()), meshName(eff->GetMeshName()), offset(eff->GetOffset())
{
  Load(meshName);
} // end Effect()

Effect::~Effect()
{
} // end ~Effect()

bool Effect::Handle (csTicks elapsed_ticks)
{
  timeleft -= elapsed_ticks;
  if (timeleft <= 0)
  {
    return false;
  }
  return true;
} // end Handle()

void Effect::Loaded(iCacheEntry* cacheEntry)
{
  if (cacheEntry->WasSuccessful())
  {
    mesh = cacheEntry->Create("effect", meshName);
    mesh->GetMovable ()->SetPosition (offset);
    switch (effectType)
    {
    case ParentEffectType:
      if (parentMesh)
        mesh->QuerySceneNode()->SetParent(parentMesh->QuerySceneNode());
      else
        Report(CS_REPORTER_SEVERITY_ERROR, "Unable to attach particle mesh %s to parent: no parent found!", effectName.c_str());
      break;
    case PositionEffectType:
      csVector3 curpos = mesh->QuerySceneNode()->GetMovable()->GetFullPosition();
      mesh->QuerySceneNode()->GetMovable()->SetPosition(curpos + pos);
      if (sec) mesh->QuerySceneNode()->GetMovable()->SetSector(sec);
      mesh->PlaceMesh();
      mesh->QuerySceneNode()->GetMovable()->UpdateMove();
      break;
    }
  }
  else
  {
    timeleft = 0.0f; // Shedule for removal.
  }
} // end Loaded()
