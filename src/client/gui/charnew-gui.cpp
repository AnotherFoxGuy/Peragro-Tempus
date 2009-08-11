/*
    Copyright (C) 2009 Development Team of Peragro Tempus

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
#include "charnew-gui.h"

#include "client/user/usermanager.h"

#include "CEGUI.h"
#include "CEGUIWindowManager.h"
#include "CEGUILogger.h"

#include "client/network/network.h"
#include "client/gui/guimanager.h"

#include "common/reporter/reporter.h"
#include "client/pointer/pointer.h"

namespace PT
{
  namespace GUI
  {
    namespace Windows
    {
      CreateCharWindow::CreateCharWindow(GUIManager* guimanager)
        : GUIWindow (guimanager)
      {
         windowName = CREATECHARWINDOW;
      } // end CreateCharWindow()

      CreateCharWindow::~CreateCharWindow()
      {
      } // end ~CreateCharWindow()


      bool CreateCharWindow::CreateButton(const CEGUI::EventArgs& e)
      {
        CreateCharacter(e);
        ToggleNewWindow(false);
        return true;
      } // end CreateButton()

      bool CreateCharWindow::CancelButton(const CEGUI::EventArgs& e)
      {
        ToggleNewWindow(false);
        return true;
      } // end CancelButton()

      bool CreateCharWindow::OnAvatarTemplateSelect(const CEGUI::EventArgs& e)
      {
        btn = winMgr->getWindow("CharNew/AvatarComboBox");
        CEGUI::ListboxItem* item = ((CEGUI::Combobox*)btn)->getSelectedItem();

        AvatarInfoRequestMessage infoMsg;
        infoMsg.setAvatarId(item->getID());
        PointerLibrary::getInstance()->getNetwork()->send(&infoMsg);

        return true;
      } // end OnAvatarTemplateSelect()

      CEGUI::String CreateCharWindow::GetNewCharName()
      {
        return winMgr->getWindow("CharNew/NickNameEditBox")->getText();
      } // end CreateCharWindow()

      void CreateCharWindow::ClearNewCharName()
      {
        winMgr->getWindow("CharNew/NickNameEditBox")->setText("");
      } // end ClearNewCharName()

      bool CreateCharWindow::CreateCharacter(const CEGUI::EventArgs& e)
      {
        CEGUI::String NewCharName = GetNewCharName();
        ClearNewCharName();

        btn = winMgr->getWindow("CharNew/AvatarComboBox");
        CEGUI::ListboxItem* item = ((CEGUI::Combobox*)btn)->getSelectedItem();

        if (NewCharName.empty()) return true;
        CharCreateRequestMessage answer_msg;
        answer_msg.setName(ptString(NewCharName.c_str(), NewCharName.length()));
        answer_msg.setAvatarTemplateID(item->getID());
        answer_msg.setSkinColour(0xC9, 0xBE, 0x89);
        answer_msg.setHairColour(0x4B, 0x41, 0x13);
        answer_msg.setDecalColour(0xFF, 0x00, 0x00);
        network->send(&answer_msg);

        return true;
      } // end NewChar()

      bool CreateCharWindow::Create()
      {
        ReloadWindow();
        return true;
      } // end Create()

      bool CreateCharWindow::ReloadWindow()
      {
        window = GUIWindow::LoadLayout ("client/charnew.layout");
        GUIWindow::AddToRoot(window);

        // Register the button events.

        btn = winMgr->getWindow("CharNew/Cancel");
        btn->subscribeEvent(CEGUI::PushButton::EventClicked,
          CEGUI::Event::Subscriber(&CreateCharWindow::CancelButton, this));

        btn = winMgr->getWindow("CharNew/Ok");
        btn->subscribeEvent(CEGUI::PushButton::EventClicked,
          CEGUI::Event::Subscriber(&CreateCharWindow::CreateButton, this));

        // Setup model list
        btn = winMgr->getWindow("CharNew/AvatarComboBox");
        ((CEGUI::Combobox*)btn)->setReadOnly (true);
        btn->subscribeEvent(CEGUI::Combobox::EventListSelectionAccepted,
          CEGUI::Event::Subscriber(&CreateCharWindow::OnAvatarTemplateSelect, this));

        return true;
      } // end ReloadWindow()

      void CreateCharWindow::ShowWindow()
      {
        btn = winMgr->getWindow(CREATECHARWINDOW);
        btn->setVisible(true);
        btn->activate();
      }

      void CreateCharWindow::EmptyAvatarList()
      {
        btn = winMgr->getWindow("CharNew/AvatarComboBox");
        static_cast<CEGUI::Combobox*>(btn)->resetList();
      } // end EmptyModelList()

      void CreateCharWindow::AddModel(unsigned int avatarId, const char* avatarName)
      { 
        btn = winMgr->getWindow("CharNew/AvatarComboBox");
        CEGUI::ListboxItem* avatarItem = new CEGUI::ListboxTextItem(avatarName,avatarId);
        avatarItem->setSelectionBrushImage((CEGUI::utf8*)"Peragro",
          (CEGUI::utf8*)"TextSelectionBrush");
        static_cast<CEGUI::Combobox*>(btn)->addItem(avatarItem); 

      } // end AddModel()


      void CreateCharWindow::SetInfoTextBox(const std::string infoText )
      {
        btn = winMgr->getWindow("CharNew/InfoEditBox");
	btn->setText( reinterpret_cast<const CEGUI::utf8*>(infoText.c_str()) ); 
      }

      void CreateCharWindow::ToggleNewWindow(bool visible)
      {
        btn = winMgr->getWindow("CharNew/Frame");
        btn->setVisible(visible);
        btn = winMgr->getWindow("CharSelect/Frame");
        btn->setVisible(!visible);
      } // end ToggleNewWindow()

    } // Windows namespace
  } // GUI namespace
} // PT namespace

