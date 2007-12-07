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

#include "server/entity/character.h"
#include "server/entity/pcentity.h"
#include "server/entity/entitymanager.h"
#include "server/entity/charactermanager.h"

#include "common/quest/npcdialog.h"
#include "common/quest/npcdialoganswer.h"
#include "common/quest/npcdialogmanager.h"

#include "server/database/database.h"
#include "server/database/table-npcdialogs.h"
#include "server/database/table-npcdialoganswers.h"

void QuestHandler::handleNpcDialogAnswer(GenericMessage* msg)
{
  const Character* c_char = NetworkHelper::getCharacter(msg);
  if (!c_char) return;

  Character* character = c_char->getLock();

  NPCDialogState* dia_state = character->getNPCDialogState();

  NpcDialogAnswerMessage message;
  message.deserialise(msg->getByteStream());

  const NPCDialog* dialog = dia_state->giveAnswer(message.getDialogId(), message.getAnswerId());

  if (dialog == 0)
  {
    NpcEntity* npc_entity = dia_state->getNpc()->getLock();
    if (npc_entity) 
    {
      npc_entity->pause(false);
      npc_entity->freeLock();

      NpcEndDialogMessage endmsg;
      endmsg.setNpcId(dia_state->getNpc()->getEntity()->getId());
      ByteStream bs;
      endmsg.serialise(&bs);
      server->broadCast(bs);
    }
    character->freeLock();
    return;
  }

  if (dialog->getAction() == NPCDialog::SHOW_TEXT)
  {
    NpcDialogMessage dialog_msg;
    dialog_msg.setDialogId((unsigned int)dialog->getDialogId());
    dialog_msg.setDialogText(dialog->getText());
    dialog_msg.setAnswersCount((unsigned char)dialog->getAnswerCount());

    // The npc has nothing more to say, let him walk away.
    if (!dialog->getAnswerCount()) {
      NpcEntity* npc_entity = dia_state->getNpc()->getLock();
      if (npc_entity) 
      {
        npc_entity->pause(false);
        npc_entity->freeLock();

        NpcEndDialogMessage endmsg;
        endmsg.setNpcId(dia_state->getNpc()->getEntity()->getId());
        ByteStream bs;
        endmsg.serialise(&bs);
        server->broadCast(bs);
      }
    }

    for (size_t i = 0; i < dialog->getAnswerCount(); i++)
    {
      const NPCDialogAnswer* answer = dialog->getAnswer(i);
      dialog_msg.setAnswerId(i, (unsigned int)i);
      dialog_msg.setAnswerText(i, answer->getText());
    }

    ByteStream bs;
    dialog_msg.serialise(&bs);

    NetworkHelper::sendMessage(character, bs);
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
      trade_msg.setPrice(0, 200);
      trade_msg.setItemId(1, 3);
      trade_msg.setPrice(1, 100);
      trade_msg.setItemId(2, 6); //empty book
      trade_msg.setPrice(2, 10);
      trade_msg.setItemId(3, 7); //gate key
      trade_msg.setPrice(3, 300);

      ByteStream bs;
      trade_msg.serialise(&bs);

      NetworkHelper::sendMessage(character, bs);

      NpcEndDialogMessage endmsg;
      endmsg.setNpcId(dia_state->getNpc()->getEntity()->getId());
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
      trade_msg.setPrice(0, 175);
      trade_msg.setItemId(1, 3);
      trade_msg.setPrice(1,  75);

      ByteStream bs;
      trade_msg.serialise(&bs);

      NetworkHelper::sendMessage(character, bs);

      NpcEndDialogMessage endmsg;
      endmsg.setNpcId(dia_state->getNpc()->getEntity()->getId());
      ByteStream bs2;
      endmsg.serialise(&bs2);
      server->broadCast(bs2);
    }
  }
  else if (dialog->getAction() == NPCDialog::TELEPORT)
  {
    // yes, it's a hack. This shouldn't go here either.
    // sector_id <0.5, 0.6, 0.8>
    int sector = 0;
    float x = 0, y = 0, z = 0;
    sscanf(dialog->getText(), "%d<%f,%f,%f>", &sector, &x, &y, &z);

    Entity* ent = character->getEntity()->getLock();
    ent->setSector(sector);
    ent->setPos(x, y, z);
    ent->freeLock();

    server->getCharacterManager()->checkForSave(ent->getPlayerEntity());

    TeleportMessage telemsg;
    telemsg.setEntityId(ent->getId());
    telemsg.setSectorId(ent->getSector());
    telemsg.setPos(ent->getPos());

    ByteStream bs;
    telemsg.serialise(&bs);
    server->broadCast(bs);

    NpcEndDialogMessage endmsg;
    endmsg.setNpcId(dia_state->getNpc()->getEntity()->getId());
    ByteStream bs2;
    endmsg.serialise(&bs2);
    server->broadCast(bs2);
  }

  character->freeLock();
}

