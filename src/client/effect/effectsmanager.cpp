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

#include "factorymanager.h"

#include <iutil/objreg.h>
#include <iutil/plugin.h>
#include <cstool/keyval.h>

#include <iengine/scenenode.h>
#include <iengine/movable.h>
#include <imesh/object.h>
#include <iengine/material.h>
#include <iengine/camera.h>

#include <iengine/collection.h>

#include <ivaria/decal.h>

#include <iengine/engine.h>

#include "client/event/eventmanager.h"
#include "client/event/entityevent.h"

#include "client/data/effect/effectdatamanager.h"
#include "client/data/effect/dataeffect.h"

#include "client/entity/player/playerentity.h"

#include "client/pointer/pointer.h"
#include "common/reporter/reporter.h"

namespace PT
{
  namespace Effect
  {

    EffectsManager::EffectsManager ()
    {
      this->obj_reg = PointerLibrary::getInstance()->getObjectRegistry();

      factoryManager = new FactoryManager(obj_reg);
    }

    EffectsManager::~EffectsManager ()
    {
      // Delete the factory manager.
      delete factoryManager;
    }

    bool EffectsManager::Initialize ()
    {
      engine =  csQueryRegistry<iEngine> (obj_reg);
      if (!engine)
        return Report(PT::Bug, "EffectsManager: Failed to locate 3D engine!");

      // Register listener for atposition.
      using namespace PT::Events;
      EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
      csRef<EventHandlerCallback> cbEffect;
      cbEffect.AttachNew(new EventHandler<EffectsManager>(&EffectsManager::CreateEffect, this));
      evmgr->AddListener("effect.atposition", cbEffect);
      eventHandlers.Push(cbEffect);


      return true;
    }

    bool EffectsManager::CreateEffect (iEvent& ev)
    {
      using namespace PT::Events;

      std::string effectName = EntityHelper::GetString(&ev, "effect");
      csVector3 pos = EntityHelper::GetPosition(&ev);
      std::string sectorName = EntityHelper::GetString(&ev, "sector");

      iSector* sector = engine->FindSector(sectorName.c_str());

      CreateEffect (effectName, pos, sector);

      return false;
    } // end CreateEffect()

    void EffectsManager::HandleEffects (csTicks elapsed_ticks)
    {
      size_t i = 0;
      while (i < effects.GetSize ())
      {
        if (effects[i].Handle (elapsed_ticks)) i++;
        else
        {
          effects.DeleteIndex (i);
        }
      }
    } // end HandleEffects()

    iMeshWrapper* EffectsManager::CreateEffectMesh (const std::string& effectName)
    {
      Data::EffectDataManager* effMgr = PointerLibrary::getInstance()->getEffectDataManager();
      PT::Data::Effect* effect = effMgr->GetEffectByName(effectName);

      if (!effect)
      {
        Report(PT::Error, "EffectsManager: No such effect: ' %s ' !",
          effectName.c_str());
        return 0;
      }

      csRef<Factory> fact = factoryManager->Get(effect->GetMeshFile());
      Effect eff (fact, effect);
      eff.Load();

      if (!eff.GetMesh())
      {
        Report(PT::Error, "EffectsManager: Effect MeshWrapper creation failed!");
        return 0;
      }

      // Add it to the effect array for later processing
      effects.Push (eff);

      return eff.GetMesh();
    } // end CreateEffectMesh()

    bool EffectsManager::CreateEffect (const std::string& effectName, iMeshWrapper* parent)
    {
      // Parent the particle mesh to the parent entity
      csRef<iMeshWrapper> effectmw = CreateEffectMesh (effectName);
      if (!effectmw)
        return Report(PT::Error, "EffectsManager: Unable to create effect: %s!", effectName.c_str());

      if (!parent)
      {
        Report(PT::Error, "EffectsManager: Unable to attach particle mesh %s to parent: no parent found!", effectName.c_str());
        return false;
      }

      effectmw->QuerySceneNode()->SetParent(parent->QuerySceneNode());

      return true;
    } // end CreateEffect()

    bool EffectsManager::CreateEffect (const std::string& effectName, csVector3 pos, iSector* sector)
    {
      // Create the particle mesh.
      csRef<iMeshWrapper> effectmw = CreateEffectMesh (effectName);
      if (!effectmw)
        return Report(PT::Error, "EffectsManager: Unable to create effect: %s!", effectName.c_str());

      // Get the current sector.
      iSector* playersector = PT::Entity::PlayerEntity::Instance()->GetSector();

      // Offset the effect.
      csVector3 curpos = effectmw->QuerySceneNode()->GetMovable()->GetFullPosition();
      effectmw->QuerySceneNode()->GetMovable()->SetPosition(curpos + pos);
      if (sector)
      {
        effectmw->QuerySceneNode()->GetMovable()->SetSector(sector);
      }
      else if (playersector)
      {
        effectmw->QuerySceneNode()->GetMovable()->SetSector(playersector);
      }
      effectmw->PlaceMesh();
      effectmw->QuerySceneNode()->GetMovable()->UpdateMove();

      return true;
    } // end CreateEffect()

    bool EffectsManager::CreateDecal (csVector3 pos)
    {
      iCamera* camera = PT::Entity::PlayerEntity::Instance()->GetCamera()->GetCamera();
      if (!camera)
        return Report(PT::Error, "EffectsManager: Unable to find camera!");

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

      csVector3 normal = camera->GetTransform().This2OtherRelative(csVector3(0,0,-1));
      csVector3 up = camera->GetTransform().This2OtherRelative(csVector3(0,1,0));

      // create the decal
      //iDecal* decal = decalMgr->CreateDecal(decalTemplate, camera->GetSector(), pos, up, normal, 1.0f, 1.0f);
      decalMgr->CreateDecal(decalTemplate, camera->GetSector(), pos, up, normal, 1.0f, 1.0f);

      return true;
    } // end CreateDecal()

  } // Data namespace
} // PT namespace

