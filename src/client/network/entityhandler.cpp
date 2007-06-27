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

#include "client/entity/ptentitymanager.h"

#include "client/event/eventmanager.h"
#include "client/event/entityevent.h"

void EntityHandler::handleAddNpcEntity(GenericMessage* msg)
{
  printf("EntityHandler: Received AddNpcEntity\n");
  AddNpcEntityMessage entmsg;
  entmsg.deserialise(msg->getByteStream());

  using namespace PT::Events;
  EntityAddEvent* entity = new EntityAddEvent();

  entity->entityName  = *entmsg.getName();
  entity->meshName    = *entmsg.getMesh();
  csVector3 pos(entmsg.getPos()[0], entmsg.getPos()[1], entmsg.getPos()[2]);
  entity->position    = pos;
  entity->sectorName  = *entmsg.getSector();
  entity->entityId    = entmsg.getEntityId();
  entity->entityType  = EntityEvent::NPCEntity; 

  PointerLibrary::getInstance()->getEventManager()->AddEvent(entity);
}

void EntityHandler::handleAddDoorEntity(GenericMessage* msg)
{
  printf("EntityHandler: Received AddDoor\n");
  AddDoorEntityMessage entmsg;
  entmsg.deserialise(msg->getByteStream());

  using namespace PT::Events;
  EntityAddEvent* entity = new EntityAddEvent();

  entity->entityName  = *entmsg.getName();
  entity->meshName    = *entmsg.getMesh();
  entity->locked      = (entmsg.getIsLocked() != 0);
  entity->open        = (entmsg.getIsOpen() != 0);
  entity->entityId    = entmsg.getEntityId();
  entity->entityType  = EntityEvent::DoorEntity; 

  PointerLibrary::getInstance()->getEventManager()->AddEvent(entity);
}
void EntityHandler::handleRemove(GenericMessage* msg)
{
  printf("EntityHandler: Received RemoveEntity\n");
  RemoveMessage entmsg;
  entmsg.deserialise(msg->getByteStream());

  using namespace PT::Events;
  EntityRemoveEvent* entity = new EntityRemoveEvent();

  entity->entityId = entmsg.getEntityId();

  PointerLibrary::getInstance()->getEventManager()->AddEvent(entity);
}

void EntityHandler::handleMove(GenericMessage* msg)
{
  printf("EntityHandler: Received MoveEntity\n");
  MoveMessage response_msg;
  response_msg.deserialise(msg->getByteStream());
  PointerLibrary::getInstance()->getEntityManager()->moveEntity(response_msg.getEntityId(), response_msg.getWalk(), response_msg.getTurn());
}

void EntityHandler::handlePickResponse(GenericMessage* msg)
{
  PickResponseMessage response_msg;
  response_msg.deserialise(msg->getByteStream());
  GUIManager* guimanager = PointerLibrary::getInstance()->getGUIManager();

  if (response_msg.getError().isNull())
  {
    //guimanager->GetInventoryWindow()->AddItem(response_msg.getItemId(), response_msg.getSlotId());
  }
  else
    printf("You can't pick Item %d! Reason: '%s'\n", response_msg.getItemId(), *response_msg.getError());
}

void EntityHandler::handleDropResponse(GenericMessage* msg)
{
  DropResponseMessage response_msg;
  response_msg.deserialise(msg->getByteStream());
  GUIManager* guimanager = PointerLibrary::getInstance()->getGUIManager();

  if (response_msg.getError().isNull())
  {
    //guimanager->GetInventoryWindow()->RemoveItem(response_msg.getSlotId());
  }
  else
    printf("You can't drop %d from slot %d! Reason: '%s'\n", response_msg.getItemId(), response_msg.getSlotId(), *response_msg.getError());
}

void EntityHandler::handleDrUpdate(GenericMessage* msg)
{
  DrUpdateMessage dr_msg;
  dr_msg.deserialise(msg->getByteStream());

  DrUpdateData* drupdate = new DrUpdateData();
  drupdate->rot = dr_msg.getRotation();
  drupdate->pos = csVector3(dr_msg.getPos()[0],dr_msg.getPos()[1],dr_msg.getPos()[2]);
  drupdate->sector = *dr_msg.getSector();
  drupdate->entity_id = dr_msg.getEntityId();

  PointerLibrary::getInstance()->getEntityManager()->DrUpdateEntity(drupdate);
}

