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

#include "client/effects/effectsmanager.h"
#include "client/effects/effect.h"


#include "cstool/initapp.h"
#include "csutil/cmdline.h"
#include "csutil/csstring.h"
#include "csutil/csshlib.h"
#include "csutil/event.h"
#include "csutil/sysfunc.h"
#include "csutil/syspath.h"
#include "iengine/camera.h"
#include "iengine/campos.h"
#include "iengine/mesh.h"
#include "iengine/sector.h"
#include "iengine/texture.h"
#include "iengine/material.h"
#include "iengine/scenenode.h"
#include "iengine/movable.h"
#include "imesh/object.h"
#include "imesh/spritecal3d.h"
#include "imesh/sprite2d.h"
#include "imesh/genmesh.h"
#include "imesh/objmodel.h"
#include "iutil/databuff.h"
#include "iutil/event.h"
#include "iutil/eventq.h"
#include "iutil/object.h"
#include "iutil/vfs.h"
#include "ivaria/collider.h"
#include "ivideo/graph2d.h"
#include "ivideo/natwin.h"
#include "ivideo/txtmgr.h"
#include "ivideo/material.h"

#include "iutil/objreg.h"

#include "imap/loader.h"


EffectsManager::EffectsManager (iObjectRegistry* obj_reg)
{
  engine = CS_QUERY_REGISTRY(obj_reg, iEngine);
  //if (!engine) return ReportError("Failed to locate 3D engine!");

  vfs = CS_QUERY_REGISTRY(obj_reg, iVFS);
  //if (!vfs) return ReportError("Failed to locate VFS!");
}

EffectsManager::~EffectsManager ()
{
}

bool EffectsManager::Initialize (iObjectRegistry* obj_reg)
{
  loader = CS_QUERY_REGISTRY (obj_reg, iLoader);
  if (!loader)
  {
    //Report (CS_REPORTER_SEVERITY_ERROR, "No loader!");
    return false;
  }

  // load the factory for particles from file
  if ( !loader->LoadLibraryFile ("/peragro/meshes/effects/alleffects.xml") ) return false;

  return true;
}

void EffectsManager::HandleEffects (csTicks elapsed_ticks)
{
  size_t i = 0;
  while (i < effects.Length ())
  {
    if (effects[i].Handle (elapsed_ticks)) i++;
    else
    {
      engine->RemoveObject (effects[i].GetMesh ());
      effects.DeleteIndex (i);
    }
  }
}

iMeshWrapper* EffectsManager::CreateEffectMesh (int effect)
{
  csString factory;
  int duration;
  float height;

 switch (effect)
  {
    case EffectsManager::Blood: 
      {
      factory = "bloodsplat"; 
      duration = 2000;
      height = 1.3f;
      }
      break;

    case EffectsManager::Levelup:
      {
      factory = "levelup"; 
      duration = 6000;
      height = 1.1f;
      }
      break;

    default : {printf("Unknown effect type!"); return 0;}
  };

 // Locate the mesh in the engine
 csRef<iMeshWrapper> effectmw = engine->FindMeshObject(factory);
 csRef<iMeshObject> effectob = effectmw ->GetMeshObject();
 if (!effectob)
 {
   printf ("Unable to load particles!");
   return false;
 }
 csVector3 position = (effectmw->GetMovable()->GetPosition()) + csVector3(0,height,0);

 // Clone the object and create a new mesh from it
 csRef<iMeshObject> effectobclone = effectob ->Clone();
 csRef<iMeshWrapper> effectmwclone = engine->CreateMeshWrapper(effectobclone, "effect", engine->FindSector("room"), position);

 // Add it to the effect array for later processing
 Effect eff (effectmwclone, duration);
 effects.Push (eff);

 return effectmwclone;
}

bool EffectsManager::CreateEffect (iMeshWrapper* parent, int effect)
{
  // Parent the particle mesh to the parent entity
  CreateEffectMesh (effect)->QuerySceneNode()->SetParent(parent->QuerySceneNode());

  return true;
}

