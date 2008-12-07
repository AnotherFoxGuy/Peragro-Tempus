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

#include "client/gui/gui.h"

#include "CEGUI.h"
#include "CEGUIWindowManager.h"
#include "CEGUILogger.h"

#include "client/network/network.h"
#include "client/gui/guimanager.h"

#include "common/reporter/reporter.h"

namespace PT
{
  namespace GUI
  {
    namespace Windows
    {
      NpcDialogWindow::NpcDialogWindow(GUIManager* guimanager)
        : GUIWindow (guimanager)
      {
        windowName = NPCDIALOGWINDOW;
        dialogId = -1;
        newDialog = false;
      } // end NpcDialogWindow()

      NpcDialogWindow::~NpcDialogWindow()
      {
      } // end ~NpcDialogWindow()

      bool NpcDialogWindow::OnCloseButton(const CEGUI::EventArgs& args)
      {
        GUIWindow::HideWindow();

        // End the dialog.
        NpcEndDialogMessage msg;
        network->send(&msg);

        return true;
      } // end OnCloseButton()

      bool NpcDialogWindow::OnAnswer(const CEGUI::EventArgs& args)
      {
        btn = winMgr->getWindow("NpcDialog/Answers");

        if (((CEGUI::Listbox*)btn)->getSelectedCount() == 0)
          return true;

        CEGUI::ListboxItem* answerItem =
          ((CEGUI::Listbox*)btn)->getFirstSelectedItem();

        if (!answerItem->isSelected()) return true;

        uint answer_id = answerItem->getID();

        Report(PT::Debug, "NpcDialogWindow: Answered dialog %d with answer %d.",
          dialogId, answer_id);

        if (answer_id == 0xff)
        {
          GUIWindow::HideWindow();
          return true;
        }

        NpcDialogAnswerMessage answer_msg;
        answer_msg.setDialogId(dialogId);
        answer_msg.setAnswerId(answer_id);
        network->send(&answer_msg);

        return true;
      } // end OnAnswer()

      void NpcDialogWindow::ClearAnswers()
      {
        // Clear the previous answers.
        btn = winMgr->getWindow("NpcDialog/Answers");
        ((CEGUI::Listbox*)btn)->resetList();
      } // end ClearAnswers()

      void NpcDialogWindow::SetName(const std::string& name)
      {
        // Set the dialog text.
        btn = winMgr->getWindow("NpcDialog/Frame");
        btn->setText(name.c_str());
      } // end SetName()

      void NpcDialogWindow::AddDialog(unsigned int dialogId, const std::string& dialog)
      {
        // Set the dialog text.
        btn = winMgr->getWindow("NpcDialog/Dialog");
        btn->setText(dialog.c_str());
        this->dialogId = dialogId;

        ClearAnswers();

        AddAnswer(0xff, "Goodbye.");

        GUIWindow::ShowWindow();
        GUIWindow::EnableWindow();

        newDialog = true;
      } // end AddDialog()

      void NpcDialogWindow::AddAnswer(unsigned int number, const std::string& answer)
      {
        if (newDialog)
        {
          ClearAnswers();
          newDialog = false;
        }

        btn = winMgr->getWindow("NpcDialog/Answers");
        CEGUI::ListboxItem* answerItem =
          new CEGUI::ListboxTextItem(answer.c_str(), number);

        ((CEGUI::Listbox*)btn)->addItem(answerItem);
      } // end AddAnswer()

      bool NpcDialogWindow::Create()
      {
        ReloadWindow();
        return true;
      } // end Create()

      bool NpcDialogWindow::ReloadWindow()
      {
        window = GUIWindow::LoadLayout ("client/npcdialog.xml");
        GUIWindow::AddToRoot(window);

        winMgr = cegui->GetWindowManagerPtr ();

        // Get the frame window
        CEGUI::FrameWindow* frame = static_cast<CEGUI::FrameWindow*>
          (winMgr->getWindow("NpcDialog/Frame"));
        frame->subscribeEvent(CEGUI::FrameWindow::EventCloseClicked,
          CEGUI::Event::Subscriber(&NpcDialogWindow::OnCloseButton, this));

        // Get the frame window
        CEGUI::Listbox* answers = static_cast<CEGUI::Listbox*>
          (winMgr->getWindow("NpcDialog/Answers"));
        answers->setMultiselectEnabled(false);
        answers->subscribeEvent(CEGUI::Listbox::EventSelectionChanged,
          CEGUI::Event::Subscriber(&NpcDialogWindow::OnAnswer, this));

        return true;
      } // end ReloadWindow()

    } // Windows namespace
  } // GUI namespace
} // PT namespace

