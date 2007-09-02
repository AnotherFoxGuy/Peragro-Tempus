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

#include "effectsmanager.h"

#include "client/reporter/reporter.h"

#include <iutil/objreg.h>
#include <iutil/plugin.h>
#include <imap/loader.h>
#include <cstool/keyval.h>

#include <iengine/scenenode.h>
#include <iengine/movable.h>
#include <imesh/object.h>
#include <iengine/material.h>
#include <iengine/camera.h>

#include <ivaria/decal.h>

#include <iutil/vfs.h>
#include <iengine/engine.h>

namespace PT
{
  namespace Data
  {
    EffectsManager::EffectsManager (iObjectRegistry* obj_reg)
    {
      this->obj_reg = obj_reg;

      engine =  csQueryRegistry<iEngine> (obj_reg);
      if (!engine) Report(PT::Bug, "EffectsManager: Failed to locate 3D engine!");

      vfs =  csQueryRegistry<iVFS> (obj_reg);
      if (!vfs) Report(PT::Bug, "EffectsManager: Failed to locate VFS!");
    }

    EffectsManager::~EffectsManager ()
    {
    }

    bool EffectsManager::Initialize ()
    {
      loader =  csQueryRegistry<iLoader> (obj_reg);
      if (!loader)
        return Report(PT::Bug, "EffectsManager: Failed to locate loader!");

      // load the factory for particles from file
      if ( !loader->LoadLibraryFile ("/peragro/meshes/effects/alleffects.xml") ) return false;

      return true;
    }

    void EffectsManager::HandleEffects (csTicks elapsed_ticks)
    {
      size_t i = 0;
      while (i < effects.GetSize ())
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

      default : {Report(PT::Error, "EffectsManager: Unknown effect type %d!", effect); return 0;}
      };

      // Find the factory and turn it into a factorywrapper.
      csRef<iMeshFactoryWrapper> effectfmw = engine->FindMeshFactory(factory);
      if (!effectfmw)
      {
        Report(PT::Error, "EffectsManager: Couldn't find particle factory: ' %s ' !", factory.GetData());
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
          Report(PT::Debug, "EffectsManager: We got a key, parsing it!");
          if (!strcmp (key->GetKey (), "duration"))
          {
            duration = atoi( key->GetValue () );
            Report(PT::Debug, "EffectsManager: Reading key value for duration!");
          }
          else if (!strcmp (key->GetKey (), "heightoffset"))
          {
            height = float(atof( key->GetValue () ));
            Report(PT::Debug, "EffectsManager: Reading key value for heightoffset!");
          }
        }
      }

      // Create the effect object.
      csRef<iMeshWrapper> effectmw = engine->CreateMeshWrapper(effectfmw, "effect", engine->FindSector("room"),csVector3(0,height,0));
      if (!effectmw)
      {
        Report(PT::Error, "EffectsManager: Particle MeshWrapper creation failed!");
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
        Report(PT::Error, "EffectsManager: Unable to create effect: %d!", effect);
        return false;
      }

      if (!parent)
      {
        Report(PT::Error, "EffectsManager: Unable to attach particle mesh %d to parent: no parent found!", effect);
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
        return Report(PT::Error, "EffectsManager: Unable to create effect: %d!", effect);

      // Offset the effect.
      csVector3 curpos = effectmw->QuerySceneNode()->GetMovable()->GetFullPosition();
      effectmw->QuerySceneNode()->GetMovable()->SetPosition(curpos + pos);
      effectmw->QuerySceneNode()->GetMovable()->UpdateMove();

      return true;
    }

    bool EffectsManager::CreateDecal (csVector3 pos, iCamera* camera)
    {
      csRef<iDecalManager> decalMgr = csLoadPluginCheck<iDecalManager> (obj_reg, "crystalspace.decal.manager");
      if (!decalMgr) 
        return Report(PT::Error, "EffectsManager: Unable to find decalmanager!");

      iMaterialWrapper * material = engine->GetMaterialList()->FindByName("movemarkermat");
      if (!material) 
        return Report(PT::Error, "EffectsManager: Unable to find material!");

      // create a template for our new decal
      csRef<iDecalTemplate> decalTemplate = decalMgr->CreateDecalTemplate(material);
      decalTemplate->SetTimeToLive(12.0f);
      //decalTemplate->SetMainColor(csColor4(1,1,1,1));

      if (!camera) 
        return Report(PT::Error, "EffectsManager: Unable to find camera!");


      csVector3 normal = camera->GetTransform().This2OtherRelative(csVector3(0,0,-1));
      csVector3 up = camera->GetTransform().This2OtherRelative(csVector3(0,1,0));

      // create the decal
      iDecal* decal = decalMgr->CreateDecal(decalTemplate, camera->GetSector(), pos, up, normal, 1.0f, 1.0f);

      return true;
    }

  } // Data namespace 
} // PT namespace 

