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

#ifndef _NPC_DIALOG_STATE_H_
#define _NPC_DIALOG_STATE_H_

class Character;
class NPCDialog;

class NPCDialogState
{
private:
  Character* character;
  NPCDialog* current_dialog;

public:
  NPCDialogState() {}

  void setCharacter(Character* character) { this->character = character; }

  const NPCDialog* startDialog(unsigned int dialog_id);
  void endDialog(unsigned int dialog_id);

  const NPCDialog* giveAnswer(unsigned int dialog_id, unsigned int answer_number);
};

#endif // _NPC_DIALOG_STATE_H_
