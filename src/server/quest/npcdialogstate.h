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

#ifndef NPC_DIALOG_STATE_H
#define NPC_DIALOG_STATE_H

#include "common/util/monitor.h"

class Character;
class NpcEntity;
class NPCDialog;

class NPCDialogState
{
private:
  Character* character;
  NpcEntity* npc;

  NPCDialog* current_dialog;

public:
  NPCDialogState() {}

  void SetCharacter(Character* character);

  /**
   *Start a dialog between the player and an NPC.
   *@param npc_id Which NPC to start the dialog with
   */
  const NPCDialog* StartDialog(unsigned int npc_id, unsigned int dialog_id = 1);
  /**
   *End a dialog between the player and an NPC.
   *@param npc_id ID of the NPC which the dialog is with.
   *@param dialog_id ID of the dialog you are on.
   */
  void EndDialog(unsigned int npc_id, unsigned int dialog_id);

  const NPCDialog* GiveAnswer(unsigned int dialog_id, unsigned int answer_number);

  void SetNpc(NpcEntity* npc);
  NpcEntity* GetNpc() { return npc; }
};

#endif // NPC_DIALOG_STATE_H
