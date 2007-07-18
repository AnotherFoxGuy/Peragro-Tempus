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

#ifndef _SKILLHANDLER_H_
#define _SKILLHANDLER_H_

#include "common/network/nwtypes.h"

#include "common/network/skillmessages.h"

class Client;

class SkillHandler : public MessageHandler
{
public:
  SkillHandler()
  {
  }

  char getType() { return MESSAGES::SKILL; }

  void handle(GenericMessage* msg)
  {
    char type = msg->getMsgType();
    if (type != MESSAGES::SKILL) assert("wrong message type");
    char id = msg->getMsgId();

    if (id == SKILL::SKILLUSAGESTARTRESPONSE) handleSkillUsageStartResponse(msg);
    else if (id == SKILL::SKILLUSAGECOMPLETION) handleSkillUsageCompletion(msg);
    else if (id == SKILL::SKILLUSAGEINTERRUPT) handleSkillUsageInterrupt(msg);
  }

  void handleSkillUsageStartResponse(GenericMessage* msg);
  void handleSkillUsageCompletion(GenericMessage* msg);
  void handleSkillUsageInterrupt(GenericMessage* msg);
};

#endif // _SKILLHANDLER_H_
