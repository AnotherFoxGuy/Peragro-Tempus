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
#include "client/gui/gui.h"
#include "client/gui/guimanager.h"
#include <propclass/prop.h>
#include <physicallayer/propclas.h>

#include "client/entity/entitymanager.h"

#include "common/event/eventmanager.h"
#include "common/event/entityevent.h"

#include "common/reporter/reporter.h"

void EntityHandler::handleAddNpcEntity(GenericMessage* msg)
{
  Report(PT::Debug, "EntityHandler: Received AddNpcEntity.");
  AddNpcEntityMessage entmsg;
  entmsg.deserialise(msg->getByteStream());

  PT::Events::EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
  csRef<iEvent> entityEvent = evmgr->CreateEvent("entity.add", true);

  entityEvent->Add("entityName", *entmsg.getName());
  entityEvent->Add("meshName", *entmsg.getMesh());
  PT::Events::EntityHelper::SetPosition(entityEvent, entmsg.getPos());
  entityEvent->Add("rotation", entmsg.getRotation());
  entityEvent->Add("sectorId", entmsg.getSectorId());
  entityEvent->Add("entityId", entmsg.getEntityId());
  entityEvent->Add("entityType", PT::Common::Entity::NPCEntityType);

  csRef<iEvent> list = evmgr->CreateEvent("equipmentList", true);
  for (unsigned char i = 0; i < entmsg.getEquipmentCount(); i++)
  {
    std::stringstream itemName;
    itemName << "equipment" << "_" << i;
    csRef<iEvent> item = evmgr->CreateEvent(itemName.str().c_str(), true);
    item->Add("itemId", entmsg.getItemId(i));
    item->Add("slotId", entmsg.getSlotId(i));
    item->Add("variation", entmsg.getVariation(i));
    item->Add("fileName", *entmsg.getFile(i));
    item->Add("meshName", *entmsg.getMesh(i));
    list->Add(itemName.str().c_str(), item);
  }
  entityEvent->Add("equipmentList", list);

  evmgr->AddEvent(entityEvent);
}

void EntityHandler::handleAddDoorEntity(GenericMessage* msg)
{
  Report(PT::Debug, "EntityHandler: Received AddDoor.");
  AddDoorEntityMessage entmsg;
  entmsg.deserialise(msg->getByteStream());

  PT::Events::EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
  csRef<iEvent> entityEvent = evmgr->CreateEvent("entity.add", true);

  entityEvent->Add("entityName", *entmsg.getName());
  entityEvent->Add("meshName", *entmsg.getMesh());
  entityEvent->Add("fileName", "none");
  // Door has no position.
  PT::Events::EntityHelper::SetPosition(entityEvent, PtVector3(0.0f));
  entityEvent->Add("sectorId", entmsg.getSectorId());
  entityEvent->Add("entityId", entmsg.getEntityId());
  entityEvent->Add("entityType", PT::Common::Entity::DoorEntityType);

  entityEvent->Add("typeId", entmsg.getDoorId());
  entityEvent->Add("locked", entmsg.getIsLocked());
  entityEvent->Add("open", entmsg.getIsOpen());
  entityEvent->Add("animationName", *entmsg.getAnimation());

  evmgr->AddEvent(entityEvent);
}
void EntityHandler::handleRemoveEntity(GenericMessage* msg)
{
  Report(PT::Debug, "EntityHandler: Received RemoveEntity.");
  RemoveEntityMessage entmsg;
  entmsg.deserialise(msg->getByteStream());

  PT::Events::EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
  csRef<iEvent> entityEvent = evmgr->CreateEvent("entity.remove", true);

  entityEvent->Add("entityId", entmsg.getEntityId());

  evmgr->AddEvent(entityEvent);
}

void EntityHandler::handleMove(GenericMessage* msg)
{
  MoveMessage entmsg;
  entmsg.deserialise(msg->getByteStream());

  using namespace PT::Events;

  PT::Events::EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
  csRef<iEvent> entityEvent = evmgr->CreateEvent(EntityHelper::MakeEntitySpecific("entity.move", entmsg.getEntityId()), true);

  entityEvent->Add("entityId", entmsg.getEntityId());
  entityEvent->Add("walkDirection", entmsg.getWalk());
  entityEvent->Add("turnDirection", entmsg.getTurn());
  entityEvent->Add("run", entmsg.getRun());
  entityEvent->Add("jump", entmsg.getJump());
  entityEvent->Add("local", false);

  evmgr->AddEvent(entityEvent);
}

void EntityHandler::handlePickResponse(GenericMessage* msg)
{
  PickResponseMessage response_msg;
  response_msg.deserialise(msg->getByteStream());

  if (response_msg.getError().isNull())
  {
    PT::Events::EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
    csRef<iEvent> entityEvent = evmgr->CreateEvent("trade.pickup", true);

    entityEvent->Add("itemId", response_msg.getItemId());
    entityEvent->Add("variationId", response_msg.getVariation());
    entityEvent->Add("slotId", response_msg.getSlotId());
    entityEvent->Add("name", *response_msg.getName());
    entityEvent->Add("icon", *response_msg.getIcon());

    evmgr->AddEvent(entityEvent);
  }
  else
    Report(PT::Notify, "You can't pick Item %d! Reason: '%s'.", response_msg.getItemId(), *response_msg.getError());
}

