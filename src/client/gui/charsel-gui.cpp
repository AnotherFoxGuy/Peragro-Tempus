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

#include "client/user/usermanager.h"

#include "CEGUI.h"
#include "CEGUIWindowManager.h"
#include "CEGUILogger.h"

#include "client/network/network.h"
#include "client/gui/guimanager.h"
#include "client/entity/entitymanager.h"

#include "common/reporter/reporter.h"
#include "client/pointer/pointer.h"

namespace PT
{
  namespace GUI
  {
    namespace Windows
    {
      SelectCharWindow::SelectCharWindow(GUIManager* guimanager)
        : GUIWindow (guimanager)
      {
         windowName = SELECTCHARWINDOW;
      } // end SelectCharWindow()

      SelectCharWindow::~SelectCharWindow()
      {
      } // end ~SelectCharWindow()

      bool SelectCharWindow::SelectChar(const CEGUI::EventArgs& e)
      {
        btn = winMgr->getWindow("CharSelect/Characters");
        if (((CEGUI::MultiColumnList*)btn)->getSelectedCount() == 0)
          return true;

        CEGUI::ListboxItem* item =
          ((CEGUI::MultiColumnList*)btn)->getFirstSelectedItem();

        if (!item->isSelected()) return true;

        int own_char_id = atoi(item->getText().c_str());

        PointerLibrary::getInstance()->getUserManager()->
          SelectCharacter(own_char_id);

        // Remove the window from view.
        btn = winMgr->getWindow(SELECTCHARWINDOW);
        btn->setVisible(false);

        return true;
      } // end SelectChar()

      bool SelectCharWindow::OnSelection(const CEGUI::EventArgs& e)
      {
        btn = winMgr->getWindow("CharSelect/Characters");
        if (((CEGUI::MultiColumnList*)btn)->getSelectedCount() == 0)
          return true;

        CEGUI::ListboxItem* item =
         ((CEGUI::MultiColumnList*)btn)->getFirstSelectedItem();

        if (!item->isSelected()) return true;

        // Do something, if needed.

        return true;
      } // end OnSelection()

      bool SelectCharWindow::NewChar(const CEGUI::EventArgs& e)
      {
        CEGUI::String NewCharName = GetNewCharName();
        ClearNewCharName();
        if (NewCharName.empty()) return true;
        CharCreateRequestMessage answer_msg;
        answer_msg.setName(ptString(NewCharName.c_str(), NewCharName.length()));
        answer_msg.setSkinColour(0xC9, 0xBE, 0x89);
        answer_msg.setHairColour(0x4B, 0x41, 0x13);
        answer_msg.setDecalColour(0xFF, 0x00, 0x00);
        network->send(&answer_msg);

        ToggleNewWindow(false);

        return true;
      } // end NewChar()

      bool SelectCharWindow::CreateButton(const CEGUI::EventArgs& e)
      {
        ToggleNewWindow(true);
        return true;
      } // end CreateButton()

      bool SelectCharWindow::CancelButton(const CEGUI::EventArgs& e)
      {
        ToggleNewWindow(false);
        return true;
      } // end CancelButton()

      bool SelectCharWindow::AdminButton(const CEGUI::EventArgs& e)
      {
        btn = winMgr->getWindow("ServerSetup/Frame");
        btn->setVisible(true);
        btn = winMgr->getWindow("CharSelect/Frame");
        btn->setVisible(false);
        return true;
      } // end AdminButton()

      bool SelectCharWindow::DelChar(const CEGUI::EventArgs& e)
      {
        return true;
      } // end DelChar()

      bool SelectCharWindow::SelectionAccepted(const CEGUI::EventArgs &e)
      {
        using namespace CEGUI;

        const KeyEventArgs& keyArgs =
          static_cast<const KeyEventArgs&>(e);

        // If input is different from RightButton,
        // swallow the event and don't do anything.
        if (   (keyArgs.scancode != Key::Tab)
            && (keyArgs.scancode != Key::Return)
            && (keyArgs.scancode != Key::NumpadEnter))
        {
          return true;
        }

        SelectChar(e);
        return true;
      } // end SelectionAccepted()

      CEGUI::String SelectCharWindow::GetNewCharName()
      {
        return winMgr->getWindow("CharSelectNew/NickNameEditBox")->getText();
      } // end GetNewCharName()

      void SelectCharWindow::ClearNewCharName()
      {
        winMgr->getWindow("CharSelectNew/NickNameEditBox")->setText("");
      } // end ClearNewCharName()

      void SelectCharWindow::ToggleNewWindow(bool visible)
      {
        btn = winMgr->getWindow("CharSelectNew/Frame");
        btn->setVisible(visible);
        btn = winMgr->getWindow("CharSelect/Frame");
        btn->setVisible(!visible);
      } // end ToggleNewWindow()