void EntityHandler::handleInventoryList(GenericMessage* msg)
{
  InventoryListMessage item_msg;
  item_msg.deserialise(msg->getByteStream());
  printf("EntityHandler: Got %d items in the Inventory: \n---------------------------\n", item_msg.getInventoryCount());
  GUIManager* guimanager = PointerLibrary::getInstance()->getGUIManager();
  for (int i=0; i<item_msg.getInventoryCount(); i++)
  {
    printf("Item %d with amount 1 in slot %d\n", item_msg.getItemId(i), item_msg.getSlotId(i));
    //guimanager->GetInventoryWindow()->AddItem(item_msg.getItemId(i), item_msg.getSlotId(i));
  }
}

void EntityHandler::handleStatsList(GenericMessage* msg)
{
  StatsListMessage stat_msg;
  stat_msg.deserialise(msg->getByteStream());
  printf("EntityHandler: Got %d stats for the Character: \n---------------------------\n", stat_msg.getStatsCount());
  GUIManager* guimanager = PointerLibrary::getInstance()->getGUIManager();
  for (int i=0; i<stat_msg.getStatsCount(); i++)
  {
    //guimanager->GetStatusWindow()->AddSkil(*stat_msg.getName(i), stat_msg.getLevel(i));
    printf("Stat %s (%d): \t %d\n", *stat_msg.getName(i), stat_msg.getStatId(i), stat_msg.getLevel(i));
  }
}

void EntityHandler::handleStatsChange(GenericMessage* msg)
{
  //TODO: Implement
  StatsChangeMessage stat_msg;
  printf("Changing stat %d to %d\n", stat_msg.getStatId(), stat_msg.getLevel());
}

void EntityHandler::handleMoveTo(GenericMessage* msg)
{
  MoveToMessage move_msg;
  move_msg.deserialise(msg->getByteStream());
  
  float* fv1 = move_msg.getFrom();
  float* fv2 = move_msg.getTo();

  PointerLibrary::getInstance()->getEntityManager()->moveEntity(move_msg.getEntityId(), move_msg.getSpeed(), fv1, fv2);
}


void EntityHandler::handleSkillsList(GenericMessage* msg)
{
  SkillsListMessage skill_msg;
  skill_msg.deserialise(msg->getByteStream());
  printf("EntityHandler: Got %d skill(s) for the Character: \n---------------------------\n", skill_msg.getSkillsCount());
  GUIManager* guimanager = PointerLibrary::getInstance()->getGUIManager();
  for (int i=0; i<skill_msg.getSkillsCount(); i++)
  {
    //guimanager->GetInventoryWindow()->AddSkil(*stat_msg.getName(i), stat_msg.getStatLevel(i));
    printf("Skill %s (%d)\n", *skill_msg.getName(i), skill_msg.getSkillId(i));
  }
}

void EntityHandler::handleEquip(GenericMessage* msg)
{
  EquipMessage equip_msg;
  equip_msg.deserialise(msg->getByteStream());
  GUIManager* guimanager = PointerLibrary::getInstance()->getGUIManager();

  unsigned int entity = equip_msg.getEntityId();
  unsigned char slot = equip_msg.getSlotId();
  unsigned int item = equip_msg.getItemId();

  printf("EquipMessage: Entity %d equiped item %d to slot %d\n", entity, slot, item);

  PointerLibrary::getInstance()->getEntityManager()->equip(entity, item, slot);
}
void EntityHandler::handleInventoryMoveItem(GenericMessage* msg)
{
  InventoryMoveItemMessage invmove_msg;
  invmove_msg.deserialise(msg->getByteStream());
  GUIManager* guimanager = PointerLibrary::getInstance()->getGUIManager();

  unsigned char old_slot = invmove_msg.getOldSlot();
  unsigned char new_slot = invmove_msg.getNewSlot();

  printf("EquipMessage: You moved an item from slot %d to slot %d\n", old_slot, new_slot);

  guimanager->GetInventoryWindow()->MoveItem(old_slot, new_slot);
}