void EntityHandler::handleDropResponse(GenericMessage* msg)
{
  DropResponseMessage response_msg;
  response_msg.deserialise(msg->getByteStream());

  if (response_msg.getError().isNull())
  {
    PT::Events::EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
    csRef<iEvent> entityEvent = evmgr->CreateEvent("trade.drop", true);

    entityEvent->Add("slotId", response_msg.getSlotId());

    evmgr->AddEvent(entityEvent);
  }
  else
    Report(PT::Notify, "You can't drop slot %d! Reason: '%s'.", response_msg.getSlotId(), *response_msg.getError());
}

void EntityHandler::handleDrUpdate(GenericMessage* msg)
{
  DrUpdateMessage dr_msg;
  dr_msg.deserialise(msg->getByteStream());

  using namespace PT::Events;

  PT::Events::EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
  csRef<iEvent> entityEvent = evmgr->CreateEvent(EntityHelper::MakeEntitySpecific("entity.drupdate", dr_msg.getEntityId()), true);

  entityEvent->Add("entityId", dr_msg.getEntityId());
  PT::Events::EntityHelper::SetPosition(entityEvent, dr_msg.getPos());
  entityEvent->Add("rotation", dr_msg.getRotation());
  entityEvent->Add("sectorId", dr_msg.getSectorId());

  evmgr->AddEvent(entityEvent);
}

void EntityHandler::handleMoveTo(GenericMessage* msg)
{
  //Report(PT::Debug, "EntityHandler: Received MoveToEntity.");
  MoveToMessage move_msg;
  move_msg.deserialise(msg->getByteStream());

  using namespace PT::Events;

  PtVector3 fv1 = move_msg.getFrom();
  PtVector3 fv2 = move_msg.getTo();

  PT::Events::EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
  csRef<iEvent> entityEvent = evmgr->CreateEvent(EntityHelper::MakeEntitySpecific("entity.moveto", move_msg.getEntityId()), true);

  entityEvent->Add("entityId", move_msg.getEntityId());
  PT::Events::EntityHelper::SetVector3(entityEvent, "origin", fv1);
  PT::Events::EntityHelper::SetVector3(entityEvent, "destination", fv2);
  entityEvent->Add("speed", move_msg.getSpeed());

  evmgr->AddEvent(entityEvent);
}

void EntityHandler::handleEquip(GenericMessage* msg)
{
  EquipMessage equip_msg;
  equip_msg.deserialise(msg->getByteStream());

  using namespace PT::Events;

  PT::Events::EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
  csRef<iEvent> entityEvent = evmgr->CreateEvent(EntityHelper::MakeEntitySpecific("entity.equip", equip_msg.getEntityId()), true);

  entityEvent->Add("entityId", equip_msg.getEntityId());
  entityEvent->Add("slotId", equip_msg.getSlotId());
  entityEvent->Add("itemId", equip_msg.getItemId());
  entityEvent->Add("mesh", *equip_msg.getMesh()?*equip_msg.getMesh():"NULL");
  entityEvent->Add("file", *equip_msg.getFile()?*equip_msg.getFile():"NULL");

  evmgr->AddEvent(entityEvent);
}

void EntityHandler::handleTeleportResponse(GenericMessage* msg)
{
  TeleportResponseMessage telemsg;
  telemsg.deserialise(msg->getByteStream());

  using namespace PT::Events;

  PT::Events::EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
  csRef<iEvent> entityEvent = evmgr->CreateEvent(EntityHelper::MakeEntitySpecific("entity.teleport", telemsg.getEntityId()), true);

  entityEvent->Add("entityId", telemsg.getEntityId());
  PT::Events::EntityHelper::SetPosition(entityEvent, telemsg.getPos());
  entityEvent->Add("rotation", telemsg.getRotation());
  entityEvent->Add("sectorId", telemsg.getSectorId());

  evmgr->AddEvent(entityEvent);
}

