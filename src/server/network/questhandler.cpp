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

#include "common/quest/npcdialog.h"
#include "common/quest/npcdialoganswer.h"

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
    character->freeLock();
    return;
  }

  if (dialog->getAction() == NPCDialog::SHOW_TEXT)
  {
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
  }
  else if (dialog->getAction() == NPCDialog::START_BUY)
  {
    const NpcEntity* c_npc = dia_state->getNpc();
    if (c_npc)
    {
      // hardcoded! just to test it!
      // All this should go to the trade handler anyway!
      TradeOffersListNpcMessage trade_msg;
      trade_msg.setOffersCount(2);
      trade_msg.setItemId(0, 4);
      trade_msg.setPrice(0, 200);
      trade_msg.setItemId(1, 3);
      trade_msg.setPrice(1, 100);

      ByteStream bs;
      trade_msg.serialise(&bs);

      NetworkHelper::sendMessage(character, bs);
    }
  }
  else if (dialog->getAction() == NPCDialog::START_SELL)
  {
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

  NpcEntity* npc_entity = npc_ent->getNpcEntity()->getLock();

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
  character->freeLock();
}
