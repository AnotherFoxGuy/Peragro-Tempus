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

#include "client/combat/combatmanager.h"

#include <iutil/objreg.h>
#include <imap/loader.h>

#include <physicallayer/pl.h>
#include <physicallayer/propfact.h>
#include <physicallayer/propclas.h>
#include <physicallayer/entity.h>
#include <propclass/colldet.h>
#include <imesh/genmesh.h>
#include <imesh/gmeshskel2.h>
#include <imesh/skeleton.h>

#include "client/reporter/reporter.h"

#include "client/gui/gui.h"
#include "client/gui/guimanager.h"

#include "common/data/skill.h"
#include "common/data/skilldatamanager.h"

#include "client/data/effect/effectsmanager.h"

#include "client/entity/entitymanager.h"

#include "client/network/network.h"
#include "common/network/netmessage.h"

CombatMGR::CombatMGR() : playing(false)
{
}

CombatMGR::~CombatMGR()
{
}

iMeshWrapper* CombatMGR::getMesh(PT::Entity::Entity* entity)
{
  iCelEntity* celentity = entity->GetCelEntity();
  if (!celentity) return 0;
  csRef<iPcMesh> pcmesh = CEL_QUERY_PROPCLASS_ENT(celentity, iPcMesh);
  if (!pcmesh) return 0;
  csRef<iMeshWrapper> parent = pcmesh->GetMesh();
  if (!parent) return 0;

  return parent;
}

bool CombatMGR::Initialize ()
{
  entitymgr    = PointerLibrary::getInstance()->getEntityManager();
  effectsmgr   = PointerLibrary::getInstance()->getEffectsManager();
  guimanager   = PointerLibrary::getInstance()->getGUIManager();
  skillmanager = PointerLibrary::getInstance()->getSkillDataManager();
  network      = PointerLibrary::getInstance()->getNetwork();

  // Register listener for ActionHit.
  PT::Events::EventHandler<CombatMGR>* cbActionHit = new PT::Events::EventHandler<CombatMGR>(&CombatMGR::ActionHit, this);
  PointerLibrary::getInstance()->getEventManager()->AddListener("input.ACTION_HIT", cbActionHit);

  if (!entitymgr)
    return Report(PT::Bug, "CombatMGR: Failed to locate ptEntityManager plugin");

  if (!effectsmgr)
    return Report(PT::Bug, "CombatMGR: Failed to locate EffectsManager plugin");

  if (!guimanager)
    return Report(PT::Bug, "CombatMGR: Failed to locate GUIManager plugin");

  if (!skillmanager)
    return Report(PT::Bug, "CombatMGR: Failed to locate SkillDataManager plugin");

  if (!network)
    return Report(PT::Bug, "CombatMGR: Failed to locate Network plugin");

  return true;
}

