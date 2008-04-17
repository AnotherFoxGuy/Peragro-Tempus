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
#include "server/entity/entitymanager.h"
#include "server/entity/charactermanager.h"

#include "common/quest/npcdialog.h"
#include "common/quest/npcdialoganswer.h"
#include "common/quest/npcdialogmanager.h"

#include "server/entity/statmanager.h"

#include "server/database/database.h"
#include "server/database/table-npcdialogs.h"
#include "server/database/table-npcdialoganswers.h"
#include "server/database/table-npcentities.h"
#include "server/database/table-characters.h"

void QuestHandler::SendDialog(Character* character, const NPCDialog* dialog)
{
  Server* server = Server::getServer();
  NPCDialogState* dia_state = character->getNPCDialogState();

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

void QuestHandler::handleNpcDialogAnswer(GenericMessage* msg)
{
  const Character* c_char = NetworkHelper::getCharacter(msg);
  if (!c_char) return;

  Character* character = c_char->getLock();

  NPCDialogState* dia_state = character->getNPCDialogState();

  NpcDialogAnswerMessage message;
  message.deserialise(msg->getByteStream());

  Server* server = Server::getServer();

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
    SendDialog(character, dialog);
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
    unsigned short sector = 0;
    float x = 0, y = 0, z = 0;
    sscanf(dialog->getText(), "%hd<%f,%f,%f>", &sector, &x, &y, &z);

    Entity* ent = character->getEntity()->getLock();
    ent->setSector(sector);
    ent->setPos(x, y, z);
    ent->freeLock();

    server->getCharacterManager()->checkForSave(ent->getPlayerEntity());

    TeleportResponseMessage telemsg;
    telemsg.setEntityId(ent->getId());
    telemsg.setPos(ent->getPos());
    telemsg.setRotation(ent->getRotation());
    telemsg.setSectorId(ent->getSector());

    ByteStream bs;
    telemsg.serialise(&bs);
    server->broadCast(bs);

    NpcEndDialogMessage endmsg;
    endmsg.setNpcId(dia_state->getNpc()->getEntity()->getId());
    ByteStream bs2;
    endmsg.serialise(&bs2);
    server->broadCast(bs2);
  }
  else if (dialog->getAction() == NPCDialog::FUNCTION)
  {
    Parse(character, dialog->getText());
  } // end ABILITYCHECK

  character->freeLock();
}

int QuestHandler::Parse(Character* character, const std::string& function)
{
  std::string func = function;
  RemoveSpaces(func);

  if (IsAtom(func))
  {
    if (func.compare("true") == 0)
      return 1;
    else if (func.compare("false") == 0)
      return 0;
    else
      return atoi(func.c_str());
  }
  else 
  {
    return Apply(character, GetOperation(func), GetArguments(func));
  }
}

int QuestHandler::Apply( Character* character,const std::string& op, const std::vector<std::string>& args)
{
  if (op.compare("?") == 0)
  {
    if (args.size() < 3) {printf("ERROR: Not enough params for operation '?'\n"); return 0;}
    if (Parse(character, args[0]))
    {
      return Parse(character, args[1]);
    }
    else
    {
      if (args.size() < 3)
        return 0;
      else
        return Parse(character, args[2]);
    }
  }
  else if (op.compare(">") == 0)
  {
    if (args.size() < 2) {printf("ERROR: Not enough params for operation '>'\n"); return 0;}
    if (Parse(character, args[0]) > Parse(character, args[1]))
      return 1;
    else
      return 0;
  }
  else if (op.compare("stat") == 0)
  {
    if (args.size() < 1) {printf("ERROR: Not enough params for operation 'stat'\n"); return 0;}
    Server* server = Server::getServer();
    Stat* stat = server->getStatManager()->findByName(ptString(args[0].c_str(), strlen(args[0].c_str())));
    if (!stat)
    {
      return 0;
    }
    return character->getStats()->getAmount(stat);
  }
  else if (op.compare("dialog") == 0)
  {
    if (args.size() < 1) {printf("ERROR: Not enough params for operation 'dialog'\n"); return 0;}
    unsigned int id = Parse(character, args[0]);
    NPCDialogState* dia_state = character->getNPCDialogState();
    const NPCDialog* dialog = dia_state->startDialog(dia_state->getNpc()->getEntity()->getId(), id);
    SendDialog(character, dialog);
  }

  return 0;
}

