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

//#include "client/entity/ptcelentity.h"
#include "client/entity/ptentitymanager.h"

#include "client/combat/combatmanager.h"

#include "csgeom/path.h"
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

#include "physicallayer/pl.h"
#include "physicallayer/propfact.h"
#include "physicallayer/propclas.h"
#include "physicallayer/entity.h"
#include "physicallayer/persist.h"
#include "celtool/initapp.h"
#include "celtool/persisthelper.h"
#include "celtool/stdparams.h"
#include "behaviourlayer/bl.h"
#include "propclass/test.h"
#include "propclass/mesh.h"
#include "propclass/meshsel.h"
#include "propclass/solid.h"
#include "propclass/inv.h"
#include "propclass/chars.h"
#include "propclass/move.h"
#include "propclass/prop.h"
#include "propclass/tooltip.h"
#include "propclass/defcam.h"
#include "propclass/gravity.h"
#include "propclass/timer.h"
#include "propclass/region.h"
#include "propclass/input.h"
#include "propclass/navgraph.h"
#include "propclass/linmove.h"
#include "propclass/actormove.h"
#include "propclass/quest.h"
#include "propclass/trigger.h"
#include "propclass/zone.h"
#include "propclass/sound.h"
#include <propclass/colldet.h>

#include "client/gui/gui.h"
#include "client/gui/guimanager.h"

#include "client/effects/effectsmanager.h"

#include "client/network/network.h"

#include "common/network/netmessage.h"

CombatMGR::CombatMGR()
{
}

CombatMGR::~CombatMGR()
{
}

iMeshWrapper* CombatMGR::getMesh(iCelEntity* entity)
{
 csRef<iPcMesh> pcmesh = CEL_QUERY_PROPCLASS_ENT(entity, iPcMesh);
 if (!pcmesh) return 0;
 csRef<iMeshWrapper> parent = pcmesh->GetMesh();
 if (!parent) return 0;

 return parent;
}

bool CombatMGR::Initialize ()
{
  client      = PointerLibrary::getInstance()->getClient();
  entitymgr   = PointerLibrary::getInstance()->getEntityManager();
  effectsmgr  = PointerLibrary::getInstance()->getEffectsManager();
  guimanager  = PointerLibrary::getInstance()->getGUIManager();
  network     = PointerLibrary::getInstance()->getNetwork();

  if (!entitymgr)
  {
    client->ReportError("CombatMGR: Failed to locate ptEntityManager plugin");
    return false;
  }

  if (!effectsmgr)
  {
    client->ReportError("CombatMGR: Failed to locate EffectsManager plugin");
    return false;
  }

  if (!guimanager)
  {
    client->ReportError("CombatMGR: Failed to locate GUIManager plugin");
    return false;
  }

  if (!network)
  {
    client->ReportError("CombatMGR: Failed to locate Network plugin");
    return false;
  }

  return true;
}

void CombatMGR::hit (int targetId, int damage)
{
  // Lookup the ID to get the actual entity.
  //iCelEntity* target = entitymgr->findCelEntById(targetId);
  PtEntity* target = entitymgr->findPtEntById(targetId);

  if (!target)
  {
    printf("CombatMGR: Couldn't find entity with ID %d !", targetId);
    return;
  }

  iCelEntity* targetcel = target->GetCelEntity();

  if (!targetcel)
  {
    printf("CombatMGR: Couldn't find iCelEntity of entity with ID %d !", targetId);
    return;
  }

  csRef<iSpriteCal3DState> cal3dstate = SCF_QUERY_INTERFACE (getMesh(targetcel)->GetMeshObject(), iSpriteCal3DState);
  if (!cal3dstate) return;

  // Damage is positive, we got hurt.
  if (damage > 0)
  {
    effectsmgr->CreateEffect(getMesh(targetcel), EffectsManager::Blood);
    cal3dstate->SetAnimAction("walkcycle", 0.5f, 0.5f);
  }
  // Damage is negative, we got healed.
  else if (damage < 0)
  {
    effectsmgr->CreateEffect(getMesh(targetcel), EffectsManager::Healeffect);
    //target->SetAction("heal");
  }
  else if (damage == 0)
  {
    effectsmgr->CreateEffect(getMesh(targetcel), EffectsManager::Deflect);
    //target->SetAction("deflect");
  }
  // Update the entity's HP(this will update the GUI aswell).
  //target->AddToHP(-damage);
  printf("You %s %d points!\n", damage < 0 ? "healed" : "got hit for", damage);

  //test
  //guimanager->GetHUDWindow()->SetHP(target->GetHP());

}

