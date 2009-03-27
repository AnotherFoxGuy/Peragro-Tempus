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

#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>

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

  typedef std::vector<boost::shared_ptr<NPCDialogAnswer> > Answers;

private:
  friend class NPCDialogManager;

  size_t npc_id;
  size_t dialog_id;
  std::string text;
  Action action;
  Answers answers;

public:
  NPCDialog(size_t npc_id, size_t dialog_id, const std::string& text, Action a)
  : npc_id(npc_id), dialog_id(dialog_id), text(text), action(a)
  {
  }

  ~NPCDialog() { }

  size_t GetNpcId() const { return npc_id; }
  size_t GetDialogId() const { return dialog_id; }

  void AddAnswer(boost::shared_ptr<NPCDialogAnswer> answer) { answers.push_back(answer); }

  const std::string& GetText() const { return text; }

  size_t GetAnswerCount() const { return answers.size(); }
  NPCDialogAnswer* GetAnswer(size_t id) const { return answers[id].get(); }

  void SetAction(Action a) { action = a; }
  Action GetAction() const { return action; }
};

#endif // NPC_DIALOG_H
