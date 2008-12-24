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

#include "effectsmanager.h"

#include <iutil/objreg.h>
//#include <iutil/plugin.h>
//#include <cstool/keyval.h>

#include <iengine/scenenode.h>
#include <iengine/movable.h>
#include <imesh/object.h>
#include <iengine/material.h>
#include <iengine/camera.h>
#include <iengine/collection.h>
#include <ivaria/decal.h>
#include <iengine/engine.h>

#include "common/event/eventmanager.h"
#include "common/event/entityevent.h"

CS_IMPLEMENT_PLUGIN

SCF_IMPLEMENT_FACTORY (EffectsManager)

void EffectsManager::Report(int severity, const char* msg, ...)
{
  va_list arg;
  va_start (arg, msg);
  csReportV(object_reg, severity, "peragro.effectsmanager", msg, arg);
  va_end (arg);
}

EffectsManager::EffectsManager (iBase* iParent)
  : scfImplementationType(this, iParent), object_reg(0)
{
}

EffectsManager::~EffectsManager ()
{
  if (eventQueue)
    eventQueue->RemoveListener(this);
}

bool CacheManager::UpdateOptions()
{
  csRef<iConfigManager> app_cfg = csQueryRegistry<iConfigManager> (object_reg);
  if (!app_cfg) return false;

  return true;
} // end UpdateOptions()

bool EffectsManager::Initialize (iObjectRegistry* obj_reg)
{
  object_reg = obj_reg;

  UpdateOptions();

  engine =  csQueryRegistry<iEngine> (obj_reg);
  if (!engine)
    return Report(CS_REPORTER_SEVERITY_BUG, "Failed to locate 3D engine!");

  vc =  csQueryRegistry<iVirtualClock> (obj_reg);
  if (!vc)
    return Report(CS_REPORTER_SEVERITY_BUG, "Failed to locate iVirtualClock!");

  eventQueue = csQueryRegistry<iEventQueue> (object_reg);
  if (!eventQueue) return false;

  nameRegistry = csEventNameRegistry::GetRegistry(object_reg);
  if (!nameRegistry) return false;

  eventQueue->RegisterListener(this);

  //Register for the Frame event, for Handle().
  eventQueue->RegisterListener (this, nameRegistry->GetID("crystalspace.frame"));

  // Register listener for atposition.
  PT_SETUP_HANDLER
  PT_REGISTER_LISTENER(EffectsManager, CreateEffect, "effect.atposition")

  return true;
}

bool CacheManager::HandleEvent(iEvent& ev)
{
  csTicks ticks = vc->GetElapsedTicks();

  HandleEffects(ticks);

  return true;
} // end HandleEvent()

bool EffectsManager::CreateEffect (iEvent& ev)
{
  using namespace PT::Events;

  std::string effectName = Helper::GetString(&ev, "effect");
  csVector3 pos = EntityHelper::GetPosition(&ev);
  std::string sectorName = Helper::GetString(&ev, "sector");

  iSector* sector = engine->FindSector(sectorName.c_str());

  CreateEffect (effectName, pos, sector);

  return false;
} // end CreateEffect()

void EffectsManager::HandleEffects (csTicks elapsed_ticks)
{
  size_t i = 0;
  while (i < effects.GetSize ())
  {
    if (effects[i]->Handle (elapsed_ticks)) 
      i++;
    else
    {
      effects.DeleteIndex (i);
    }
  }
} // end HandleEffects()

PT::Data::Effect* EffectsManager::GetEffectData (const std::string& effectName)
{
  Data::EffectDataManager* effMgr = PointerLibrary::getInstance()->getEffectDataManager();
  PT::Data::Effect* effect = effMgr->GetEffectByName(effectName);

  if (!effect)
  {
    Report(CS_REPORTER_SEVERITY_ERROR, "EffectsManager: No such effect: ' %s ' !",
      effectName.c_str());
    return 0;
  }

  return effect;
} // end GetEffectData()

bool EffectsManager::CreateEffect (const std::string& effectName, iMeshWrapper* parent)
{
  PT::Data::Effect* data = GetEffectData(effectName);
  if (!data) return false;

  csRef<Effect> effect (object_reg, data, parent);

  // Add it to the effect array for later processing
  effects.Push (effect);

  return true;
} // end CreateEffect()

bool EffectsManager::CreateEffect (const std::string& effectName, const csVector3& pos, iSector* sector)
{
  PT::Data::Effect* data = GetEffectData(effectName);
  if (!data) return false;

  csRef<Effect> effect (object_reg, data, pos, sector);

  // Add it to the effect array for later processing
  effects.Push (effect);

  return true;
} // end CreateEffect()

bool EffectsManager::CreateDecal (csVector3 pos)
{
/* TODO
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
*/
  return false;
} // end CreateDecal()

