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

#include "effecttemplate.h"

void Effect::Report(int severity, const char* msg, ...)
{
  va_list arg;
  va_start (arg, msg);
  csReportV(object_reg, severity, "peragro.effect", msg, arg);
  va_end (arg);
}

Effect::Effect(iObjectRegistry* obj_reg, EffectTemplate* t, csVector3 position, iSector* sector)
  : iCacheUser(obj_reg),
  effectType(PositionEffectType),
  pos(position), sec(sector),
  object_reg(obj_reg),
  timeleft(t->GetDuration()), fileName(t->GetMeshFile()),
  meshName(t->GetMeshName()), offset(t->GetOffset())
{
  Load(fileName);
} // end Effect()

Effect::Effect(iObjectRegistry* obj_reg, EffectTemplate* t, iMeshWrapper* parent)
  : iCacheUser(obj_reg),
  effectType(ParentEffectType),
  parentMesh(parent),
  object_reg(obj_reg),
  timeleft(t->GetDuration()), fileName(t->GetMeshFile()),
  meshName(t->GetMeshName()), offset(t->GetOffset())
{
  Load(fileName);
} // end Effect()

Effect::~Effect()
{
  if (mesh)
  {
    // Effect is only ref holder, don't have to do anything.
    if (mesh->GetRefCount() == 1) return;

    // Remove from any collections.
    if (mesh->QueryObject())
      if (mesh->QueryObject()->GetObjectParent())
        mesh->QueryObject()->GetObjectParent()->ObjRemove(mesh->QueryObject());

    // Remove from sectors and any parent meshes.
    if (mesh->GetMovable())
    {
      mesh->GetMovable()->ClearSectors();
      mesh->GetMovable()->GetSceneNode()->SetParent(0);
      mesh->GetMovable()->UpdateMove();
    }
  }
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
