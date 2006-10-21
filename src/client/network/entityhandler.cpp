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

void EntityHandler::handleAddNpcEntity(GenericMessage* msg)
{
  printf("EntityHandler: Received AddNpcEntity\n");
  AddNpcEntityMessage entmsg;
  entmsg.deserialise(msg->getByteStream());
  PtEntity* entity = new PtNpcEntity();
  entity->SetName(*entmsg.getName());
  entity->SetMeshName(*entmsg.getMesh());
  csVector3 pos(entmsg.getPos()[0], entmsg.getPos()[1], entmsg.getPos()[2]);
  entity->SetPosition(pos);
  entity->SetSectorName(*entmsg.getSector());
  entity->SetId(entmsg.getEntityId());
  PointerLibrary::getInstance()->getEntityManager()->addEntity(entity);
}

void EntityHandler::handleAddDoorEntity(GenericMessage* msg)
{
  printf("EntityHandler: Received AddDoor\n");
  AddDoorEntityMessage entmsg;
  entmsg.deserialise(msg->getByteStream());
  PtDoorEntity* entity = 0;
  entity = new PtDoorEntity();
  entity->SetName(*entmsg.getName());
  entity->SetMeshName(*entmsg.getMesh());
  entity->SetOpen(entmsg.getIsOpen() != 0);
  entity->SetLocked(entmsg.getIsLocked() != 0);
  entity->SetId(entmsg.getEntityId());
  PointerLibrary::getInstance()->getEntityManager()->addEntity(entity);
}
void EntityHandler::handleRemove(GenericMessage* msg)
{
  printf("EntityHandler: Received RemoveEntity\n");
  RemoveMessage entmsg;
  entmsg.deserialise(msg->getByteStream());
  PtEntity* entity = PointerLibrary::getInstance()->getEntityManager()->findPtEntById(entmsg.getEntityId());
  if (!entity) return;
  PointerLibrary::getInstance()->getEntityManager()->delEntity(entity);
  if (entity->GetType() == PtEntity::PlayerEntity)
    PointerLibrary::getInstance()->getGUIManager()->GetBuddyWindow()->RemovePlayer(entity->GetName().GetData());
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
  const char* ownname = PointerLibrary::getInstance()->getEntityManager()->GetOwnName().GetData();

  if (response_msg.getError().isNull())
  {
    printf("%s picks Item %s(%d) %s\n", *response_msg.getName(), *response_msg.getTarget(), response_msg.getItemId(),ownname);
    if (strlen(*response_msg.getName()) == strlen(ownname) && !strcmp(*response_msg.getName(), ownname))
      guimanager->GetInventoryWindow()->AddItem(response_msg.getItemId(), response_msg.getSlot(), 1);
  }
  else
    printf("%s can't pick Item %s! Reason: '%s'\n", *response_msg.getName(), *response_msg.getTarget(), *response_msg.getError());
}

void EntityHandler::handleDropResponse(GenericMessage* msg)
{
  DropResponseMessage response_msg;
  response_msg.deserialise(msg->getByteStream());

  if (response_msg.getError().isNull())
    printf("%s picks %s\n", *response_msg.getName(), *response_msg.getTarget());
  else
    printf("%s can't pick %s! Reason: '%s'\n", *response_msg.getName(), *response_msg.getTarget(), *response_msg.getError());
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
    printf("Item %d with amount %d in slot %d\n", item_msg.getItemId(i), item_msg.getAmount(i), item_msg.getSlotId(i));
    guimanager->GetInventoryWindow()->AddItem(item_msg.getItemId(i), item_msg.getSlotId(i), item_msg.getAmount(i));
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
    guimanager->GetStatusWindow()->AddSkil(*stat_msg.getName(i), stat_msg.getLevel(i));
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
  if (!equip_msg.getError().isNull())
  {
    printf("EquipMessage: ERROR: '%s'\n", *equip_msg.getError());
    return;
  }
  else
  {
    if (equip_msg.getNewSlotId() < 10)
    {
      printf("EquipMessage: Entity %d equiped item %d to slot %d\n", 
        equip_msg.getEntityId(), equip_msg.getItemId(), equip_msg.getNewSlotId());
      guimanager->GetInventoryWindow()->MoveItem(equip_msg.getOldSlotId(), equip_msg.getNewSlotId());
      PointerLibrary::getInstance()->getEntityManager()->equip(equip_msg.getEntityId(), equip_msg.getItemId(), equip_msg.getNewSlotId());
    }
    else
    {
      printf("EquipMessage: You moved item %d from slot %d to slot %d\n", 
        equip_msg.getItemId(), equip_msg.getOldSlotId(), equip_msg.getNewSlotId());
      if(equip_msg.getOldSlotId() < 10)
      {
        PointerLibrary::getInstance()->getEntityManager()->unequip(equip_msg.getEntityId(), equip_msg.getOldSlotId());
      }
      guimanager->GetInventoryWindow()->MoveItem(equip_msg.getOldSlotId(), equip_msg.getNewSlotId());
    }
  }
  // do something
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
  PtPcEntity* entity = new PtPcEntity();
  entity->SetName(*entmsg.getName());
  entity->SetMeshName(*entmsg.getMesh());
  csVector3 pos(entmsg.getPos()[0], entmsg.getPos()[1], entmsg.getPos()[2]);
  entity->SetPosition(pos);
  entity->SetSectorName(*entmsg.getSector());
  entity->SetId(entmsg.getEntityId());
  PointerLibrary* ptr = PointerLibrary::getInstance();
  ptr->getEntityManager()->addEntity(entity);
  ptr->getGUIManager()->GetBuddyWindow()->AddPlayer(*entmsg.getName());
  for (size_t i = 0; i < entmsg.getEquipmentCount(); i++)
  {
    unsigned int itemId = entmsg.getItemId(i);

    if (itemId == 0) 
      continue;

    entity->GetEquipment()->Equip(itemId, i);
  }
}

void EntityHandler::handleAddItemEntity(GenericMessage* msg)
{
  printf("EntityHandler: Received AddItemEntity\n");

  PointerLibrary* ptr = PointerLibrary::getInstance();

  AddItemEntityMessage entmsg;
  entmsg.deserialise(msg->getByteStream());

  PtItemEntity* entity = new PtItemEntity();
  csVector3 pos(entmsg.getPos()[0], entmsg.getPos()[1], entmsg.getPos()[2]);
  entity->SetPosition(pos);
  entity->SetSectorName(*entmsg.getSector());
  entity->SetItemId(entmsg.getItemId());
  entity->SetId(entmsg.getEntityId());
  ptr->getEntityManager()->addEntity(entity);
}
