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

#include "client/entity/entitymanager.h"
#include "common/event/eventmanager.h"
#include "common/event/entityevent.h"

#include "common/reporter/reporter.h"

void PlayerHandler::handleInventoryList(GenericMessage* msg)
{
  InventoryListMessage item_msg;
  item_msg.deserialise(msg->getByteStream());

  PT::Events::EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();

  Report(PT::Debug, "---------------------------");
  Report(PT::Debug, "PlayerHandler: Got %d items in the Inventory:", item_msg.getInventoryCount());
  for (int i=0; i<item_msg.getInventoryCount(); i++)
  {
    csRef<iEvent> playerEvent = evmgr->CreateEvent("trade.inventory.add", true);

    playerEvent->Add("name", *item_msg.getName(i));
    playerEvent->Add("itemId", item_msg.getItemId(i));
    playerEvent->Add("slotId", item_msg.getSlotId(i));
    playerEvent->Add("variationId", item_msg.getVariation(i));
    playerEvent->Add("weight", item_msg.getWeight(i));
    playerEvent->Add("description", *item_msg.getDescription(i));
    playerEvent->Add("equipType", *item_msg.getEquipType(i));
    playerEvent->Add("icon", *item_msg.getIcon(i));

    evmgr->AddEvent(playerEvent);
  }
  Report(PT::Debug, "---------------------------\n");
}

void PlayerHandler::handleStatsList(GenericMessage* msg)
{
  using namespace PT::GUI;
  using namespace PT::GUI::Windows;

  StatsListMessage stat_msg;
  stat_msg.deserialise(msg->getByteStream());

  PT::Events::EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();

  Report(PT::Debug, "---------------------------");
  Report(PT::Debug, "EntityHandler: Got %d stats for the Character:", stat_msg.getStatsCount());
  GUIManager* guimanager = PointerLibrary::getInstance()->getGUIManager();
  for (int i=0; i<stat_msg.getStatsCount(); i++)
  {
    StatusWindow* statusWindow = guimanager->GetWindow<StatusWindow>(STATUSWINDOW);
    statusWindow->AddSkill(*stat_msg.getName(i), stat_msg.getLevel(i)); // TODO: Do this in the status window instead, using the event
    //guimanager->GetStatusWindow()->AddSkill(*stat_msg.getName(i), stat_msg.getLevel(i)); // TODO: Do this in the status window instead, using the event
    Report(PT::Debug, "Stat %s (%d): \t %d", *stat_msg.getName(i), stat_msg.getStatId(i), stat_msg.getLevel(i));

    csRef<iEvent> playerEvent = evmgr->CreateEvent("entity.stat.add.player", true);

    playerEvent->Add("name", *stat_msg.getName(i));
    playerEvent->Add("id", stat_msg.getStatId(i));
    playerEvent->Add("level", stat_msg.getLevel(i));

    evmgr->AddEvent(playerEvent);
  }
  Report(PT::Debug, "---------------------------\n");
}

void PlayerHandler::handleStatsChange(GenericMessage* msg)
{
  StatsChangeMessage stat_msg;
  stat_msg.deserialise(msg->getByteStream());

  using namespace PT::Events;
  EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
  csRef<iEvent> playerEvent = evmgr->CreateEvent(EntityHelper::MakeEntitySpecific("entity.stat.change", stat_msg.getEntityId()), true);

  playerEvent->Add("name", *stat_msg.getName());
  playerEvent->Add("level", stat_msg.getLevel());
  playerEvent->Add("id", stat_msg.getStatId());
  playerEvent->Add("entityid", stat_msg.getEntityId());

  evmgr->AddEvent(playerEvent);
}

void PlayerHandler::handleSkillsList(GenericMessage* msg)
{
  SkillsListMessage skill_msg;
  skill_msg.deserialise(msg->getByteStream());

  Report(PT::Debug, "---------------------------");
  Report(PT::Debug, "EntityHandler: Got %d skill(s) for the Character:", skill_msg.getSkillsCount());
  for (int i=0; i<skill_msg.getSkillsCount(); i++)
  {
    //guimanager->GetInventoryWindow()->AddSkill(*stat_msg.getName(i), stat_msg.getStatLevel(i));
    Report(PT::Debug, "Skill %s (%d)", *skill_msg.getName(i), skill_msg.getSkillId(i));
  }
  Report(PT::Debug, "---------------------------\n");
}

void PlayerHandler::handleInventoryMoveItem(GenericMessage* msg)
{
  using namespace PT::GUI;
  using namespace PT::GUI::Windows;

  InventoryMoveItemMessage invmove_msg;
  invmove_msg.deserialise(msg->getByteStream());
  GUIManager* guimanager = PointerLibrary::getInstance()->getGUIManager();

  unsigned char old_slot = invmove_msg.getOldSlot();
  unsigned char new_slot = invmove_msg.getNewSlot();

  Report(PT::Debug, "EquipMessage: You moved an item from slot %d to slot %d", old_slot, new_slot);

  InventoryWindow* inventoryWindow = guimanager->GetWindow<InventoryWindow>(INVENTORYWINDOW);
  inventoryWindow->MoveItem(old_slot, new_slot);
}
