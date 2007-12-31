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

#include "networkhelper.h"
#include "server/network/network.h"
#include "server/entity/character.h"
#include "server/entity/usermanager.h"
#include "server/entity/skillmanager.h"

void SkillHandler::handleSkillUsageStartRequest(GenericMessage* msg)
{
  const Character* c_char = NetworkHelper::getCharacter(msg);
  if (!c_char) return;

  Character* character = c_char->getLock();

  ptString name = character->getName();

  SkillUsageStartRequestMessage request_msg;
  request_msg.deserialise(msg->getByteStream());

  Server* server = Server::getServer();

  Skill* skill = server->getSkillManager()->findById(request_msg.getSkill());
  if (skill)
  {
    skill->castPrepare(character, request_msg.getTarget());
  }
  character->freeLock();
}

void SkillHandler::handleSkillUsageStopRequest(GenericMessage* msg)
{
  const Character* c_char = NetworkHelper::getCharacter(msg);
  if (!c_char) return;

  Character* character = c_char->getLock();

  ptString name = character->getName();

  SkillUsageStopRequestMessage request_msg;
  request_msg.deserialise(msg->getByteStream());

  Server* server = Server::getServer();

  Skill* skill = server->getSkillManager()->findById(request_msg.getSkill());
  skill->castInterrupt(character->getSkills()->findSkill(skill->getId()));

  character->freeLock();
}
