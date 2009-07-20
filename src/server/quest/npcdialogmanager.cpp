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
#include "server/database/tablemanager.h"
#include "server/database/table-npcdialogs.h"
#include "server/database/table-npcdialoganswers.h"

#include "common/util/printhelper.h"

NPCDialogManager* NPCDialogManager::self;

NPCDialogManager::~NPCDialogManager()
{
}

NPCDialog* NPCDialogManager::GetDialog(size_t npc_id, size_t dialog_id)
{
  std::vector<boost::shared_ptr<NPCDialog> >::const_iterator it;
  for (it = dialogs.begin(); it != dialogs.end(); it++)
  {
    boost::shared_ptr<NPCDialog> d = *it;
    if (d->GetNpcId() == npc_id && d->GetDialogId() == dialog_id)
      return d.get();
  }
  return 0;
}

void NPCDialogManager::LoadFromDB()
{
  // Empty list
  dialogs.clear();

  TableManager* tablemgr = Server::getServer()->GetTableManager();
  {
    // Load NPC Dialogs
    NpcDialogsTableVOArray diags = tablemgr->Get<NpcDialogsTable>()->GetAll();
    NpcDialogsTableVOArray::const_iterator it;
    for (it = diags.begin(); it != diags.end(); it++)
    {
      NpcDialogsTableVOp vo = *it;

      NPCDialog::Action action;
      if (vo->action == "text") action = NPCDialog::SHOW_TEXT;
      else if (vo->action == "sell") action = NPCDialog::START_SELL;
      else if (vo->action == "buy") action = NPCDialog::START_BUY;
      else if (vo->action == "teleport") action = NPCDialog::TELEPORT;
      else if (vo->action == "function") action = NPCDialog::FUNCTION;
      else continue;

      boost::shared_ptr<NPCDialog> dialog(new NPCDialog(vo->entity_id, vo->id, vo->text, action));
      dialogs.push_back(dialog);
    }
  }
  {
    // Load NPC Dialog Answers
    NpcDialogAnswersTableVOArray answers = tablemgr->Get<NpcDialogAnswersTable>()->GetAll();
    NpcDialogAnswersTableVOArray::const_iterator it;
    for (it = answers.begin(); it != answers.end(); it++)
    {
      NpcDialogAnswersTableVOp vo = *it;

      NPCDialog* next_dialog = 0;

      if (vo->isEnd == 0)
      {
        next_dialog = this->GetDialog(vo->entity_id, vo->nextDialog_id);
      }

      boost::shared_ptr<NPCDialogAnswer> answer(new NPCDialogAnswer(next_dialog, vo->text));
      NPCDialog* dialog = this->GetDialog(vo->entity_id, vo->dialog_id);
      if (!dialog){printf("Failed to match answer to dialog, npcid=%"SIZET", dialogid=%"SIZET"\n", vo->entity_id, vo->dialog_id);continue;} // This only happens if you edit the DB without knowing what you're doing
      dialog->AddAnswer(answer);
    }
    
  }
}

void NPCDialogManager::DeleteAll()
{
  TableManager* tablemgr = Server::getServer()->GetTableManager();
  tablemgr->Get<NpcDialogsTable>()->DeleteAll();
  tablemgr->Get<NpcDialogAnswersTable>()->DeleteAll();
  dialogs.clear();
}
