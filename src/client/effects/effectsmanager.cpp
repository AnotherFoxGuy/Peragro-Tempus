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

#include "cstool/keyval.h"

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
//#include "imesh/objmodel.h"
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
  this->obj_reg = obj_reg;

  engine = CS_QUERY_REGISTRY(obj_reg, iEngine);
  //if (!engine) return ReportError("Failed to locate 3D engine!");

  vfs = CS_QUERY_REGISTRY(obj_reg, iVFS);
  //if (!vfs) return ReportError("Failed to locate VFS!");
}

EffectsManager::~EffectsManager ()
{
}

bool EffectsManager::Initialize ()
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
  int duration = 0;
  float height = 0;

 switch (effect)
  {
    case EffectsManager::Blood: 
      {
      factory = "bloodsplatfact"; 
      }
      break;

    case EffectsManager::Levelup:
      {
      factory = "levelupfact"; 
      }
      break;

    case EffectsManager::Pentagram:
      {
      factory = "pentagramfact"; 
      }
      break;

    case EffectsManager::Energysphere:
      {
        factory = "energyspherefact"; 
      }
      break;

    case EffectsManager::MoveMarker:
      {
        factory = "movemarkerfact"; 
      }
      break;

    default : {printf("EffectsManager: Unknown effect type %d!\n", effect); return 0;}
  };

 // Find the factory and turn it into a factorywrapper.
 csRef<iMeshFactoryWrapper> effectfmw = engine->FindMeshFactory(factory);
 if (!effectfmw)
 {
   printf ("EffectsManager: Couldn't find particle factory: ' %s ' !\n", factory.GetData());
   return 0;
 }

 // Parse the keyvalue for effect duration and heightoffset.
 csRef<iObjectIterator> it = effectfmw->QueryObject ()->GetIterator ();
 while (it->HasNext ())
 {
   iObject* obj = it->Next ();
   csRef<iKeyValuePair> key = scfQueryInterface<iKeyValuePair> (obj);
   if (key)
   {
     printf ("EffectsManager: We got a key, parsing it!\n");
     if (!strcmp (key->GetKey (), "duration"))
     {
       duration = atoi( key->GetValue () );
       printf ("EffectsManager: Reading key value for duration!\n");
     }
     else if (!strcmp (key->GetKey (), "heightoffset"))
     {
       height = float(atof( key->GetValue () ));
       printf ("EffectsManager: Reading key value for heightoffset!\n");
     }
   }
 }

 // Create the effect object.
 csRef<iMeshWrapper> effectmw = engine->CreateMeshWrapper(effectfmw, "effect", engine->FindSector("room"),csVector3(0,height,0));
 if (!effectmw)
 {
   printf ("EffectsManager: Particle MeshWrapper creation failed!\n");
   return 0;
 }

 // Add it to the effect array for later processing
 Effect eff (effectmw, duration);
 effects.Push (eff);

 return effectmw;
}

bool EffectsManager::CreateEffect (iMeshWrapper* parent, int effect)
{
  // Parent the particle mesh to the parent entity
  csRef<iMeshWrapper> effectmw = CreateEffectMesh (effect);
  if (!effectmw)
  {
    printf ("EffectsManager: Unable to create effect: %d!\n", effect);
    return false;
  }

  if (!parent)
  {
    printf ("EffectsManager: Unable to attach particle mesh %d to parent: no parent found!\n", effect);
    return false;
  }

  effectmw->QuerySceneNode()->SetParent(parent->QuerySceneNode());

  return true;
}

bool EffectsManager::CreateEffect (int effect, csVector3 pos)
{
  // Create the particle mesh.
  csRef<iMeshWrapper> effectmw = CreateEffectMesh (effect);
  if (!effectmw)
  {
    printf ("EffectsManager: Unable to create effect: %d!\n", effect);
    return false;
  }
  // Offset the effect.
  csVector3 curpos = effectmw->QuerySceneNode()->GetMovable()->GetFullPosition();
  effectmw->QuerySceneNode()->GetMovable()->SetPosition(curpos + pos);
  effectmw->QuerySceneNode()->GetMovable()->UpdateMove();

  return true;
}

