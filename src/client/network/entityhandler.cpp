/*
    Copyright (C) 2008 Development Team of Peragro Tempus

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

#include "client/network/entityhandler.h"

#include "common/event/eventmanager.h"
#include "common/event/entityevent.h"

#include "client/pointer/pointer.h"

void EntityHandler::handleAddNpcEntity(GenericMessage* msg)
{
  AddNpcEntityMessage pmsg;
  pmsg.deserialise(msg->getByteStream());

  using namespace PT::Events;
  EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
  {
    csRef<iEvent> pEvent = evmgr->CreateEvent("entity.add.npc", true);
    pEvent->Add("entityName", *pmsg.getEntityName()?*pmsg.getEntityName():"");
    pEvent->Add("meshName", *pmsg.getMeshName()?*pmsg.getMeshName():"");
    pEvent->Add("fileName", *pmsg.getFileName()?*pmsg.getFileName():"");
    PT::Events::EntityHelper::SetVector3(pEvent, "position", pmsg.getPosition());
    pEvent->Add("rotation", pmsg.getRotation());
    pEvent->Add("entityId", pmsg.getEntityId());
    csRef<iEvent> equipmentList = evmgr->CreateEvent("equipmentList", true);
    for (unsigned char i = 0; i < pmsg.getEquipmentCount(); i++)
    {
      std::stringstream itemName;
      itemName << "equipment" << "_" << i;
      csRef<iEvent> item = evmgr->CreateEvent(itemName.str().c_str(), true);
      item->Add("slotId", pmsg.getSlotId(i));
      item->Add("itemId", pmsg.getItemId(i));
      item->Add("meshName", *pmsg.getMeshName(i)?*pmsg.getMeshName(i):"");
      item->Add("fileName", *pmsg.getFileName(i)?*pmsg.getFileName(i):"");
      equipmentList->Add(itemName.str().c_str(), item);
    }
    pEvent->Add("equipmentList", equipmentList);

    evmgr->AddEvent(pEvent);
  }

  {
    csRef<iEvent> pEvent = evmgr->CreateEvent(EntityHelper::MakeEntitySpecific("entity.pose", pmsg.getEntityId()), true);
    pEvent->Add("entityId", pmsg.getEntityId());
    pEvent->Add("poseId", pmsg.getPoseId());

    evmgr->AddEvent(pEvent);
  }

} // end handleAddNpcEntity

void EntityHandler::handleAddItemEntity(GenericMessage* msg)
{
  AddItemEntityMessage pmsg;
  pmsg.deserialise(msg->getByteStream());

  using namespace PT::Events;
  EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
  {
    csRef<iEvent> pEvent = evmgr->CreateEvent("entity.add.item", true);
    pEvent->Add("entityName", *pmsg.getEntityName()?*pmsg.getEntityName():"");
    pEvent->Add("meshName", *pmsg.getMeshName()?*pmsg.getMeshName():"");
    pEvent->Add("fileName", *pmsg.getFileName()?*pmsg.getFileName():"");
    PT::Events::EntityHelper::SetVector3(pEvent, "position", pmsg.getPosition());
    pEvent->Add("rotation", pmsg.getRotation());
    pEvent->Add("entityId", pmsg.getEntityId());

    evmgr->AddEvent(pEvent);
  }

} // end handleAddItemEntity

void EntityHandler::handleAddDoorEntity(GenericMessage* msg)
{
  AddDoorEntityMessage pmsg;
  pmsg.deserialise(msg->getByteStream());

  using namespace PT::Events;
  EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
  {
    csRef<iEvent> pEvent = evmgr->CreateEvent("entity.add.door", true);
    pEvent->Add("isOpen", pmsg.getIsOpen());
    pEvent->Add("isLocked", pmsg.getIsLocked());
    pEvent->Add("entityId", pmsg.getEntityId());
    pEvent->Add("entityName", *pmsg.getEntityName()?*pmsg.getEntityName():"");
    pEvent->Add("meshId", pmsg.getMeshId());
    pEvent->Add("meshName", *pmsg.getMeshName()?*pmsg.getMeshName():"");
    pEvent->Add("fileName", *pmsg.getFileName()?*pmsg.getFileName():"");
    pEvent->Add("animationName", *pmsg.getAnimationName()?*pmsg.getAnimationName():"");

    evmgr->AddEvent(pEvent);
  }

} // end handleAddDoorEntity

void EntityHandler::handleAddPlayerEntity(GenericMessage* msg)
{
  AddPlayerEntityMessage pmsg;
  pmsg.deserialise(msg->getByteStream());

  using namespace PT::Events;
  EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
  {
    csRef<iEvent> pEvent = evmgr->CreateEvent("entity.add.pc", true);
    pEvent->Add("entityName", *pmsg.getEntityName()?*pmsg.getEntityName():"");
    pEvent->Add("meshName", *pmsg.getMeshName()?*pmsg.getMeshName():"");
    pEvent->Add("fileName", *pmsg.getFileName()?*pmsg.getFileName():"");
    PT::Events::EntityHelper::SetVector3(pEvent, "position", pmsg.getPosition());
    pEvent->Add("rotation", pmsg.getRotation());
    pEvent->Add("hairColour", *pmsg.getHairColour());
    pEvent->Add("skinColour", *pmsg.getSkinColour());
    pEvent->Add("decalColour", *pmsg.getDecalColour());
    pEvent->Add("entityId", pmsg.getEntityId());
    csRef<iEvent> equipmentList = evmgr->CreateEvent("equipmentList", true);
    for (unsigned char i = 0; i < pmsg.getEquipmentCount(); i++)
    {
      std::stringstream itemName;
      itemName << "equipment" << "_" << i;
      csRef<iEvent> item = evmgr->CreateEvent(itemName.str().c_str(), true);
      item->Add("slotId", pmsg.getSlotId(i));
      item->Add("itemId", pmsg.getItemId(i));
      item->Add("meshName", *pmsg.getMeshName(i)?*pmsg.getMeshName(i):"");
      item->Add("fileName", *pmsg.getFileName(i)?*pmsg.getFileName(i):"");
      equipmentList->Add(itemName.str().c_str(), item);
    }
    pEvent->Add("equipmentList", equipmentList);

    evmgr->AddEvent(pEvent);
  }

  {
    csRef<iEvent> pEvent = evmgr->CreateEvent(EntityHelper::MakeEntitySpecific("entity.pose", pmsg.getEntityId()), true);
    pEvent->Add("entityId", pmsg.getEntityId());
    pEvent->Add("poseId", pmsg.getPoseId());

    evmgr->AddEvent(pEvent);
  }

} // end handleAddPlayerEntity

void EntityHandler::handleRemoveEntity(GenericMessage* msg)
{
  RemoveEntityMessage pmsg;
  pmsg.deserialise(msg->getByteStream());

  using namespace PT::Events;
  EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
  {
    csRef<iEvent> pEvent = evmgr->CreateEvent("entity.remove", true);
    pEvent->Add("entityId", pmsg.getEntityId());

    evmgr->AddEvent(pEvent);
  }

} // end handleRemoveEntity

void EntityHandler::handleMove(GenericMessage* msg)
{
  MoveMessage pmsg;
  pmsg.deserialise(msg->getByteStream());

  using namespace PT::Events;
  EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
  {
    csRef<iEvent> pEvent = evmgr->CreateEvent(EntityHelper::MakeEntitySpecific("entity.move", pmsg.getEntityId()), true);
    PT::Events::EntityHelper::SetVector3(pEvent, "moveDirection", pmsg.getMoveDirection());
    pEvent->Add("entityId", pmsg.getEntityId());
    pEvent->Add("run", pmsg.getRun());
    pEvent->Add("jump", pmsg.getJump());

    evmgr->AddEvent(pEvent);
  }

} // end handleMove

void EntityHandler::handlePickResponse(GenericMessage* msg)
{
  PickResponseMessage pmsg;
  pmsg.deserialise(msg->getByteStream());

  using namespace PT::Events;
  EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
  {
    csRef<iEvent> pEvent = evmgr->CreateEvent("trade.inventory.pickup", true);
    pEvent->Add("itemEntityId", pmsg.getItemEntityId());
    pEvent->Add("slotId", pmsg.getSlotId());
    pEvent->Add("name", *pmsg.getName()?*pmsg.getName():"");
    pEvent->Add("iconName", *pmsg.getIconName()?*pmsg.getIconName():"");
    pEvent->Add("description", *pmsg.getDescription()?*pmsg.getDescription():"");
    pEvent->Add("weight", pmsg.getWeight());
    pEvent->Add("equipType", *pmsg.getEquipType()?*pmsg.getEquipType():"");
    pEvent->Add("error", *pmsg.getError()?*pmsg.getError():"");

    evmgr->AddEvent(pEvent);
  }

} // end handlePickResponse

void EntityHandler::handleDropResponse(GenericMessage* msg)
{
  DropResponseMessage pmsg;
  pmsg.deserialise(msg->getByteStream());

  using namespace PT::Events;
  EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
  {
    csRef<iEvent> pEvent = evmgr->CreateEvent("trade.inventory.drop", true);
    pEvent->Add("slotId", pmsg.getSlotId());
    pEvent->Add("error", *pmsg.getError()?*pmsg.getError():"");

    evmgr->AddEvent(pEvent);
  }

} // end handleDropResponse

void EntityHandler::handleEquip(GenericMessage* msg)
{
  EquipMessage pmsg;
  pmsg.deserialise(msg->getByteStream());

  using namespace PT::Events;
  EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
  {
    csRef<iEvent> pEvent = evmgr->CreateEvent(EntityHelper::MakeEntitySpecific("entity.equip", pmsg.getEntityId()), true);
    pEvent->Add("entityId", pmsg.getEntityId());
    pEvent->Add("slotId", pmsg.getSlotId());
    pEvent->Add("meshName", *pmsg.getMeshName()?*pmsg.getMeshName():"");
    pEvent->Add("fileName", *pmsg.getFileName()?*pmsg.getFileName():"");

    evmgr->AddEvent(pEvent);
  }

} // end handleEquip

void EntityHandler::handleMoveTo(GenericMessage* msg)
{
  MoveToMessage pmsg;
  pmsg.deserialise(msg->getByteStream());

  using namespace PT::Events;
  EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
  {
    csRef<iEvent> pEvent = evmgr->CreateEvent(EntityHelper::MakeEntitySpecific("entity.moveto", pmsg.getEntityId()), true);
    PT::Events::EntityHelper::SetVector3(pEvent, "from", pmsg.getFrom());
    PT::Events::EntityHelper::SetVector3(pEvent, "to", pmsg.getTo());
    pEvent->Add("speed", pmsg.getSpeed());
    pEvent->Add("entityId", pmsg.getEntityId());
    pEvent->Add("run", pmsg.getRun());
    pEvent->Add("turn", pmsg.getTurn());
    pEvent->Add("jump", pmsg.getJump());

    evmgr->AddEvent(pEvent);
  }

} // end handleMoveTo

void EntityHandler::handleTeleportResponse(GenericMessage* msg)
{
  TeleportResponseMessage pmsg;
  pmsg.deserialise(msg->getByteStream());

  using namespace PT::Events;
  EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
  {
    csRef<iEvent> pEvent = evmgr->CreateEvent(EntityHelper::MakeEntitySpecific("entity.teleport", pmsg.getEntityId()), true);
    pEvent->Add("entityId", pmsg.getEntityId());
    PT::Events::EntityHelper::SetVector3(pEvent, "position", pmsg.getPosition());
    pEvent->Add("rotation", pmsg.getRotation());

    evmgr->AddEvent(pEvent);
  }

} // end handleTeleportResponse

void EntityHandler::handleDrUpdate(GenericMessage* msg)
{
  DrUpdateMessage pmsg;
  pmsg.deserialise(msg->getByteStream());

  using namespace PT::Events;
  EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
  {
    csRef<iEvent> pEvent = evmgr->CreateEvent(EntityHelper::MakeEntitySpecific("entity.drupdate", pmsg.getEntityId()), true);
    PT::Events::EntityHelper::SetVector3(pEvent, "position", pmsg.getPosition());
    pEvent->Add("rotation", pmsg.getRotation());
    pEvent->Add("entityId", pmsg.getEntityId());

    evmgr->AddEvent(pEvent);
  }

} // end handleDrUpdate

void EntityHandler::handleAddMountEntity(GenericMessage* msg)
{
  AddMountEntityMessage pmsg;
  pmsg.deserialise(msg->getByteStream());

  using namespace PT::Events;
  EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
  {
    csRef<iEvent> pEvent = evmgr->CreateEvent("entity.add.mount", true);
    pEvent->Add("entityName", *pmsg.getEntityName()?*pmsg.getEntityName():"");
    pEvent->Add("meshName", *pmsg.getMeshName()?*pmsg.getMeshName():"");
    pEvent->Add("fileName", *pmsg.getFileName()?*pmsg.getFileName():"");
    PT::Events::EntityHelper::SetVector3(pEvent, "position", pmsg.getPosition());
    pEvent->Add("rotation", pmsg.getRotation());
    pEvent->Add("entityId", pmsg.getEntityId());
    pEvent->Add("inventoryId", pmsg.getInventoryId());

    evmgr->AddEvent(pEvent);
  }

} // end handleAddMountEntity

void EntityHandler::handleMount(GenericMessage* msg)
{
  MountMessage pmsg;
  pmsg.deserialise(msg->getByteStream());

  using namespace PT::Events;
  EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
  {
    csRef<iEvent> pEvent = evmgr->CreateEvent("entity.mount", true);
    pEvent->Add("playerEntityId", pmsg.getPlayerEntityId());
    pEvent->Add("mountEntityId", pmsg.getMountEntityId());
    pEvent->Add("canControl", pmsg.getCanControl());

    evmgr->AddEvent(pEvent);
  }

} // end handleMount

void EntityHandler::handleUnmount(GenericMessage* msg)
{
  UnmountMessage pmsg;
  pmsg.deserialise(msg->getByteStream());

  using namespace PT::Events;
  EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
  {
    csRef<iEvent> pEvent = evmgr->CreateEvent("entity.unmount", true);
    pEvent->Add("playerEntityId", pmsg.getPlayerEntityId());
    pEvent->Add("mountEntityId", pmsg.getMountEntityId());

    evmgr->AddEvent(pEvent);
  }

} // end handleUnmount

void EntityHandler::handlePose(GenericMessage* msg)
{
  PoseMessage pmsg;
  pmsg.deserialise(msg->getByteStream());

  using namespace PT::Events;
  EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
  {
    csRef<iEvent> pEvent = evmgr->CreateEvent(EntityHelper::MakeEntitySpecific("entity.pose", pmsg.getEntityId()), true);
    pEvent->Add("entityId", pmsg.getEntityId());
    pEvent->Add("poseId", pmsg.getPoseId());

    evmgr->AddEvent(pEvent);
  }

} // end handlePose