      void SelectCharWindow::ShowAdminButton()
      {
        btn = winMgr->getWindow("CharSelect/Admin");
        btn->setVisible(true);
        btn = winMgr->getWindow("CharSelect/Frame");
        btn->setHeight(btn->getHeight()+CEGUI::UDim(0,28));
      } // end ShowAdminButton()

      void SelectCharWindow::AddCharacter(unsigned int charId, const char* name,
                                          unsigned char skincolour,
                                          unsigned char hairncolour,
                                          unsigned char decalcolour)
      {
        btn = winMgr->getWindow("CharSelect/Characters");
        char charIdstr[10];
        snprintf(charIdstr, 10, "%d", charId);
        CEGUI::ListboxItem* charIdItem = new CEGUI::ListboxTextItem(charIdstr);
        CEGUI::ListboxItem* charNameItem = new CEGUI::ListboxTextItem(name);

        charIdItem->setSelectionBrushImage((CEGUI::utf8*)"Peragro",
          (CEGUI::utf8*)"TextSelectionBrush");
        charNameItem->setSelectionBrushImage((CEGUI::utf8*)"Peragro",
          (CEGUI::utf8*)"TextSelectionBrush");

        unsigned int row = ((CEGUI::MultiColumnList*)btn)->addRow();
        ((CEGUI::MultiColumnList*)btn)->setItem(charIdItem, 0, row);
        ((CEGUI::MultiColumnList*)btn)->setItem(charNameItem, 1, row);
        ((CEGUI::MultiColumnList*)btn)->
          setSelectionMode(CEGUI::MultiColumnList::RowSingle);

        SelectFirstChar();
        ShowWindow();

      } // end AddCharacter()

      void SelectCharWindow::SelectFirstChar()
      {
        btn = winMgr->getWindow("CharSelect/Characters");

        CEGUI::MCLGridRef ref(0,0);

        ((CEGUI::MultiColumnList*)btn)->setItemSelectState(ref ,true);
      } // end SelectFirstChar()

      void SelectCharWindow::EmptyCharList()
      {
        btn = winMgr->getWindow("CharSelect/Characters");
        unsigned int i=0;
        while (i < ((CEGUI::MultiColumnList*)btn)->getRowCount())
        {
          ((CEGUI::MultiColumnList*)btn)->removeRow(i);
          ++i;
        }
      } // end EmptyCharList()

      bool SelectCharWindow::Create()
      {
        window = GUIWindow::LoadLayout ("client/charselect.xml");
        GUIWindow::AddToRoot(window);
        window = GUIWindow::LoadLayout ("client/charselectnew.xml");
        GUIWindow::AddToRoot(window);

        // Register the button events.
        btn = winMgr->getWindow("CharSelect/Ok");
        btn->subscribeEvent(CEGUI::PushButton::EventClicked,
          CEGUI::Event::Subscriber(&SelectCharWindow::SelectChar, this));

        btn = winMgr->getWindow("CharSelect/Frame");
        btn->subscribeEvent(CEGUI::Window::EventKeyDown,
          CEGUI::Event::Subscriber(&SelectCharWindow::SelectionAccepted, this));

        btn = winMgr->getWindow("CharSelect/Create");
        btn->subscribeEvent(CEGUI::PushButton::EventClicked,
          CEGUI::Event::Subscriber(&SelectCharWindow::CreateButton, this));

        btn = winMgr->getWindow("CharSelectNew/Cancel");
        btn->subscribeEvent(CEGUI::PushButton::EventClicked,
          CEGUI::Event::Subscriber(&SelectCharWindow::CancelButton, this));

        btn = winMgr->getWindow("CharSelectNew/Ok");
        btn->subscribeEvent(CEGUI::PushButton::EventClicked,
          CEGUI::Event::Subscriber(&SelectCharWindow::NewChar, this));

        btn = winMgr->getWindow("CharSelect/Admin");
        btn->subscribeEvent(CEGUI::PushButton::EventClicked,
          CEGUI::Event::Subscriber(&SelectCharWindow::AdminButton, this));

        btn = winMgr->getWindow("CharSelect/Characters");
        btn->subscribeEvent(CEGUI::MultiColumnList::EventSelectionChanged,
          CEGUI::Event::Subscriber(&SelectCharWindow::OnSelection, this));
        CEGUI::String str_id("Id");
        CEGUI::String str_name("Name");
        ((CEGUI::MultiColumnList*)btn)->
          addColumn(str_id,0,CEGUI::UDim(0.25f,0));
        ((CEGUI::MultiColumnList*)btn)->
          addColumn(str_name,1,CEGUI::UDim(0.75f,0));
        ((CEGUI::MultiColumnList*)btn)->
          setSelectionMode(CEGUI::MultiColumnList::RowSingle);

        return true;
      } // end Create()

      void SelectCharWindow::ShowWindow()
      {
        btn = winMgr->getWindow("CharSelect/Frame");
        btn->setVisible(true);
        btn->activate();
      }
    } // Windows namespace
  } // GUI namespace
} // PT namespace

