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
#include <iutil/cfgmgr.h>
#include <iutil/vfs.h>
#include <iutil/document.h>

#include <iengine/scenenode.h>
#include <iengine/movable.h>
#include <imesh/object.h>
#include <iengine/material.h>
#include <iengine/camera.h>
#include <iengine/collection.h>
#include <ivaria/decal.h>
#include <iengine/engine.h>

#include <csutil/csevent.h>

#include "effecttemplate.h"
#include "effect.h"

CS_IMPLEMENT_PLUGIN

SCF_IMPLEMENT_FACTORY (EffectsManager)

//=================================================================
std::string EffectsManager::GetString(const iEvent* event, const char* name)
{
  const char* str = "";
  if (event->Retrieve(name, str) != csEventErrNone)
    Report(CS_REPORTER_SEVERITY_ERROR, "GetString '%s' failed!", name);

  std::string text = str;
  return text;
}

csVector3 EffectsManager::GetVector3(const iEvent* ev, const char* name)
{
  std::string nm = name;
  std::string nmX = nm + "_x";
  std::string nmY = nm + "_y";
  std::string nmZ = nm + "_z";

  float x, y, z;
  x = y = z = 0.0f;

  if (ev->Retrieve(nmX.c_str(), x) != csEventErrNone)
    Report(CS_REPORTER_SEVERITY_ERROR, "GetVector3 failed! X attribute not present!");
  if (ev->Retrieve(nmY.c_str(), y) != csEventErrNone)
    Report(CS_REPORTER_SEVERITY_ERROR, "GetVector3 failed! Y attribute not present!");
  if (ev->Retrieve(nmZ.c_str(), z) != csEventErrNone)
    Report(CS_REPORTER_SEVERITY_ERROR, "GetVector3 failed! Z attribute not present!");

  csVector3 pos(x, y, z);
  return pos;
}

//=================================================================

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

bool EffectsManager::UpdateOptions()
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
  {
    Report(CS_REPORTER_SEVERITY_BUG, "Failed to locate 3D engine!");
    return false;
  }

  vc =  csQueryRegistry<iVirtualClock> (obj_reg);
  if (!vc)
  {
    Report(CS_REPORTER_SEVERITY_BUG, "Failed to locate iVirtualClock!");
    return false;
  }

  eventQueue = csQueryRegistry<iEventQueue> (object_reg);
  if (!eventQueue) return false;

  nameRegistry = csEventNameRegistry::GetRegistry(object_reg);
  if (!nameRegistry) return false;

  eventQueue->RegisterListener(this);

  //Register for the Frame event, for Handle().
  eventQueue->RegisterListener (this, nameRegistry->GetID("crystalspace.frame"));

  // Register listener for atposition.
  eventQueue->RegisterListener (this, nameRegistry->GetID("effect.atposition"));

  return true;
} // end Initialize()

bool EffectsManager::LoadEffectTemplates(const std::string& fileName)
{
  csRef<iVFS> vfs = csQueryRegistry<iVFS>(object_reg);
  if (!vfs.IsValid())
  {
    Report(CS_REPORTER_SEVERITY_BUG, "Failed to locate VFS!\n");
    return false;
  }

  csRef<iDataBuffer> xmlfile = vfs->ReadFile(fileName.c_str());
  if (!xmlfile.IsValid())
  {
    Report(CS_REPORTER_SEVERITY_ERROR, "Can't load file '%s'!\n", fileName.c_str());
    return false;
  }

  csRef<iDocumentSystem> docsys(csQueryRegistry<iDocumentSystem>(object_reg));
  csRef<iDocument> doc(docsys->CreateDocument());

  const char* error = doc->Parse(xmlfile, true);
  if (error)
  {
    Report(CS_REPORTER_SEVERITY_ERROR, "'%s'!\n", error);
    return false;
  }

  csRef<iDocumentNode> xml = doc->GetRoot();
  xml = xml->GetNode("effects");
  if (!xml.IsValid()) return false;

  csRef<iDocumentNodeIterator> it(xml->GetNodes("effect"));
  while (it->HasNext())
  {
    csRef<iDocumentNode> node(it->Next());
    if (!LoadEffectTemplate(node)) return false;
  }

  return true;
} // end LoadEffectTemplates()

bool EffectsManager::LoadEffectTemplate(iDocumentNode* node)
{
  csRef<EffectTemplate> effect;
  effect.AttachNew(new EffectTemplate(node));

  if (effectTemplates.In(effect->GetName()))
    Report(CS_REPORTER_SEVERITY_WARNING, 
            "EffectTemplate with name: '%s' already present, replaced!",
            effect->GetName().c_str());

  effectTemplates.PutUnique(effect->GetName(), effect);

  return true;
} // end LoadEffectTemplate()

bool EffectsManager::HandleEvent(iEvent& ev)
{
  if (ev.GetName() == nameRegistry->GetID("crystalspace.frame"))
  {
    csTicks ticks = vc->GetElapsedTicks();
    HandleEffects(ticks);
  }
  else if (ev.GetName() == nameRegistry->GetID("effect.atposition"))
  {
    return CreateEffect(ev);
  }

  return true;
} // end HandleEvent()

bool EffectsManager::CreateEffect (iEvent& ev)
{
  std::string effectName = GetString(&ev, "effect");
  csVector3 pos = GetVector3(&ev, "position");
  std::string sectorName = GetString(&ev, "sector");

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

EffectTemplate* EffectsManager::GetEffectTemplate (const std::string& effectName)
{
  if (effectTemplates.In(effectName))
    return effectTemplates.Get(effectName, 0);
  else
  {
    Report(CS_REPORTER_SEVERITY_ERROR, "No such EffectTemplate: '%s' !",
      effectName.c_str());
    return 0;
  }
} // end GetEffectData()

bool EffectsManager::CreateEffect (const std::string& effectName, iMeshWrapper* parent)
{
  csRef<EffectTemplate> data = GetEffectTemplate(effectName);
  if (!data) return false;

  csRef<Effect> effect;
  effect.AttachNew(new Effect(object_reg, data, parent));

  // Add it to the effect array for later processing
  effects.Push (effect);

  return true;
} // end CreateEffect()

bool EffectsManager::CreateEffect (const std::string& effectName, const csVector3& pos, iSector* sector)
{
  csRef<EffectTemplate> data = GetEffectTemplate(effectName);
  if (!data) return false;

  csRef<Effect> effect;
  effect.AttachNew(new Effect(object_reg, data, pos, sector));

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