void EntityHandler::handleTeleport(GenericMessage* msg)
{
  TeleportMessage telemsg;
  telemsg.deserialise(msg->getByteStream());

  int entity_id = telemsg.getEntityId();
  float* pos = telemsg.getPos();
  const char* sector = *telemsg.getSector();

  PointerLibrary::getInstance()->getEntityManager()->teleport(entity_id, pos, sector);
}

void EntityHandler::handleAddPlayerEntity(GenericMessage* msg)
{
  printf("EntityHandler: Received AddEntity\n");
  AddPlayerEntityMessage entmsg;
  entmsg.deserialise(msg->getByteStream());

  using namespace PT::Events;
  EntityAddEvent* entity = new EntityAddEvent();

  entity->entityName  = *entmsg.getName();
  entity->meshName    = *entmsg.getMesh();
  csVector3 pos(entmsg.getPos()[0], entmsg.getPos()[1], entmsg.getPos()[2]);
  entity->position    = pos;
  entity->sectorName  = *entmsg.getSector();
  entity->entityId    = entmsg.getEntityId();
  entity->entityType  = EntityEvent::PlayerEntity; 

  for (size_t i = 0; i < entmsg.getEquipmentCount(); i++)
  {
    unsigned int itemId = entmsg.getItemId(i);
    unsigned int slotId = i;
    if (itemId == 0)  continue;
    entity->equipment.push_back( (std::pair<int, int> (itemId, slotId)) );
  }

  PointerLibrary::getInstance()->getEventManager()->AddEvent(entity);
}

void EntityHandler::handleAddItemEntity(GenericMessage* msg)
{
  printf("EntityHandler: Received AddItemEntity\n");

  AddItemEntityMessage entmsg;
  entmsg.deserialise(msg->getByteStream());

  using namespace PT::Events;
  EntityAddEvent* entity = new EntityAddEvent();

  entity->meshId      = entmsg.getItemId();
  csVector3 pos(entmsg.getPos()[0], entmsg.getPos()[1], entmsg.getPos()[2]);
  entity->position    = pos;
  entity->sectorName  = *entmsg.getSector();
  entity->entityId    = entmsg.getEntityId();
  entity->entityType  = EntityEvent::ItemEntity; 

  PointerLibrary::getInstance()->getEventManager()->AddEvent(entity);
}

void EntityHandler::handleAddMountEntity(GenericMessage* msg)
{
  printf("EntityHandler: Received AddMountEntity\n");
  AddMountEntityMessage entmsg;
  entmsg.deserialise(msg->getByteStream());

  using namespace PT::Events;
  EntityAddEvent* entity = new EntityAddEvent();

  entity->entityName  = *entmsg.getName();
  entity->meshName    = *entmsg.getMesh();
  csVector3 pos(entmsg.getPos()[0], entmsg.getPos()[1], entmsg.getPos()[2]);
  entity->position    = pos;
  entity->sectorName  = *entmsg.getSector();
  entity->entityId    = entmsg.getEntityId();
  entity->entityType  = EntityEvent::MountEntity; 

  PointerLibrary::getInstance()->getEventManager()->AddEvent(entity);
}

void EntityHandler::handleMount(GenericMessage* msg)
{
  MountMessage mount_msg;
  mount_msg.deserialise(msg->getByteStream());

  unsigned int entity = mount_msg.getPlayerEntityId();
  unsigned int mount = mount_msg.getMountEntityId();
  unsigned char control = mount_msg.getControl();

  PointerLibrary::getInstance()->getEntityManager()->mount(entity, mount, control > 0);
}

void EntityHandler::handleUnmount(GenericMessage* msg)
{
  UnmountMessage mount_msg;
  mount_msg.deserialise(msg->getByteStream());

  unsigned int entity = mount_msg.getPlayerEntityId();
  unsigned int mount = mount_msg.getMountEntityId();

  PointerLibrary::getInstance()->getEntityManager()->unmount(entity, mount);
}