void QuestHandler::handleNpcStartDialog(GenericMessage* msg)
{
  const Character* c_char = NetworkHelper::getCharacter(msg);
  if (!c_char) return;

  Character* character = c_char->getLock();

  NPCDialogState* dia_state = character->getNPCDialogState();

  NpcStartDialogMessage message;
  message.deserialise(msg->getByteStream());

  unsigned int npc_id = message.getNpcId();
  const Entity* npc_ent = server->getEntityManager()->findById(npc_id);

  if (!npc_ent || npc_ent->getType() != Entity::NPCEntityType)
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
  NpcEntity* npc_entity = npc_ent->getNpcEntity()->getLock();
  npc_entity->pause(true);

  dia_state->setNpc(npc_entity);
  const NPCDialog* dialog = dia_state->startDialog(npc_entity->getStartDialog());

  npc_entity->freeLock();

  NpcDialogMessage dialog_msg;
  dialog_msg.setDialogId((unsigned int)dialog->getDialogId());
  dialog_msg.setDialogText(dialog->getText());
  dialog_msg.setAnswersCount((unsigned char)dialog->getAnswerCount());
  for (size_t i = 0; i < dialog->getAnswerCount(); i++)
  {
    const NPCDialogAnswer* answer = dialog->getAnswer(i);
    dialog_msg.setAnswerId(i, (unsigned int)i);
    dialog_msg.setAnswerText(i, answer->getText());
  }

  ByteStream bs;
  dialog_msg.serialise(&bs);

  NetworkHelper::sendMessage(character, bs);
  character->freeLock();
}

void QuestHandler::handleNpcEndDialog(GenericMessage* msg)
{
  const Character* c_char = NetworkHelper::getCharacter(msg);
  if (!c_char) return;

  Character* character = c_char->getLock();

  NPCDialogState* dia_state = character->getNPCDialogState();

  NpcEndDialogMessage message;
  message.deserialise(msg->getByteStream());

  dia_state->endDialog(0);
  NpcEntity* npc_entity = dia_state->getNpc()->getLock();
  if (npc_entity) 
  {
    printf("Unpausing npc\n");
    npc_entity->pause(false);
    npc_entity->freeLock();
  }
  character->freeLock();
}

void QuestHandler::handleSetupDialogs(GenericMessage* msg)
{
  User* user = NetworkHelper::getUser(msg)->getLock();
  size_t level = user->getPermissionList().getLevel(Permission::Admin);
  user->freeLock();

  // TODO: send "not authorised message"
  if (level == 0) return;

  SetupDialogsMessage setupmsg;
  setupmsg.deserialise(msg->getByteStream());

  NpcDialogsTable* dialogtable = 
    Server::getServer()->getDatabase()->getNpcDialogsTable();

  NpcDialogAnswersTable* answertable =
    Server::getServer()->getDatabase()->getNpcDialogAnswersTable();

  if (setupmsg.getDeleteExisting())
  {
    answertable->dropTable();
    dialogtable->dropTable();
    dialogtable->createTable();
    answertable->createTable();
  }

  for (size_t i = 0; i < setupmsg.getDialogsCount(); i++)
  {
    unsigned int dialogid = setupmsg.getDialogId(i);
    const char* text = setupmsg.getValue(i);
    bool start = setupmsg.getIsStartDialog(i);
    ptString action = setupmsg.getAction(i);
    dialogtable->insert(dialogid, text, start, *action);
  }

  for (size_t i = 0; i < setupmsg.getAnswersCount(); i++)
  {
    unsigned int dialogid = setupmsg.getAnswerDialogId(i);
    unsigned int answerid = setupmsg.getAnswerId(i);
    const char* text = setupmsg.getAnswerText(i);
    bool end = setupmsg.getIsEndAnswer(i);
    unsigned int link = setupmsg.getAnswerLink(i);
    answertable->insert(dialogid, answerid, text, end, link);
  }

  NPCDialogManager::getDialogManager().load();
}
