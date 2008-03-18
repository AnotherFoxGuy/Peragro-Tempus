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

#include "npcdialog.h"
#include "npcdialogmanager.h"
#include "npcdialoganswer.h"
#include "npcdialogstate.h"

#include "server/entity/character.h"
#include "server/entity/npcentity.h"

void NPCDialogState::setCharacter(Character* character)
{
  this->character = character->getRef(); 
}

const NPCDialog* NPCDialogState::startDialog(unsigned int npc_id)
{
  current_dialog = NPCDialogManager::getDialogManager().getDialog(npc_id, 0); // Start-dialogs are always 0
  if (!current_dialog)
  {
    current_dialog = 0;
    printf("Dialog not found! (npc_id=%u, dialog id=0)\n",npc_id);
  }
  return current_dialog;
}

void NPCDialogState::endDialog(unsigned int npc_id, unsigned int dialog_id)
{
  if (current_dialog != 0 && npc_id == current_dialog->getNpcId() && dialog_id == current_dialog->getDialogId())
  {
    current_dialog = 0;
  }
}

const NPCDialog* NPCDialogState::giveAnswer(unsigned int dialog_id, unsigned int answer_number)
{
  if (current_dialog == 0 || dialog_id != current_dialog->getDialogId()) return 0;

  if (current_dialog->getAnswerCount() <= answer_number) return 0;

  const NPCDialogAnswer* answer = current_dialog->getAnswer(answer_number);

  if (answer == 0) return 0;

  current_dialog = answer->getNextDialog();

  return answer->getNextDialog();
}

void NPCDialogState::setNpc(const NpcEntity* npc)
{
  this->npc = npc->getRef(); 
}
