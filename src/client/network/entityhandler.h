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

#include "common/network/nwtypes.h"

#include "common/network/entitymessages.h"

class Client;

class EntityHandler : public MessageHandler
{
public:
  EntityHandler()
  {
  }

  char getType() { return MESSAGES::ENTITY; }

  void handle(GenericMessage* msg)
  {
    char type = msg->getMsgType();
    if (type != MESSAGES::ENTITY) assert("wrong message type");
    char id = msg->getMsgId();

    if (id == ENTITY::ADDPLAYERENTITY) handleAddPlayerEntity(msg);
    else if (id == ENTITY::ADDITEMENTITY) handleAddItemEntity(msg);
    else if (id == ENTITY::ADDNPCENTITY) handleAddNpcEntity(msg);
    else if (id == ENTITY::ADDDOORENTITY) handleAddDoorEntity(msg);
    else if (id == ENTITY::REMOVE) handleRemove(msg);
    else if (id == ENTITY::MOVE) handleMove(msg);
    else if (id == ENTITY::MOVETO) handleMoveTo(msg);
    else if (id == ENTITY::PICKRESPONSE) handlePickResponse(msg);
    else if (id == ENTITY::DROPRESPONSE) handleDropResponse(msg);
    else if (id == ENTITY::INVENTORYLIST) handleInventoryList(msg);
    else if (id == ENTITY::STATSLIST) handleStatsList(msg);
    else if (id == ENTITY::STATSCHANGE) handleStatsChange(msg);
    else if (id == ENTITY::SKILLSLIST) handleSkillsList(msg);
    else if (id == ENTITY::EQUIP) handleEquip(msg);
    else if (id == ENTITY::TELEPORT) handleTeleport(msg);
    else if (id == ENTITY::DRUPDATE) handleDrUpdate(msg);
    else if (id == ENTITY::INVENTORYMOVEITEM) handleInventoryMoveItem(msg);
    else if (id == ENTITY::ADDMOUNTENTITY) handleAddMountEntity(msg);
    else if (id == ENTITY::MOUNT) handleMount(msg);
    else if (id == ENTITY::UNMOUNT) handleUnmount(msg);
    else if (id == ENTITY::POSE) handlePose(msg);
  }

  void handleAddPlayerEntity(GenericMessage* msg);
  void handleAddItemEntity(GenericMessage* msg);
  void handleAddNpcEntity(GenericMessage* msg);
  void handleAddDoorEntity(GenericMessage* msg);
  void handleRemove(GenericMessage* msg);
  void handleMove(GenericMessage* msg);
  void handleMoveTo(GenericMessage* msg);
  void handlePickResponse(GenericMessage* msg);
  void handleDropResponse(GenericMessage* msg);
  void handleInventoryList(GenericMessage* msg);
  void handleStatsList(GenericMessage* msg);
  void handleStatsChange(GenericMessage* msg);
  void handleSkillsList(GenericMessage* msg);
  void handleEquip(GenericMessage* msg);
  void handleTeleport(GenericMessage* msg);
  void handleDrUpdate(GenericMessage* msg);
  void handleInventoryMoveItem(GenericMessage* msg);
  void handleAddMountEntity(GenericMessage* msg);
  void handleMount(GenericMessage* msg);
  void handleUnmount(GenericMessage* msg);
  void handlePose(GenericMessage* msg);
};

#endif // _ENTITYHANDLER_H_
