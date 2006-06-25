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

#ifndef _SKILLENGINE_H_
#define _SKILLENGINE_H_

#include "common/entity/entity.h"
#include "common/entity/characterentity.h"
#include "common/entity/item.h"
#include "common/util/array.h"
#include "common/util/timer.h"
#include "common/util/sleep.h"
#include "server/server.h"

class SkillEngine : public Timer
{
private:

  // Timer implementation
  void timeOut()
  {
    EntityManager* em = Server::getServer()->getEntityManager();
    for (size_t i=0; i<em->getEntityCount(); i++)
    {
      Entity* entity = em->getEntity(i);
      if (entity->getType() == Entity::PlayerEntity || entity->getType() == Entity::NPCEntity)
        procEntity((CharacterEntity*)em->getEntity(i));
    }
  }

  void procEntity(CharacterEntity* entity)
  {
    CharacterSkills* skills = entity->getSkills();
    for (size_t i=0; i<skills->getSkillCount(); i++)
    {
      CharSkill* skill = skills->getSkill(i);
      if (skill->isActive())
        procSkill(skill, entity);
    }
  }

  void procSkill(CharSkill* charskill, CharacterEntity* entity)
  {
    Skill* skill = Server::getServer()->getSkillManager()->findById(charskill->skill_id);
    skill->triggerSkill(charskill, entity);
  }

public:
  SkillEngine()
  {
    this->setInverval(1);
    this->start();
  }
};

#endif // _SKILLENGINE_H_
