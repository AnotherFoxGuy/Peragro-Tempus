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

#ifndef NPC_DIALOG_H
#define NPC_DIALOG_H

#include "common/util/array.h"
#include <string>

class NPCDialogAnswer;
class NPCDialogManager;

class NPCDialog
{
public:
  enum Action
  {
    SHOW_TEXT = 0,
    START_BUY = 1,
    START_SELL = 2,
    TELEPORT = 3,
    FUNCTION = 4
  };

private:
  friend class NPCDialogManager;

  size_t npc_id;
  size_t dialog_id;

  std::string text;

  Array<NPCDialogAnswer*> answers;

  Action action;

public:
  NPCDialog(size_t npc_id, size_t dialog_id, const char* text, Action a)
  : npc_id(npc_id), dialog_id(dialog_id), text(text), answers(0,1), action(a)
  {
  }

  ~NPCDialog() { answers.delAll(); }

  size_t getNpcId() const { return npc_id; }
  size_t getDialogId() const { return dialog_id; }

  void addAnswer(NPCDialogAnswer* answer) { answers.add(answer); }

  const char* getText() const { return text.c_str(); }

  size_t getAnswerCount() const { return answers.getCount(); }
  NPCDialogAnswer*  getAnswer(size_t idx) const { return answers.get(idx); }

  void setAction(Action a) { action = a; }
  Action getAction() const { return action; }
};

#endif // NPC_DIALOG_H
