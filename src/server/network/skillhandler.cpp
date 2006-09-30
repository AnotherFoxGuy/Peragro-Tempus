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

#include "server/network/network.h"
#include "server/usermanager.h"
#include "common/entity/skillmanager.h"

void SkillHandler::handleSkillUsageStartRequest(GenericMessage* msg)
{
  Connection* conn = msg->getConnection();
  if (!conn) return;

  CharacterEntity* user_ent = conn->getUser()->getEntity();
  ptString name = user_ent->getName();

  SkillUsageStartRequestMessage request_msg;
  request_msg.deserialise(msg->getByteStream());

  Skill* skill = server->getSkillManager()->findById(request_msg.getSkill());
  if (skill)
    skill->castPrepare(user_ent, request_msg.getTarget());
}

void SkillHandler::handleSkillUsageStopRequest(GenericMessage* msg)
{
  Connection* conn = msg->getConnection();
  if (!conn) return;

  CharacterEntity* user_ent = conn->getUser()->getEntity();
  ptString name = user_ent->getName();

  SkillUsageStopRequestMessage request_msg;
  request_msg.deserialise(msg->getByteStream());

  Skill* skill = server->getSkillManager()->findById(request_msg.getSkill());
  skill->castInterrupt(user_ent->getSkills()->findSkill(skill->getId()));
}