void EntityHandler::handleAddPlayerEntity(GenericMessage* msg)
{
  Report(PT::Debug, "EntityHandler: Received AddEntity.");
  AddPlayerEntityMessage entmsg;
  entmsg.deserialise(msg->getByteStream());

  PT::Events::EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
  csRef<iEvent> entityEvent = evmgr->CreateEvent("entity.add", true);

  entityEvent->Add("entityName", *entmsg.getName());
  entityEvent->Add("meshName", *entmsg.getMesh());
  PT::Events::EntityHelper::SetPosition(entityEvent, entmsg.getPos());
  entityEvent->Add("rotation", entmsg.getRotation());
  entityEvent->Add("sectorId", entmsg.getSectorId());
  entityEvent->Add("entityId", entmsg.getEntityId());
  entityEvent->Add("entityType", PT::Common::Entity::PCEntityType);

  

  csRef<iEvent> list = evmgr->CreateEvent("equipmentList", true);
  for (unsigned char i = 0; i < entmsg.getEquipmentCount(); i++)
  {
    std::stringstream itemName;
    itemName << "equipment" << "_" << i;
    csRef<iEvent> item = evmgr->CreateEvent(itemName.str().c_str(), true);
    item->Add("itemId", entmsg.getItemId(i));
    item->Add("slotId", entmsg.getSlotId(i));
    item->Add("variation", entmsg.getVariation(i));
    item->Add("fileName", *entmsg.getFile(i));
    item->Add("meshName", *entmsg.getMesh(i));
    list->Add(itemName.str().c_str(), item);
  }
  entityEvent->Add("equipmentList", list);

  evmgr->AddEvent(entityEvent);

  // Pose event.
  csRef<iEvent> poseEvent = evmgr->CreateEvent("entity.pose", true);

  poseEvent->Add("entityId", entmsg.getEntityId());
  poseEvent->Add("poseId", entmsg.getPoseId());

  evmgr->AddEvent(poseEvent);
}

void EntityHandler::handleAddItemEntity(GenericMessage* msg)
{
  Report(PT::Debug, "EntityHandler: Received AddItemEntity.");

  AddItemEntityMessage entmsg;
  entmsg.deserialise(msg->getByteStream());

  PT::Events::EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
  csRef<iEvent> entityEvent = evmgr->CreateEvent("entity.add", true);

  entityEvent->Add("typeId", entmsg.getItemId());
  entityEvent->Add("entityName", *entmsg.getName());
  entityEvent->Add("meshName", *entmsg.getMesh());
  entityEvent->Add("fileName", *entmsg.getFile());
  PT::Events::EntityHelper::SetPosition(entityEvent, entmsg.getPos());
  entityEvent->Add("rotation", entmsg.getRotation());
  entityEvent->Add("sectorId", entmsg.getSectorId());
  entityEvent->Add("entityId", entmsg.getEntityId());
  entityEvent->Add("entityType", PT::Common::Entity::ItemEntityType);

  evmgr->AddEvent(entityEvent);
}

void EntityHandler::handleAddMountEntity(GenericMessage* msg)
{
  Report(PT::Debug, "EntityHandler: Received AddMountEntity.");
  AddMountEntityMessage entmsg;
  entmsg.deserialise(msg->getByteStream());

  PT::Events::EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
  csRef<iEvent> entityEvent = evmgr->CreateEvent("entity.add", true);

  entityEvent->Add("entityName", *entmsg.getName());
  entityEvent->Add("meshName", *entmsg.getMesh());
  PT::Events::EntityHelper::SetPosition(entityEvent, entmsg.getPos());
  entityEvent->Add("rotation", entmsg.getRotation());
  entityEvent->Add("sectorId", entmsg.getSectorId());
  entityEvent->Add("entityId", entmsg.getEntityId());
  entityEvent->Add("entityType", PT::Common::Entity::MountEntityType);

  evmgr->AddEvent(entityEvent);
}

void EntityHandler::handleMount(GenericMessage* msg)
{
  MountMessage mount_msg;
  mount_msg.deserialise(msg->getByteStream());

  PT::Events::EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
  csRef<iEvent> entityEvent = evmgr->CreateEvent("entity.mount", true);

  entityEvent->Add("entityId", mount_msg.getPlayerEntityId());
  entityEvent->Add("mountId", mount_msg.getMountEntityId());
  entityEvent->Add("control", mount_msg.getCanControl());
  entityEvent->Add("mount", true);

  evmgr->AddEvent(entityEvent);
}

void EntityHandler::handleUnmount(GenericMessage* msg)
{
  UnmountMessage mount_msg;
  mount_msg.deserialise(msg->getByteStream());

  PT::Events::EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
  csRef<iEvent> entityEvent = evmgr->CreateEvent("entity.mount", true);

  entityEvent->Add("entityId", mount_msg.getPlayerEntityId());
  entityEvent->Add("mountId", mount_msg.getMountEntityId());
  entityEvent->Add("control", false);
  entityEvent->Add("mount", false);

  evmgr->AddEvent(entityEvent);
}

void EntityHandler::handlePose(GenericMessage* msg)
{
  PoseMessage poseMsg;
  poseMsg.deserialise(msg->getByteStream());

  using namespace PT::Events;

  PT::Events::EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
  csRef<iEvent> poseEvent = evmgr->CreateEvent(EntityHelper::MakeEntitySpecific("entity.pose", poseMsg.getEntityId()), true);

  poseEvent->Add("entityId", poseMsg.getEntityId());
  poseEvent->Add("poseId", poseMsg.getPoseId());

  evmgr->AddEvent(poseEvent);
}