void CombatMGR::hit (int targetId, int damage)
{
  // Lookup the ID to get the actual entity.
  PT::Entity::Entity* target = entitymgr->findPtEntById(targetId);

  if (!target)
  {
    Report(PT::Error, "CombatMGR: Couldn't find entity with ID %d !", targetId);
    return;
  }

  // Damage is positive, we got hurt.
  if (damage > 0)
  {
    effectsmgr->CreateEffect("Blood", getMesh(target));
    if (target->GetType() == PT::Entity::PCEntityType)
    {
      ((PT::Entity::PcEntity*) target)->PlayAnimation("hit", 0.1f);
    }
  }
  // Damage is negative, we got healed.
  else if (damage < 0)
  {
    effectsmgr->CreateEffect("Heal", getMesh(target));
    //target->SetAction("heal");
  }
  else if (damage == 0)
  {
    effectsmgr->CreateEffect("Deflect", getMesh(target));
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
  PT::Entity::Entity* target = entitymgr->findPtEntById(targetId);

  if (!target)
  {
    Report(PT::Error, "CombatMGR: Couldn't find dieing entity with ID %d !", targetId);
    return;
  }

  effectsmgr->CreateEffect("Die", getMesh(target));
  //target->SetHP(0);
  // Perfrom the die animation and lock it.
  //target->SetAction("die", true);

}

void CombatMGR::levelup (int targetId)
{
  // Lookup the ID to get the actual entity.
  PT::Entity::Entity* target = entitymgr->findPtEntById(targetId);

  if (!target)
  {
    Report(PT::Error, "CombatMGR: Couldn't find entity with ID %d !", targetId);
    return;
  }

  effectsmgr->CreateEffect("Levelup", getMesh(target));
  //guimanager->GetCombatLog()->AddMessage("%s has gained a level.", target->GetName());

  Report(PT::Debug, "%s has gained a level.", target->GetName().c_str() );
}

void CombatMGR::experience (int exp)
{
  if (!PT::Entity::PlayerEntity::Instance()) return;
  // Lookup the ID to get the actual entity.
  PT::Entity::Entity* entity = PT::Entity::PlayerEntity::Instance();

  if (!entity)
  {
    Report(PT::Error, "CombatMGR: Couldn't find player entity!");
    return;
  }

  // Update the entity's experience(this will update the GUI aswell).
  //entity->SetExp(exp);
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
  PT::Entity::Entity* caster = entitymgr->findPtEntById(casterId);
  PT::Entity::Entity* target = entitymgr->findPtEntById(targetId);

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

  std::string caststring;
  PT::Data::SkillDataManager* skillmgr = PointerLibrary::getInstance()->getSkillDataManager();
  PT::Data::Skill* skill = skillmgr->GetSkillById(skillId);

  if (skill)
  {
    effectsmgr->CreateEffect(skill->GetEffects().caster.c_str(), getMesh(caster));
    caststring = skill->GetStartString();
    if (caster->GetType() == PT::Entity::PCEntityType)
    {
      ((PT::Entity::PcEntity*)caster)->PlayAnimation(skill->GetEffects().castanim.c_str());
    }
  }
  else
    Report(PT::Error, "CombatMGR: Unknown skill with ID %d !", skillId);

  char msg[1024];
  sprintf(msg,"%s %s %s.", caster->GetName().c_str(),
                           caststring.c_str(),
                           target->GetName().c_str());
  guimanager->GetChatWindow()->AddMessage(msg);

}

void CombatMGR::SkillUsageComplete (unsigned int casterId, unsigned int targetId, int skillId)
{
  /*
  *  Here the used skill completed succesfully, so we create the effect on the target.
  */

  // Lookup the IDs to get the actual entities.
  PT::Entity::Entity* caster = entitymgr->findPtEntById(casterId);
  PT::Entity::Entity* target = entitymgr->findPtEntById(targetId);

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

  std::string caststring;
  PT::Data::SkillDataManager* skillmgr = PointerLibrary::getInstance()->getSkillDataManager();
  PT::Data::Skill* skill = skillmgr->GetSkillById(skillId);

  if (skill)
  {
    effectsmgr->CreateEffect(skill->GetEffects().target.c_str(), getMesh(target));
    caststring = skill->GetCompleteString();
    if (target->GetType() == PT::Entity::PCEntityType)
    {
      ((PT::Entity::PcEntity*)target)->PlayAnimation(skill->GetEffects().targetanim.c_str());
    }
  }
  else
    Report(PT::Error, "CombatMGR: Unknown skill with ID %d !", skillId);

  char msg[1024];
  sprintf(msg,"%s %s %s.", caster->GetName().c_str(),
                           caststring.c_str(),
                           target->GetName().c_str());
  guimanager->GetChatWindow()->AddMessage(msg);


}

void CombatMGR::RequestSkillUsageStart (iCelEntity* target, unsigned int skillId)
{
  // Lookup the ID to get the actual entity.
  csRef<iPcProperties> pcprop = CEL_QUERY_PROPCLASS_ENT(target, iPcProperties);
  if (!pcprop)
  {
    Report(PT::Error, "CombatMGR: Couldn't find pcprop for target!");
    return;
  }

  unsigned int targetId   = pcprop->GetPropertyLong(pcprop->GetPropertyIndex("Entity ID"));

  RequestSkillUsageStart (targetId, skillId);
}

void CombatMGR::RequestSkillUsageStart (unsigned int targetId, unsigned int skillId)
{
  if (!skillId)
  {
    Report(PT::Error, "CombatMGR: skillId is 0!");
    return;
  }

  if (!PT::Entity::PlayerEntity::Instance()) return;

  // Get your own entity.
  PT::Entity::Entity* attacker = PT::Entity::PlayerEntity::Instance();


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

  Report(PT::Debug, "CombatMGR: Sent SkillUsageStartRequestMessage target: %d skillid: %.", targetId, skillId);

}

bool CombatMGR::ActionHit(PT::Events::Eventp ev)
{
  using namespace PT::Events;

  if (playing)
  {
    InputEvent* inputEv = GetInputEvent<InputEvent*>(ev);
    if (!inputEv) return false;

    if (!inputEv->released)
    {
      hit(entitymgr->GetPlayerId(), 20);
    }
  }

  return true;
}
