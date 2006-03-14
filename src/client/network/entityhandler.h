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

#ifndef _ENTITYHANDLER_H_
#define _ENTITYHANDLER_H_

#include "stdio.h"

#include "common/network/messagehandler.h"
#include "common/network/entitymessages.h"

#include "common/entity/entitymanager.h"

#include "client/network/skillhandler.h"

class EntityHandler : public MessageHandler
{
private:
  Network* network;
  Client* client;
  GUIManager* guimanager;

  SkillHandler skill_handler;

public:
  EntityHandler(Network* network, Client* client) 
  : network(network), client(client), skill_handler(client)
  {
  }

  void handle(GenericMessage* msg)
  {
    char type = msg->getMsgType();
    if (type == MESSAGES::ENTITY)
    {
      char id = msg->getMsgId();

      if (id == 0) handleAddEntity(msg);
      else if (id == ENTITY::REMOVE) handleRemoveEntity(msg);
      else if (id == ENTITY::MOVE) handleMoveEntity(msg);
      else if (id == ENTITY::DRUPDATE) handleDrUpdate(msg);
      else if (id == ENTITY::PICK_RESPONSE) handlePickEntity(msg);
      else if (id == ENTITY::DROP_RESPONSE) handleDropEntity(msg);
      else if (id == ENTITY::INV_ITEM_LIST) handleInventoryItemList(msg);
      else if (id == ENTITY::CHAR_STAT_LIST) handleCharacterStatList(msg);
      else if (id == ENTITY::MOVE_TO) handleMoveEntityTo(msg);
      else if (id == ENTITY::CHAR_SKILL_LIST) handleCharacterSkillList(msg);
    }
    else
    {
      skill_handler.handle(msg);
    }
  }

  char getType()
  {
    return MESSAGES::ENTITY;
  }

  void handleAddEntity(GenericMessage* msg);
  void handleRemoveEntity(GenericMessage* msg);
  void handleMoveEntity(GenericMessage* msg);
  void handlePickEntity(GenericMessage* msg);
  void handleDropEntity(GenericMessage* msg);
  void handleDrUpdate(GenericMessage* msg);
  void handleInventoryItemList(GenericMessage* msg);
  void handleCharacterStatList(GenericMessage* msg);
  void handleMoveEntityTo(GenericMessage* msg);
  void handleCharacterSkillList(GenericMessage* msg);
};

#endif // _ENTITYHANDLER_H_
