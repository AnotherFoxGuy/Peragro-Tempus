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

#include "client/entity/ptentitymanager.h"

#include "client/combat/combatmanager.h"

#include <iutil/objreg.h>
#include <imap/loader.h>

#include <physicallayer/pl.h>
#include <physicallayer/propfact.h>
#include <physicallayer/propclas.h>
#include <physicallayer/entity.h>
#include <propclass/colldet.h>

#include "client/reporter/reporter.h"

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
    return Report(PT::Bug, "CombatMGR: Failed to locate ptEntityManager plugin");

  if (!effectsmgr)
    return Report(PT::Bug, "CombatMGR: Failed to locate EffectsManager plugin");

  if (!guimanager)
    return Report(PT::Bug, "CombatMGR: Failed to locate GUIManager plugin");

  if (!network)
    return Report(PT::Bug, "CombatMGR: Failed to locate Network plugin");

  return true;
}

void CombatMGR::hit (int targetId, int damage)
{
  // Lookup the ID to get the actual entity.
  //iCelEntity* target = entitymgr->findCelEntById(targetId);
  PtEntity* target = entitymgr->findPtEntById(targetId);

  if (!target)
  {
    Report(PT::Error, "CombatMGR: Couldn't find entity with ID %d !", targetId);
    return;
  }

  iCelEntity* targetcel = target->GetCelEntity();

  if (!targetcel)
  {
    Report(PT::Error, "CombatMGR: Couldn't find iCelEntity of entity with ID %d !", targetId);
    return;
  }

  csRef<iSpriteCal3DState> cal3dstate = scfQueryInterface<iSpriteCal3DState> (getMesh(targetcel)->GetMeshObject());
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
  Report(PT::Debug, "You %s %d points!", damage < 0 ? "healed" : "got hit for", damage);

  //test
  //guimanager->GetHUDWindow()->SetHP(target->GetHP());

}

void CombatMGR::die (int targetId)
{
  // Lookup the ID to get the actual entity.
  iCelEntity* target = entitymgr->findCelEntById(targetId);

  if (!target)
  {
    Report(PT::Error, "CombatMGR: Couldn't find dieing entity with ID %d !", targetId);
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
    Report(PT::Error, "CombatMGR: Couldn't find entity with ID %d !", targetId);
    return;
  }
  effectsmgr->CreateEffect(getMesh(target), EffectsManager::Levelup);
  //guimanager->GetCombatLog()->AddMessage("%s has gained a level.", target->GetName());

  csRef<iPcProperties> pcprop = CEL_QUERY_PROPCLASS_ENT(target, iPcProperties);
  if (!pcprop)return;
  Report(PT::Debug, "%s has gained a level.", pcprop->GetPropertyString(pcprop->GetPropertyIndex("Entity Name"))  );
}

void CombatMGR::experience (int exp)
{
  // Lookup the ID to get the actual entity.
  iCelEntity* ownent = entitymgr->getOwnCelEntity();

  if (!ownent)
  {
    Report(PT::Error, "CombatMGR: Couldn't find own entity!");
    return;
  }

  // Update the entity's experience(this will update the GUI aswell).
  //ownent->SetExp(exp);
  //guimanager->GetStatsWindow()->Setexp(exp);

  // We gaind experience.
  if (exp >= 0)
    //guimanager->GetCombatLog()->AddMessage("You gained %d experience points", exp);
    Report(PT::Debug, "You gained %d experience points!", exp);
  // We lost experience.
  else if (exp < 0)
    //guimanager->GetCombatLog()->AddMessage("You lost %d experience points", exp);
    Report(PT::Debug, "You lost %d experience points!", exp);

}

void CombatMGR::SkillUsageStart (unsigned int casterId, unsigned int targetId, int skillId, ptString error)
{

  /*
  *  Here the we start using skill, so we create the effect on the caster.
  */

  if (ptString(0,0) == error)
    Report(PT::Debug, "CombatMGR: %d cast %d on %d, error %s\n",casterId,targetId,skillId, *error);
  else
  {
    Report(PT::Notify, "CombatMGR: %s \n", *error);
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
    Report(PT::Error, "CombatMGR: Couldn't find target with ID %d !", targetId);
    return;
  }
  if (!caster)
  {
    Report(PT::Error, "CombatMGR: Couldn't find caster with ID %d !", casterId);
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

  default: Report(PT::Error, "CombatMGR: Unknown skill with ID %d !", skillId); return;
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
    Report(PT::Error, "CombatMGR: Couldn't find target with ID %d !", targetId);
    return;
  }
  if (!caster)
  {
    Report(PT::Error, "CombatMGR: Couldn't find attacker with ID %d !", casterId);
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

  default: Report(PT::Error, "CombatMGR: Unknown skill with ID %d !", skillId); return;
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
    Report(PT::Error, "CombatMGR: skillId is 0!");
    return;
  }
  // Get your own entity.
  iCelEntity* attacker = entitymgr->getOwnCelEntity();

  // Lookup the ID to get the actual entity.
  csRef<iPcProperties> pcprop = CEL_QUERY_PROPCLASS_ENT(target, iPcProperties);
  if (!pcprop) 
  {
    Report(PT::Error, "CombatMGR: Couldn't find pcprop for target!");
    return;
  }

  unsigned int targetId   = pcprop->GetPropertyLong(pcprop->GetPropertyIndex("Entity ID"));


  if (!targetId)
  {
    Report(PT::Error, "CombatMGR: Couldn't find ID for target!");
    return;
  }
  if (!attacker)
  {
    Report(PT::Error, "CombatMGR: Couldn't find attacker entity!");
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

  Report(PT::Debug, "CombatMGR: Sent SkillUsageStartRequestMessage.");

}
