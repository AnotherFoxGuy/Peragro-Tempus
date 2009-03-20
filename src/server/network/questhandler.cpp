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

#include "networkhelper.h"
#include "server/network/network.h"
#include "server/network/connection.h"
#include "server/entity/user.h"

#include "server/entity/character/character.h"
#include "server/entity/entitymanager.h"

#include "server/quest/npcdialog.h"
#include "server/quest/npcdialoganswer.h"
#include "server/quest/npcdialogmanager.h"
#include "server/quest/questutils.h"

#include "common/database/database.h"
#include "server/database/table-npcdialogs.h"
#include "server/database/table-npcdialoganswers.h"
#include "server/database/table-npcentities.h"
#include "server/database/table-characters.h"

void QuestHandler::handleNpcDialogAnswer(GenericMessage* msg)
{
  /*
  const Character* c_char = NetworkHelper::getCharacter(msg);
  if (!c_char) return;

  ptScopedMonitorable<Character> character (c_char);

  NPCDialogState* dia_state = character->getNPCDialogState();

  NpcDialogAnswerMessage message;
  message.deserialise(msg->getByteStream());

  Server* server = Server::getServer();

  const NPCDialog* dialog = dia_state->giveAnswer(message.getDialogId(), message.getAnswerId());

  if (dialog == 0)
  {
    ptScopedMonitorable<NpcEntity> npc_entity (dia_state->getNpc());
    npc_entity->pause(false);

    NpcEndDialogMessage endmsg;
    endmsg.setNpcId(dia_state->getNpc()->getEntity()->GetId());
    ByteStream bs;
    endmsg.serialise(&bs);
    server->broadCast(bs);

    return;
  }

  if (dialog->getAction() == NPCDialog::SHOW_TEXT)
  {
    QuestUtils::SendDialog(character, dialog);
  }
  else if (dialog->getAction() == NPCDialog::START_BUY)
  {
    const NpcEntity* c_npc = dia_state->getNpc();
    if (c_npc)
    {
      // hardcoded! just to test it!
      // All this should go to the trade handler anyway!
      TradeOffersListNpcMessage trade_msg;
      trade_msg.setIsBuy(1);
      trade_msg.setOffersCount(4);

      trade_msg.setItemId(0, 4);
      trade_msg.setName(0, ptString("Pot", 3));
      trade_msg.setIconName(0, ptString("set:Inventory image:tinyballpot", 31));
      trade_msg.setPrice(0, 200);

      trade_msg.setItemId(1, 3);
      trade_msg.setName(1, ptString("Scythe", 6));
      trade_msg.setIconName(1, ptString("set:Inventory image:Scythe", 26));
      trade_msg.setPrice(1, 100);

      trade_msg.setItemId(2, 6); //empty book
      trade_msg.setName(2, ptString("Empty book", 10));
      trade_msg.setIconName(2, ptString("set:Inventory image:oldbook", 27));
      trade_msg.setPrice(2, 10);

      trade_msg.setItemId(3, 7); //gate key
      trade_msg.setName(3, ptString("Gate key", 8));
      trade_msg.setIconName(3, ptString("set:Inventory image:key01", 25));
      trade_msg.setPrice(3, 300);

      ByteStream bs;
      trade_msg.serialise(&bs);

      NetworkHelper::sendMessage(character, bs);

      NpcEndDialogMessage endmsg;
      endmsg.setNpcId(dia_state->getNpc()->getEntity()->GetId());
      ByteStream bs2;
      endmsg.serialise(&bs2);
      server->broadCast(bs2);
    }
  }
  else if (dialog->getAction() == NPCDialog::START_SELL)
  {
    const NpcEntity* c_npc = dia_state->getNpc();
    if (c_npc)
    {
      // hardcoded! just to test it!
      // All this should go to the trade handler anyway!
      TradeOffersListNpcMessage trade_msg;
      trade_msg.setIsBuy(0);
      trade_msg.setOffersCount(2);

      trade_msg.setItemId(0, 4);
      trade_msg.setName(0, ptString("Pot", 3));
      trade_msg.setIconName(0, ptString("set:Inventory image:tinyballpot", 31));
      trade_msg.setPrice(0, 175);

      trade_msg.setItemId(1, 3);
      trade_msg.setName(1, ptString("Scythe", 6));
      trade_msg.setIconName(1, ptString("set:Inventory image:Scythe", 26));
      trade_msg.setPrice(1,  75);

      ByteStream bs;
      trade_msg.serialise(&bs);

      NetworkHelper::sendMessage(character, bs);

      NpcEndDialogMessage endmsg;
      endmsg.setNpcId(dia_state->getNpc()->getEntity()->GetId());
      ByteStream bs2;
      endmsg.serialise(&bs2);
      server->broadCast(bs2);
    }
  }
  else if (dialog->getAction() == NPCDialog::TELEPORT)
  {
    // yes, it's a hack. This shouldn't go here either.
    // sector_id <0.5, 0.6, 0.8>
    unsigned short sector = 0;
    float x = 0, y = 0, z = 0;
    sscanf(dialog->getText(), "%hd<%f,%f,%f>", &sector, &x, &y, &z);

    ptScopedMonitorable<Entity> ent (character->getEntity());
    ent->SetSector(sector);
    ent->SetPosition(x, y, z);

    server->getCharacterManager()->checkForSave(ent->getPlayerEntity());

    TeleportResponseMessage telemsg;
    telemsg.setEntityId(ent->GetId());
    telemsg.SetPosition(ent->GetPosition());
    telemsg.SetRotation(ent->GetRotation());
    telemsg.SetSectorId(ent->GetSector());

    ByteStream bs;
    telemsg.serialise(&bs);
    server->broadCast(bs);

    NpcEndDialogMessage endmsg;
    endmsg.setNpcId(dia_state->getNpc()->getEntity()->GetId());
    ByteStream bs2;
    endmsg.serialise(&bs2);
    server->broadCast(bs2);
  }
  else if (dialog->getAction() == NPCDialog::FUNCTION)
  {
    QuestUtils::Parse(character, dialog->getText());
  } // end ABILITYCHECK
  */
}