void CombatMGR::die (int targetId)
{
  // Lookup the ID to get the actual entity.
  iCelEntity* target = entitymgr->findCelEntById(targetId);

  if (!target)
  {
    printf("CombatMGR: Couldn't find dieing entity with ID %d !", targetId);
    return;
  }
  effectsmgr->CreateEffect(getMesh(target), EffectsManager::Die);
  //target->SetHP(0);
  // Perfrom the die animation and lock it.
  //target->SetAction("die", true);

}

void CombatMGR::levelup (int targetId)
{
  // Lookup the ID to get the actual entity.
  iCelEntity* target = entitymgr->findCelEntById(targetId);

  if (!target)
  {
    printf("CombatMGR: Couldn't find dieing entity with ID %d !", targetId);
    return;
  }
  effectsmgr->CreateEffect(getMesh(target), EffectsManager::Levelup);
  //guimanager->GetCombatLog()->AddMessage("%s has gained a level.", target->GetName());

  csRef<iPcProperties> pcprop = CEL_QUERY_PROPCLASS_ENT(target, iPcProperties);
  if (!pcprop)return;
  printf("%s has gained a level.\n", pcprop->GetPropertyString(pcprop->GetPropertyIndex("Entity Name"))  );
}

void CombatMGR::experience (int exp)
{
  // Lookup the ID to get the actual entity.
  iCelEntity* ownent = entitymgr->getOwnEntity();

  if (!ownent)
  {
    printf("CombatMGR: Couldn't find own entity!");
    return;
  }

  // Update the entity's experience(this will update the GUI aswell).
  //ownent->SetExp(exp);
  //guimanager->GetStatsWindow()->Setexp(exp);

  // We gaind experience.
  if (exp >= 0)
    //guimanager->GetCombatLog()->AddMessage("You gained %d experience points", exp);
     printf("You gained %d experience points!\n", exp);
  // We lost experience.
  else if (exp < 0)
    //guimanager->GetCombatLog()->AddMessage("You lost %d experience points", exp);
     printf("You lost %d experience points!\n", exp);

}

void CombatMGR::SkillUsageStart (unsigned int casterId, unsigned int targetId, int skillId, ptString error)
{
  
  /*
  *  Here the we start using skill, so we create the effect on the caster.
  */

  if (ptString(0,0) == error)
    printf("CombatMGR: %d cast %d on %d, error %s\n",casterId,targetId,skillId, *error);
  else
  {
    printf("CombatMGR: %s \n", *error);
    char msg[1024];
    sprintf(msg,"%s", *error);
    guimanager->GetChatWindow()->AddMessage(msg);
    return;
  }

  // Lookup the IDs to get the actual entities.
  iCelEntity* caster = entitymgr->findCelEntById(casterId);
  iCelEntity* target = entitymgr->findCelEntById(targetId);

  if (!target)
  {
    printf("CombatMGR: Couldn't find target with ID %d !\n", targetId);
    return;
  }
  if (!caster)
  {
    printf("CombatMGR: Couldn't find caster with ID %d !\n", casterId);
    return;
  }

  csString caststring;

  switch(skillId)
  {
  case CombatMGR::Heal:
    effectsmgr->CreateEffect(getMesh(target), EffectsManager::Levelup);
    caststring = "starts rejuvenating";
    break;
  case CombatMGR::EnergySpear:
    effectsmgr->CreateEffect(getMesh(target), EffectsManager::Energyspear);
    //attacker->SetAction("casting2");
    caststring = "starts casting Energy Spear and attacks";
    break;
  case CombatMGR::Melee:
    //attacker->SetAction("melee");
    caststring = "starts taking a swing at";
    break;
  case CombatMGR::EnergyBind:
    effectsmgr->CreateEffect(getMesh(caster), EffectsManager::Pentagram);
    caststring = "starts casting Energy Bind on";
    break;

  default: printf("CombatMGR: Unknown skill with ID %d !\n", skillId); return;
  }

  // Send a message to the GUI.
  csRef<iPcProperties> pcpropa = CEL_QUERY_PROPCLASS_ENT(caster, iPcProperties);
  if (!pcpropa)return;
  csRef<iPcProperties> pcpropt = CEL_QUERY_PROPCLASS_ENT(target, iPcProperties);
  if (!pcpropt)return;
  csString castername = pcpropa->GetPropertyString(pcpropa->GetPropertyIndex("Entity Name"));
  csString targetname = pcpropt->GetPropertyString(pcpropt->GetPropertyIndex("Entity Name"));

  char msg[1024];
  sprintf(msg,"%s %s %s.",castername.GetData(), caststring.GetData(), targetname.GetData() );
  guimanager->GetChatWindow()->AddMessage(msg);

}

