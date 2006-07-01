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

#include "entity.h"
#include "skill.h"
#include "characterentity.h"
#include "characterskills.h"
#include "entitymanager.h"
#include "server/server.h"
#include "statmanager.h"
#include "common/network/skillmessages.h"
#include "server/network/connection.h"
#include "server/user.h"

Skill::Skill() : id(-1), range(0) 
{
  mp = Server::getServer()->getStatManager()->findByName(ptString("Mana", strlen("Mana")));
}

//void Skill::triggerSkill(CharSkill* skilldata, CharacterEntity* caster)
//{
//  Entity* target = Server::getServer()->getEntityManager()->findById(skilldata->target_id);
//  const char* targetname = target?*target->getName():"";
//  printf("Dummy Skill from %s targetting %s!\n",*caster->getName(),targetname);
//}

void Skill::castPrepare(CharacterEntity* caster)
{
  CharSkill* skilldata = caster->getSkills()->findSkill(id);
  Entity* target = Server::getServer()->getEntityManager()->findById(skilldata->target_id);

  SkillUsageStartResponseMessage reponse_msg;
  reponse_msg.setSkill(id);
  reponse_msg.setTarget(0);

  if (caster->getStats()->getAmount(mp) < mpCost)
  {
    //Abort 'Not enough MP'
    reponse_msg.setError(ptString("Not enough MP", strlen("Not enough MP")));
    return ;
  }
  else if (caster->getDistanceTo(target) > range)
  {
    //Abort 'too far away'
    reponse_msg.setError(ptString("Too far away", strlen("Too far away")));
  }
  else
  {
    skilldata->skill = this;

    reponse_msg.setError(ptString());
    reponse_msg.setTarget(target->getId());

    skilldata->state = SkillState::CASTING;
    skilldata->setInverval(skillTime);
    skilldata->start();
  }

  ByteStream bs;
  reponse_msg.serialise(&bs);
  Server::getServer()->broadCast(bs);
}

void Skill::castInterrupt(CharSkill* skilldata)
{
  if (skilldata->state == SkillState::CASTING)
  {
    skilldata->stop();
    skilldata->state = SkillState::RECOVER;
    skilldata->setInverval(reuseDelay);
    skilldata->start();
  }

  Entity* caster = Server::getServer()->getEntityManager()->findById(skilldata->caster_id);
  if (!caster || caster->getType() != Entity::PlayerEntity)
    return;

  SkillUsageInterruptMessage reponse_msg;
  reponse_msg.setSkill(skilldata->skill->getId());

  ByteStream bs;
  reponse_msg.serialise(&bs);
  Server::getServer()->broadCast(bs);
}

void Skill::castExecute(CharSkill* skilldata)
{
  if (skilldata->state == SkillState::CASTING)
  {
    skilldata->stop();
    skilldata->state = SkillState::RECOVER;
    skilldata->setInverval(reuseDelay);
    skilldata->start();
  }

  SkillUsageCompletionMessage reponse_msg;
  reponse_msg.setSkill(skilldata->skill->getId());

  ByteStream bs;
  reponse_msg.serialise(&bs);
  Server::getServer()->broadCast(bs);
}

void Skill::castRecover(CharSkill* skilldata)
{
  skilldata->stop();
  skilldata->state = SkillState::READY;
}
