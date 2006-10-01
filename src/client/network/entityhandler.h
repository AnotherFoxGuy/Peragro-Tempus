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

#include "client/network/doorhandler.h"
#include "client/network/questhandler.h"
#include "client/network/skillhandler.h"
#include "client/network/tradehandler.h"

#include "client/entity/ptentitymanager.h"

class EntityHandler : public MessageHandler
{
private:
  Network* network;
  Client* client;
  GUIManager* guimanager;

  DoorHandler door_handler;
  QuestHandler quest_handler;
  SkillHandler skill_handler;
  TradeHandler trade_handler;

public:
  EntityHandler(Network* network, Client* client) 
  : network(network), client(client), door_handler(client),
    quest_handler(client), skill_handler(client), trade_handler(client)
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
      else if (id == ENTITY::EQUIP) handleEquip(msg);
      else if (id == ENTITY::INIT_DOOR) handleAddDoor(msg);
      else if (id == ENTITY::TELEPORT) handleTeleport(msg);
      else if (id == ENTITY::ADDCHARACTERENTITY) handleAddCharacterEntity(msg);
    }
    else if (type == MESSAGES::DOOR)
    {
      door_handler.handle(msg);
    }
    else if (type == MESSAGES::QUEST)
    {
      quest_handler.handle(msg);
    }
    else if (type == MESSAGES::TRADE)
    {
      trade_handler.handle(msg);
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
  void handleEquip(GenericMessage* msg);
  void handleAddDoor(GenericMessage* msg);
  void handleTeleport(GenericMessage* msg);
  void handleAddCharacterEntity(GenericMessage* msg);
};

#endif // _ENTITYHANDLER_H_