void QuestHandler::handleNpcStartDialog(GenericMessage* msg)
{
  /*
  const Character* c_char = NetworkHelper::getCharacter(msg);
  if (!c_char) return;

  ptScopedMonitorable<Character> character (c_char);

  NPCDialogState* dia_state = character->getNPCDialogState();

  NpcStartDialogMessage message;
  message.deserialise(msg->getByteStream());

  Server* server = Server::getServer();

  unsigned int npc_id = message.getNpcId();
  const Entity* npc_ent = server->getEntityManager()->findById(npc_id);

  if (!npc_ent || npc_ent->GetType() != Common::Entity::NPCEntityType)
    return;

  // TODO: NPC will be unpaused when its done with the chat.
  // However, the player might press the "X" before the chat is over
  // this needs to unpause the NPC as well. Problem is if we are done
  // with our chat, someone else starts to chat with the NPC
  // and we press "X" during there chat - this will then unpause
  // the NPC in the middle of a chat
  //
  // Or is it impossible for someone else to start a chat with the NPC
  // before we press the "X" this could be really bad too since then a player
  // could hold up an NPC forever - need a time out for that then.
  ptScopedMonitorable<NpcEntity> npc_entity (npc_ent->getNpcEntity());
  npc_entity->pause(true);

  dia_state->setNpc(npc_entity);
  const NPCDialog* dialog = dia_state->startDialog(npc_id);

  if (!dialog)
  {
    return;
  }

  if (dialog->getAction() == NPCDialog::SHOW_TEXT)
  {
    QuestUtils::SendDialog(character, dialog);
  }
  else if (dialog->getAction() == NPCDialog::START_BUY)
  {
    const NpcEntity* c_npc = dia_state->getNpc();
    if (c_npc)
    {
      // hardcoded! just to test it!
      // All this should go to the trade handler anyway!
      TradeOffersListNpcMessage trade_msg;
      trade_msg.setIsBuy(1);
      trade_msg.setOffersCount(4);

      trade_msg.setItemId(0, 4);
      trade_msg.setName(0, ptString("Pot", 3));
      trade_msg.setIconName(0, ptString("set:Inventory image:tinyballpot", 31));
      trade_msg.setPrice(0, 200);

      trade_msg.setItemId(1, 3);
      trade_msg.setName(1, ptString("Scythe", 6));
      trade_msg.setIconName(1, ptString("set:Inventory image:Scythe", 26));
      trade_msg.setPrice(1, 100);

      trade_msg.setItemId(2, 6); //empty book
      trade_msg.setName(2, ptString("Empty book", 10));
      trade_msg.setIconName(2, ptString("set:Inventory image:oldbook", 27));
      trade_msg.setPrice(2, 10);

      trade_msg.setItemId(3, 7); //gate key
      trade_msg.setName(3, ptString("Gate key", 8));
      trade_msg.setIconName(3, ptString("set:Inventory image:key01", 25));
      trade_msg.setPrice(3, 300);

      ByteStream bs;
      trade_msg.serialise(&bs);

      NetworkHelper::sendMessage(character, bs);

      NpcEndDialogMessage endmsg;
      endmsg.setNpcId(dia_state->getNpc()->getEntity()->GetId());
      ByteStream bs2;
      endmsg.serialise(&bs2);
      server->broadCast(bs2);
    }
  }
  else if (dialog->getAction() == NPCDialog::START_SELL)
  {
    const NpcEntity* c_npc = dia_state->getNpc();
    if (c_npc)
    {
      // hardcoded! just to test it!
      // All this should go to the trade handler anyway!
      TradeOffersListNpcMessage trade_msg;
      trade_msg.setIsBuy(0);
      trade_msg.setOffersCount(2);

      trade_msg.setItemId(0, 4);
      trade_msg.setName(0, ptString("Pot", 3));
      trade_msg.setIconName(0, ptString("set:Inventory image:tinyballpot", 31));
      trade_msg.setPrice(0, 175);

      trade_msg.setItemId(1, 3);
      trade_msg.setName(1, ptString("Scythe", 6));
      trade_msg.setIconName(1, ptString("set:Inventory image:Scythe", 26));
      trade_msg.setPrice(1,  75);

      ByteStream bs;
      trade_msg.serialise(&bs);

      NetworkHelper::sendMessage(character, bs);

      NpcEndDialogMessage endmsg;
      endmsg.setNpcId(dia_state->getNpc()->getEntity()->GetId());
      ByteStream bs2;
      endmsg.serialise(&bs2);
      server->broadCast(bs2);
    }
  }
  else if (dialog->getAction() == NPCDialog::TELEPORT)
  {
    // yes, it's a hack. This shouldn't go here either.
    // sector_id <0.5, 0.6, 0.8>
    unsigned short sector = 0;
    float x = 0, y = 0, z = 0;
    sscanf(dialog->getText(), "%hd<%f,%f,%f>", &sector, &x, &y, &z);

    ptScopedMonitorable<Entity> ent (character->getEntity());
    ent->SetSector(sector);
    ent->SetPosition(x, y, z);

    server->getCharacterManager()->checkForSave(ent->getPlayerEntity());

    TeleportResponseMessage telemsg;
    telemsg.setEntityId(ent->GetId());
    telemsg.SetPosition(ent->GetPosition());
    telemsg.SetRotation(ent->GetRotation());
    telemsg.SetSectorId(ent->GetSector());

    ByteStream bs;
    telemsg.serialise(&bs);
    server->broadCast(bs);

    NpcEndDialogMessage endmsg;
    endmsg.setNpcId(dia_state->getNpc()->getEntity()->GetId());
    ByteStream bs2;
    endmsg.serialise(&bs2);
    server->broadCast(bs2);
  }
  else if (dialog->getAction() == NPCDialog::FUNCTION)
  {
    QuestUtils::Parse(character, dialog->getText());
  }
  */
}

