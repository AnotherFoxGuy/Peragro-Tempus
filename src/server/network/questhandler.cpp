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

#include "server/network/network.h"
#include "server/network/connection.h"
#include "server/user.h"

#include "common/entity/character.h"
#include "common/entity/pcentity.h"

#include "common/quest/npcdialog.h"
#include "common/quest/npcdialoganswer.h"

void QuestHandler::handleNpcDialogAnswer(GenericMessage* msg)
{
  Connection* conn = msg->getConnection();
  if (!conn) return;

  User* user = conn->getUser();
  if (!user) return;

  PcEntity* pc = user->getEntity();
  if (!pc) return;

  Character* character = pc->getCharacter();
  if (!character) return;

  NPCDialogState* dia_state = character->getNPCDialogState();

  NpcDialogAnswerMessage message;
  message.deserialise(msg->getByteStream());

  const NPCDialog* dialog = dia_state->giveAnswer(message.getDialogId(), message.getAnswerId());

  if (dialog == 0) return;

  NpcDialogMessage dialog_msg;
  dialog_msg.setDialogId(dialog->getDialogId());
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
  if (conn) conn->send(bs);
}

void QuestHandler::handleNpcStartDialog(GenericMessage* msg)
{
  Connection* conn = msg->getConnection();
  if (!conn) return;

  User* user = conn->getUser();
  if (!user) return;

  PcEntity* pc = user->getEntity();
  if (!pc) return;

  Character* character = pc->getCharacter();
  if (!character) return;

  NPCDialogState* dia_state = character->getNPCDialogState();

  NpcStartDialogMessage message;
  message.deserialise(msg->getByteStream());

  const NPCDialog* dialog = dia_state->startDialog(0);

  NpcDialogMessage dialog_msg;
  dialog_msg.setDialogId(dialog->getDialogId());
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
  if (conn) conn->send(bs);
}

void QuestHandler::handleNpcEndDialog(GenericMessage* msg)
{
  Connection* conn = msg->getConnection();
  if (!conn) return;

  User* user = conn->getUser();
  if (!user) return;

  PcEntity* pc = user->getEntity();
  if (!pc) return;

  Character* character = pc->getCharacter();
  if (!character) return;

  NPCDialogState* dia_state = character->getNPCDialogState();

  NpcEndDialogMessage message;
  message.deserialise(msg->getByteStream());

  dia_state->endDialog(0);
}
