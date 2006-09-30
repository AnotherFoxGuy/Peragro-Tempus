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
  hp = Server::getServer()->getStatManager()->findByName(ptString("Health", strlen("Health")));
}

//void Skill::triggerSkill(CharSkill* skilldata, CharacterEntity* caster)
//{
//  Entity* target = Server::getServer()->getEntityManager()->findById(skilldata->target_id);
//  const char* targetname = target?*target->getName():"";
//  printf("Dummy Skill from %s targetting %s!\n",*caster->getName(),targetname);
//}

void Skill::castPrepare(CharacterEntity* caster, unsigned int target_id)
{
  CharSkill* skilldata = caster->getSkills()->findSkill(id);
  Entity* target = Server::getServer()->getEntityManager()->findById(target_id);

  SkillUsageStartResponseMessage response_msg;
  response_msg.setSkill(id);
  response_msg.setTarget(0);
  response_msg.setCaster(caster->getId());

  skilldata->caster_id = caster->getId();

  printf("Casting: %s starts to cast %s on %s\n", *caster->getName(), *this->getName(), *target->getName());
  printf("Casting: Skill State: %d \n", skilldata->state);

  if (caster->getStats()->getAmount(mp) < mpCost)
  {
    //Abort 'Not enough MP'
    response_msg.setError(ptString("Not enough MP", strlen("Not enough MP")));
    return ;
  }
  else if (caster->getDistanceTo(target) > range)
  {
    //Abort 'too far away'
    response_msg.setError(ptString("Too far away", strlen("Too far away")));
  }
  else if (skilldata->state != SkillState::READY)
  {
    //Abort 'Skill not ready'
    response_msg.setError(ptString("Skill not ready", strlen("Skill not ready")));
  }
  else
  {
    skilldata->skill = this;
    skilldata->target_id = target->getId();

    response_msg.setError(ptString());
    response_msg.setTarget(target->getId());
    response_msg.setMpCost(mpCost);

    caster->getStats()->takeStat(mp, mpCost);

    skilldata->state = SkillState::CASTING;
    skilldata->setInverval(skillTime);
    skilldata->start();
  }

  ByteStream bs;
  response_msg.serialise(&bs);
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

  SkillUsageInterruptMessage response_msg;
  response_msg.setSkill(skilldata->skill->getId());
  response_msg.setCaster(skilldata->caster_id);
  response_msg.setTarget(skilldata->target_id);

  ByteStream bs;
  response_msg.serialise(&bs);
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

  SkillUsageCompletionMessage response_msg;
  response_msg.setSkill(skilldata->skill->getId());
  response_msg.setCaster(skilldata->caster_id);
  response_msg.setTarget(skilldata->target_id);

  Entity* target = Server::getServer()->getEntityManager()->findById(skilldata->target_id);
  CharacterStats* stats = ((CharacterEntity*)target)->getStats();
  if (type == TYPE_HURT)
  {
    unsigned int health = stats->getAmount(hp);
    if ( health > power )
    {
      // target took damage
      stats->takeStat(hp, power);
      printf("Hitting %s with %d damage => %d HP remaining\n", *target->getName(), power, stats->getAmount(hp));
    }
    else
    {
      // target died
      stats->takeStat(hp, health);
      printf("Hitting %s with %d damage => Target Died => %d HP remaining\n", *target->getName(), power, stats->getAmount(hp));
    }
  }
  else if (type == TYPE_HEAL)
  {
    stats->addStat(hp, power);
    printf("Healing %s with %d Health => %d HP remaining\n", *target->getName(), power, stats->getAmount(hp));
    //no upper stats limit for now...
  }

  ByteStream bs;
  response_msg.serialise(&bs);
  Server::getServer()->broadCast(bs);
}

void Skill::castRecover(CharSkill* skilldata)
{
  skilldata->stop();
  skilldata->state = SkillState::READY;
}
