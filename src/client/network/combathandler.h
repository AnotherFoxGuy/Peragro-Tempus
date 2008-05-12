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

//  !! This file was automatically generated by a network code generator   !!
//  !! Do not change this file since all changes will be overwritten later !!
//  !! Instead please change the source files here: peragro/data/generate  !!

#ifndef COMBATHANDLER_H
#define COMBATHANDLER_H

#include "common/network/nwtypes.h"

#include "common/network/combatmessages.h"

class CombatHandler : public MessageHandler
{
public:
  CombatHandler()
  {
  }

  char getType() { return MESSAGES::COMBAT; }

  void handle(GenericMessage* msg)
  {
    char type = msg->getMsgType();
    if (type != MESSAGES::COMBAT) assert("wrong message type");
    char id = msg->getMsgId();

    if (id == COMBAT::ATTACKNOTIFICATION) handleAttackNotification(msg);
    else if (id == COMBAT::UPDATEARMOR) handleUpdateArmor(msg);
    else if (id == COMBAT::UPDATEWEAPON) handleUpdateWeapon(msg);
    else if (id == COMBAT::ATTACKRESULT) handleAttackResult(msg);
  }

  void handleAttackNotification(GenericMessage* msg);
  void handleUpdateArmor(GenericMessage* msg);
  void handleUpdateWeapon(GenericMessage* msg);
  void handleAttackResult(GenericMessage* msg);
};

#endif // COMBATHANDLER_H
