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

#include "client/event/eventmanager.h"
#include "client/event/entityevent.h"
#include "client/event/tradeevent.h"

#include "client/reporter/reporter.h"

void PlayerHandler::handleInventoryList(GenericMessage* msg)
{
  InventoryListMessage item_msg;
  item_msg.deserialise(msg->getByteStream());

  Report(PT::Debug, "---------------------------");
  Report(PT::Debug, "EntityHandler: Got %d items in the Inventory:", item_msg.getInventoryCount());
  for (int i=0; i<item_msg.getInventoryCount(); i++)
  {
    using namespace PT::Events;
    InventoryAddEvent* tradeEvent = new InventoryAddEvent();

    tradeEvent->name         = *item_msg.getName(i);
    tradeEvent->itemId       = item_msg.getItemId(i);
    tradeEvent->slotId       = item_msg.getSlotId(i);
    tradeEvent->variationId  = item_msg.getVariation(i);
    tradeEvent->weight       = item_msg.getWeight(i);
    tradeEvent->description  = *item_msg.getDescription(i);
    tradeEvent->equipType    = *item_msg.getEquipType(i);
    tradeEvent->icon         = *item_msg.getIcon(i);

    PointerLibrary::getInstance()->getEventManager()->AddEvent(tradeEvent);
  }
  Report(PT::Debug, "---------------------------\n");
}

void PlayerHandler::handleStatsList(GenericMessage* msg)
{
  StatsListMessage stat_msg;
  stat_msg.deserialise(msg->getByteStream());
  Report(PT::Debug, "---------------------------");
  Report(PT::Debug, "EntityHandler: Got %d stats for the Character:", stat_msg.getStatsCount());
  GUIManager* guimanager = PointerLibrary::getInstance()->getGUIManager();
  for (int i=0; i<stat_msg.getStatsCount(); i++)
  {
    guimanager->GetStatusWindow()->AddSkil(*stat_msg.getName(i), stat_msg.getLevel(i)); // TODO: Do this in the status window instead, using the event
    Report(PT::Debug, "Stat %s (%d): \t %d", *stat_msg.getName(i), stat_msg.getStatId(i), stat_msg.getLevel(i));

    using namespace PT::Events;
    EntityStatEvent* statEvent=new EntityStatEvent();
    statEvent->name  = *stat_msg.getName(i);
    statEvent->id    = stat_msg.getStatId(i);
    statEvent->level = stat_msg.getLevel(i);

    PointerLibrary::getInstance()->getEventManager()->AddEvent(statEvent);
  }
  Report(PT::Debug, "---------------------------\n");
}

void PlayerHandler::handleStatsChange(GenericMessage* msg)
{
  //TODO: Implement
  StatsChangeMessage stat_msg;
  Report(PT::Debug, "Changing stat %d to %d.", stat_msg.getStatId(), stat_msg.getLevel());
}

void PlayerHandler::handleSkillsList(GenericMessage* msg)
{
  SkillsListMessage skill_msg;
  skill_msg.deserialise(msg->getByteStream());
  Report(PT::Debug, "---------------------------");
  Report(PT::Debug, "EntityHandler: Got %d skill(s) for the Character:", skill_msg.getSkillsCount());
  for (int i=0; i<skill_msg.getSkillsCount(); i++)
  {
    //guimanager->GetInventoryWindow()->AddSkil(*stat_msg.getName(i), stat_msg.getStatLevel(i));
    Report(PT::Debug, "Skill %s (%d)", *skill_msg.getName(i), skill_msg.getSkillId(i));
  }
  Report(PT::Debug, "---------------------------\n");
}

void PlayerHandler::handleInventoryMoveItem(GenericMessage* msg)
{
  InventoryMoveItemMessage invmove_msg;
  invmove_msg.deserialise(msg->getByteStream());
  GUIManager* guimanager = PointerLibrary::getInstance()->getGUIManager();

  unsigned char old_slot = invmove_msg.getOldSlot();
  unsigned char new_slot = invmove_msg.getNewSlot();

  Report(PT::Debug, "EquipMessage: You moved an item from slot %d to slot %d", old_slot, new_slot);

  guimanager->GetInventoryWindow()->MoveItem(old_slot, new_slot);
}
