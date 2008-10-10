/*
    Copyright (C) 2008 Development Team of Peragro Tempus

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

#include "questmanager.h"

#include "client/pointer/pointer.h"
#include "common/reporter/reporter.h"

#include "client/gui/gui.h"
#include "client/gui/guimanager.h"

#include "common/event/eventmanager.h"


namespace PT
{
  namespace Quest
  {
    QuestManager::QuestManager()
    {
    }

    QuestManager::~QuestManager()
    {
    }

    bool QuestManager::Initialize()
    {
      SETUP_HANDLER
      REGISTER_LISTENER(QuestManager, NPCDialog, "quest.dialog.npc.add")
      REGISTER_LISTENER(QuestManager, EndNPCDialog, "quest.dialog.npc.end")

      return true;
    } // end Initialize()

    bool QuestManager::NPCDialog(iEvent& ev)
    {
      using namespace PT::GUI;
      using namespace PT::GUI::Windows;
      using namespace Events;

      PT::GUI::GUIManager* guimanager = PointerLibrary::getInstance()->getGUIManager();
      if (!guimananger) return false;
      NpcDialogWindow* npcWindow = guimanager->GetWindow<NpcDialogWindow>(NPCDIALOGWINDOW);
      if (!npcWindow) return false;

      unsigned int dialogId = Helper::GetUInt(ev, "dialogId");
      std::string dialogText = Helper::GetString(ev, "dialogText");

      npcWindow->AddDialog(dialogId, dialogText);

      csRef<iEvent> list;
      if (ev.Retrieve("answersList", list) == csEventErrNone)
      {
        csRef<iEventAttributeIterator> answers = list->GetAttributeIterator();
        while (answers->HasNext())
        {
          csRef<iEvent> item; list->Retrieve(answers->Next(), item);
          unsigned int answerId = Helper::GetUInt(item, "answerId");
          std::string answerText = Helper::GetString(item, "answerText");
          npcWindow->AddAnswer(answerId, answerText);
        } // end while
      }
      else
        Report(PT::Error, "PlayerInventory failed to get inventory list!");

      return true;
    } // end NPCDialog()

    bool QuestManager::EndNPCDialog(iEvent& /*ev*/)
    {
      using namespace PT::GUI;
      using namespace PT::GUI::Windows;

      GUIManager* guimanager = PointerLibrary::getInstance()->getGUIManager();
      if (!guimananger) return false;
      NpcDialogWindow* npcWindow = guimanager->GetWindow<NpcDialogWindow>(NPCDIALOGWINDOW);
      if (!npcWindow) return false;
      npcWindow->HideWindow();

      return true;
    } // end NPCDialog()

  } // Quest namespace
} // PT namespace