std::string QuestHandler::GetOperation(const std::string& function)
{
  std::string::size_type idx =  function.find_first_of(" ");
  std::string op = function.substr(1, idx-1);

  return op;
}

void QuestHandler::RemoveSpaces(std::string& function)
{
  size_t left = 0;
  for (size_t i = 0; i < function.length(); i++)
  {
    if (function[i] == ' ') left++;
    else break;
  }

  function = function.substr(left, function.length()-left);

  size_t right = function.length();
  for (size_t i = 0; i < function.length(); i++)
  {
    if (function[function.length()-1-i] == ' ') right--;
    else break;
  }

  function = function.substr(0, right);
}

std::vector<std::string> QuestHandler::GetArguments(const std::string& function)
{
  std::vector<std::string> argvec;
  
  std::string::size_type oplen = GetOperation(function).length()+1;
  std::string args = function.substr(oplen, function.length() - oplen);
  std::string::size_type right = args.find_last_of(")");
  if (right == std::string::npos) right = args.length();
  args = args.substr(0, right);

  while (args.length() > 0)
  {
    int left = 0;
    std::string::size_type idx1 = std::string::npos;
    for(size_t i = 0; i < args.length(); i++)
    {
      if ('(' == args[i])
        left++;
      else if (')' == args[i])
      {
        left--;
        if (left == 0) 
        {
          idx1 = i+1;
          break;
        }
      }
    }
    if (idx1 != std::string::npos && idx1 < args.length())
    {
      std::string arg = args.substr(0, idx1);
      RemoveSpaces(arg);
      argvec.push_back(arg);
      args = args.substr(idx1+1, args.length());
    }
    else
    {
      RemoveSpaces(args);
      argvec.push_back(args);
      break;
    }
  }

  return argvec;
}

bool QuestHandler::IsAtom(const std::string& function)
{
  std::string::size_type idx =  function.find_first_of("(");
  return idx == std::string::npos;
}

void QuestHandler::handleNpcStartDialog(GenericMessage* msg)
{
  const Character* c_char = NetworkHelper::getCharacter(msg);
  if (!c_char) return;

  Character* character = c_char->getLock();

  NPCDialogState* dia_state = character->getNPCDialogState();

  NpcStartDialogMessage message;
  message.deserialise(msg->getByteStream());

  Server* server = Server::getServer();

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
  const NPCDialog* dialog = dia_state->startDialog(npc_id);

  npc_entity->freeLock();

  if (!dialog){character->freeLock();return;}

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

  dia_state->endDialog(message.getNpcId(),0);
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

  // TODO: send "not authorized message"
  if (level == 0) return;

  SetupDialogsMessage setupmsg;
  setupmsg.deserialise(msg->getByteStream());

  CharacterTable* charactertable =
    Server::getServer()->getDatabase()->getCharacterTable();

  NpcEntitiesTable* npcentitytable =
    Server::getServer()->getDatabase()->getNpcEntitiesTable();

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
    ptString npcname = setupmsg.getNpcName(i);
    CharactersTableVO* character = charactertable->FindCharacterByName(*npcname);
    if(!character){ printf("Failed to find NPC %s\n",*npcname); continue; }
    NpcEntitiesTableVO* npc = npcentitytable->getByCharacter(character->id);
    if(!npc){ printf("Failed to find NPC %s's ID\n",*npcname); continue; }
    unsigned int dialogid = setupmsg.getDialogId(i);
    const char* text = setupmsg.getValue(i);
    ptString action = setupmsg.getAction(i);
    dialogtable->insert(npc->id, dialogid, text, *action);
  }

  for (size_t i = 0; i < setupmsg.getAnswersCount(); i++)
  {
    ptString npcname = setupmsg.getAnswerNpcName(i);
    CharactersTableVO* character = charactertable->FindCharacterByName(*npcname);
    if(!character){ printf("Failed to find NPC %s\n",*npcname); continue; }
    NpcEntitiesTableVO* npc = npcentitytable->getByCharacter(character->id);
    if(!npc){ printf("Failed to find NPC %s's ID\n",*npcname); continue; }
    unsigned int dialogid = setupmsg.getAnswerDialogId(i);
    unsigned int answerid = setupmsg.getAnswerId(i);
    const char* text = setupmsg.getAnswerText(i);
    bool end = setupmsg.getIsEndAnswer(i);
    unsigned int link = setupmsg.getAnswerLink(i);
    answertable->insert(npc->id, dialogid, answerid, text, end, link);
  }

  NPCDialogManager::getDialogManager().load();
}
