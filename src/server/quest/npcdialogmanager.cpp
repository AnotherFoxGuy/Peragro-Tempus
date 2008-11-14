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
#include "common/database/database.h"
#include "server/database/tables.h"
#include "server/database/table-npcdialogs.h"
#include "server/database/table-npcdialoganswers.h"

NPCDialogManager* NPCDialogManager::self;

NPCDialogManager::~NPCDialogManager()
{
  dialogs.delAll();
}

NPCDialog* NPCDialogManager::getDialog(unsigned int npc_id, unsigned int dialog_id)
{
  for(unsigned int i=0; i<dialogs.getCount(); i++)
  {
    if (dialogs[i]->getNpcId() == npc_id && dialogs[i]->getDialogId() == dialog_id){return dialogs[i];}
  }
  return 0;
}

void NPCDialogManager::load()
{
  // Empty list
  dialogs.delAll();

  Tables* tables = Server::getServer()->getTables();

  // Load NPC Dialogs
  Array<NpcDialogsTableVO*> dialogs = tables->getNpcDialogsTable()->getAll();
  for (size_t i=0; i<dialogs.getCount(); i++)
  {
    NpcDialogsTableVO* vo = dialogs.get(i);

    NPCDialog::Action action;
    if (vo->action == ptString("text", 4)) action = NPCDialog::SHOW_TEXT;
    else if (vo->action == ptString("sell", 4)) action = NPCDialog::START_SELL;
    else if (vo->action == ptString("buy", 3)) action = NPCDialog::START_BUY;
    else if (vo->action == ptString("teleport", 8)) action = NPCDialog::TELEPORT;
    else if (vo->action == ptString("function", strlen("function"))) action = NPCDialog::FUNCTION;
    else continue;

    NPCDialog* dialog = new NPCDialog(vo->npcid, vo->dialogid, vo->text.c_str(), action);
    this->dialogs.add(dialog);

    delete vo;
  }

  // Load NPC Dialog Answers
  Array<NpcDialogAnswersTableVO*> answers = tables->getNpcDialogAnswersTable()->getAll();
  for (size_t i=0; i<answers.getCount(); i++)
  {
    NpcDialogAnswersTableVO* vo = answers.get(i);

    NPCDialog* next_dialog = 0;

    if (vo->isend == 0)
    {
      next_dialog = this->getDialog(vo->npcid, vo->nextdialogid);
    }

    NPCDialogAnswer* answer = new NPCDialogAnswer(next_dialog, vo->text.c_str());
    NPCDialog* dialog = this->getDialog(vo->npcid, vo->dialogid);
    if (!dialog){printf("Failed to match answer to dialog, npcid=%i, dialogid=%i\n", vo->npcid, vo->dialogid);continue;} // This only happens if you edit the DB without knowing what you're doing
    dialog->addAnswer(answer);

    delete vo;
  }
}

void NPCDialogManager::delAll()
{
  Tables* tables = Server::getServer()->getTables();
  tables->getNpcDialogsTable()->removeAll();
  tables->getNpcDialogAnswersTable()->removeAll();
  dialogs.delAll();
}
