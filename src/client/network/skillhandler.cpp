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

#include "client/network/network.h"
#include "client/client.h"
#include "client/combat/combatmanager.h"

void SkillHandler::handleSkillUsageStartResponse(GenericMessage* msg)
{
  printf("SkillHandler: Recieved a StartSkill message.\n");
  SkillUsageStartResponseMessage skillmsg;
  skillmsg.deserialise(msg->getByteStream());

  client->getCombatmgr()->SkillUsageStart ( skillmsg.getCaster(), skillmsg.getTarget(), skillmsg.getSkill(), skillmsg.getError() );
}

void SkillHandler::handleSkillUsageCompletion(GenericMessage* msg)
{
  //TODO: handle the message
  printf("SkillHandler: Recieved a CompleteSkill message.\n");
  SkillUsageCompletionMessage skillmsg;
  skillmsg.deserialise(msg->getByteStream());

  client->getCombatmgr()->SkillUsageComplete ( skillmsg.getCaster(), skillmsg.getTarget(), skillmsg.getSkill());
}

void SkillHandler::handleSkillUsageInterrupt(GenericMessage* msg)
{
  //TODO: handle the message
  printf("SkillHandler: Recieved a InterruptSkill message.\n");
}
