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

#include "server/server.h"
#include "server/database/database.h"
#include "server/database/table-npcdialogs.h"
#include "server/database/table-npcdialoganswers.h"

NPCDialogManager* NPCDialogManager::self;

void NPCDialogManager::addDialog(NPCDialog* dialog)
{
  if (dialog->getDialogId() == dialogs.getCount())
  {
    dialogs.add(dialog);
  }
  else
  {
    printf("Error: Dialog out of order!\n");
  }
}

NPCDialog* NPCDialogManager::getDialog(unsigned int dialog_id)
{
  if (dialog_id >= dialogs.getCount()) return 0;

  return dialogs.get(dialog_id);
}

void NPCDialogManager::load()
{
  // Empty list
  dialogs.delAll();

  Database* db = Server::getServer()->getDatabase();

  // Load NPC Dialogs
  Array<NpcDialogsTableVO*> dialogs = db->getNpcDialogsTable()->getAll();
  for (size_t i=0; i<dialogs.getCount(); i++)
  {
    NpcDialogsTableVO* vo = dialogs.get(i);

    NPCDialog::Action action;
    if (vo->action == ptString("text", 4)) action = NPCDialog::SHOW_TEXT;
    else if (vo->action == ptString("sell", 4)) action = NPCDialog::START_BUY;
    else if (vo->action == ptString("buy", 3)) action = NPCDialog::START_SELL;
    else if (vo->action == ptString("teleport", 8)) action = NPCDialog::TELEPORT;
    else continue;

    NPCDialog* dialog = new NPCDialog(vo->dialogid, vo->isstart != 0, vo->text.c_str(), action);
    this->addDialog(dialog);

    delete vo;
  }

  // Load NPC Dialog Answers
  Array<NpcDialogAnswersTableVO*> answers = db->getNpcDialogAnswersTable()->getAll();
  for (size_t i=0; i<answers.getCount(); i++)
  {
    NpcDialogAnswersTableVO* vo = answers.get(i);

    NPCDialog* next_dialog = 0;

    if (vo->isend == 0)
    {
      next_dialog = this->getDialog(vo->nextdialogid);
    }

    NPCDialogAnswer* answer = new NPCDialogAnswer(next_dialog, vo->text.c_str());
    NPCDialog* dialog = this->getDialog(vo->dialogid);
    dialog->addAnswer(answer);

    delete vo;
  }
}