void CombatMGR::SkillUsageComplete (unsigned int casterId, unsigned int targetId, int skillId)
{
  /*
  *  Here the used skill completed succesfully, so we create the effect on the target.
  */

  // Lookup the IDs to get the actual entities.
  iCelEntity* caster = entitymgr->findCelEntById(casterId);
  iCelEntity* target = entitymgr->findCelEntById(targetId);

  if (!target)
  {
    printf("CombatMGR: Couldn't find target with ID %d !\n", targetId);
    return;
  }
  if (!caster)
  {
    printf("CombatMGR: Couldn't find attacker with ID %d !\n", casterId);
    return;
  }

  csString attackstring;

  switch(skillId)
  {
  case CombatMGR::Heal:
    effectsmgr->CreateEffect(getMesh(target), EffectsManager::Levelup);
    attackstring = "rejuvenates";
    break;
  case CombatMGR::EnergySpear:
    effectsmgr->CreateEffect(getMesh(target), EffectsManager::Energyspear);
    //attacker->SetAction("casting2");
    attackstring = "casts Energy Spear and attacks";
    break;
  case CombatMGR::Melee:
    //attacker->SetAction("melee");
    attackstring = "takes a swing at";
    break;
  case CombatMGR::EnergyBind:
    effectsmgr->CreateEffect(getMesh(target), EffectsManager::Energysphere);
    attackstring = "casts Energy Bind on";
    break;

  default: printf("CombatMGR: Unknown skill with ID %d !\n", skillId); return;
  }

  // Send a message to the GUI.

  //char msg[1024];
  //sprintf(msg, "%s %s %s",attacker->GetName(), attackstring->GetData(), target->GetName() );
  //guimanager->GetCombatLog()->AddMessage(msg);

  csRef<iPcProperties> pcpropa = CEL_QUERY_PROPCLASS_ENT(caster, iPcProperties);
  if (!pcpropa)return;
  csRef<iPcProperties> pcpropt = CEL_QUERY_PROPCLASS_ENT(target, iPcProperties);
  if (!pcpropt)return;
  csString castername = pcpropa->GetPropertyString(pcpropa->GetPropertyIndex("Entity Name"));
  csString targetname = pcpropt->GetPropertyString(pcpropt->GetPropertyIndex("Entity Name"));

  char msg[1024];
  sprintf(msg,"%s %s %s.",castername.GetData(), attackstring.GetData(), targetname.GetData() );
  guimanager->GetChatWindow()->AddMessage(msg);

}

void CombatMGR::RequestSkillUsageStart (iCelEntity* target, unsigned int skillId)
{
  if (!skillId)
  {
    printf("CombatMGR: skillId is 0!\n");
    return;
  }
  // Get your own entity.
  iCelEntity* attacker = entitymgr->getOwnEntity();
  unsigned int attackerId = entitymgr->GetOwnId();

  // Lookup the ID to get the actual entity.
  csRef<iPcProperties> pcprop = CEL_QUERY_PROPCLASS_ENT(target, iPcProperties);
  if (!pcprop) 
  {
    printf("CombatMGR: Couldn't find pcprop for target!\n");
    return;
  }

  unsigned int targetId   = pcprop->GetPropertyLong(pcprop->GetPropertyIndex("Entity ID"));


  if (!targetId)
  {
    printf("CombatMGR: Couldn't find ID for target!\n");
    return;
  }
  if (!attacker)
  {
    printf("CombatMGR: Couldn't find attacker entity!\n");
    return;
  }

  /* Next are a couple of checks to not stress the server/network
  *  with attacks it will reject anyways.
  *  Ofcourse it still needs to be implemented on the server
  *  to avoid cheating.
  */

  /*
  // Calculate the distance between both enemies.
  float distance = distance(target, attacker);
  // See if the attacker is actually facing the target
  bool lookat = lookat(attacker, target);
  if (distance > 10.0f)
  {
    guimanager->GetCombatLog()->AddMessage("Target is out of range!");
    return;
  }
  if (!lookat)
  {
    guimanager->GetCombatLog()->AddMessage("You should face your target!");
    return;
  }
   */

  // Prepare and send the network message.
  SkillUsageStartRequestMessage msg;
  msg.setTarget(targetId);
  msg.setSkill(skillId);
  network->send(&msg);

  printf("CombatMGR: Sent SkillUsageStartRequestMessage.\n");
 
}