void QuestHandler::handleNpcEndDialog(GenericMessage* msg)
{
  /*
  const Character* c_char = NetworkHelper::getCharacter(msg);
  if (!c_char) return;

  ptScopedMonitorable<Character> character (c_char);

  NPCDialogState* dia_state = character->getNPCDialogState();

  NpcEndDialogMessage message;
  message.deserialise(msg->getByteStream());

  dia_state->endDialog(message.getNpcId(),0);
  ptScopedMonitorable<NpcEntity> npc_entity (dia_state->getNpc());
  printf("Unpausing npc\n");
  npc_entity->pause(false);
  */
}

void QuestHandler::handleSetupDialogs(GenericMessage* msg)
{
  /*
  ptScopedMonitorable<User> user (NetworkHelper::getUser(msg));
  size_t level = user->getPermissionList().getLevel(Permission::Admin);

  // TODO: send "not authorized message"
  if (level == 0) return;

  SetupDialogsMessage setupmsg;
  setupmsg.deserialise(msg->getByteStream());

  CharacterTable* charactertable =
    Server::getServer()->getTables()->getCharacterTable();

  NpcEntitiesTable* npcentitytable =
    Server::getServer()->getTables()->getNpcEntitiesTable();

  NpcDialogsTable* dialogtable =
    Server::getServer()->getTables()->getNpcDialogsTable();

  NpcDialogAnswersTable* answertable =
    Server::getServer()->getTables()->getNpcDialogAnswersTable();

  if (setupmsg.getDeleteExisting())
  {
    answertable->dropTable();
    dialogtable->dropTable();
    dialogtable->createTable();
    answertable->createTable();
  }

  for (size_t i = 0; i < setupmsg.getDialogsCount(); i++)
  {
    ptString npcname = setupmsg.getNpcName(i);
    CharactersTableVO* character = charactertable->FindCharacterByName(*npcname);
    if (!character){ printf("Failed to find NPC %s\n",*npcname); continue; }
    NpcEntitiesTableVO* npc = npcentitytable->getByCharacter(character->id);
    if (!npc){ printf("Failed to find NPC %s's ID\n",*npcname); continue; }
    unsigned int dialogid = setupmsg.getDialogId(i);
    const char* text = setupmsg.getValue(i);
    ptString action = setupmsg.getAction(i);
    dialogtable->insert(npc->id, dialogid, text, *action);
  }

  for (size_t i = 0; i < setupmsg.getAnswersCount(); i++)
  {
    ptString npcname = setupmsg.getAnswerNpcName(i);
    CharactersTableVO* character = charactertable->FindCharacterByName(*npcname);
    if (!character){ printf("Failed to find NPC %s\n",*npcname); continue; }
    NpcEntitiesTableVO* npc = npcentitytable->getByCharacter(character->id);
    if (!npc){ printf("Failed to find NPC %s's ID\n",*npcname); continue; }
    unsigned int dialogid = setupmsg.getAnswerDialogId(i);
    unsigned int answerid = setupmsg.getAnswerId(i);
    const char* text = setupmsg.getAnswerText(i);
    bool end = setupmsg.getIsEndAnswer(i);
    unsigned int link = setupmsg.getAnswerLink(i);
    answertable->insert(npc->id, dialogid, answerid, text, end, link);
  }

  NPCDialogManager::getDialogManager().load();
  */
}
