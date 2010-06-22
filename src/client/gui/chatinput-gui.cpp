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

#include "client/gui/chatinput-gui.h"

#include <CEGUI.h>
#include <CEGUIWindowManager.h>
#include <CEGUILogger.h>

#include "client/network/network.h"

#include "common/reporter/reporter.h"

namespace PT
{
  namespace GUI
  {
    namespace Windows
    {
      ChatInputWindow::ChatInputWindow(GUIManager* guimanager)
        : GUIWindow (guimanager)
      {
        windowName = CHATINPUTWINDOW ;
        sumbitEventSubscriber = 0;
      } // end ChatInputWindow()

      ChatInputWindow::~ChatInputWindow()
      {
        if (sumbitEventSubscriber)
          delete sumbitEventSubscriber;
      } // end ~ChatInputWindow()

      void ChatInputWindow::HideWindow()
      {
        winMgr->getWindow("Chatinput/Frame")->setVisible(false);
        visible = false;
      } // end HideWindow()

      void ChatInputWindow::ShowWindow()
      {
        visible = true;
      } // end ShowWindow()

      void ChatInputWindow::SetSubmitEvent(CEGUI::SlotFunctorBase* subscriber)
      {
        sumbitEventSubscriber = subscriber;

        // set up submitting message on enter
        CEGUI::Window* btn = winMgr->getWindow("Chatinput/InputBox");
        if (btn) btn->subscribeEvent(CEGUI::Editbox::EventKeyDown,
            sumbitEventSubscriber);
      } // end SetSubmitEvent()

      bool ChatInputWindow::OnDropList(const CEGUI::EventArgs& e)
      {
        Report(PT::Debug, "success.");
        return true;
      } // end OnDropList()

      bool ChatInputWindow::OnRootKeyDown(const CEGUI::EventArgs& e)
      {
        using namespace CEGUI;

        const KeyEventArgs& keyArgs = static_cast<const KeyEventArgs&>(e);

        Report(PT::Debug, "ChatInputWindow::OnRootKeyDown: key %d pressed.", keyArgs.scancode);

        switch (keyArgs.scancode)
        {
        case Key::Return:
          if (!visible) return false;
         // winMgr->getWindow("Chatinput/Frame")->setVisible(false);
          return true;
          break;

        default: return false;
        }

        return false;
      } // end OnRootKeyDown()

      void ChatInputWindow::CreateDropList()
      {
        btn = winMgr->getWindow("Chatinput/ChatDropList");
        CEGUI::ListboxItem* charIdItem =
          new CEGUI::ListboxTextItem((CEGUI::utf8*)"Say /s", 0);
        ((CEGUI::Combobox*)btn)->addItem(charIdItem);
        charIdItem = new CEGUI::ListboxTextItem((CEGUI::utf8*)"Shout /h", 1);
        ((CEGUI::Combobox*)btn)->addItem(charIdItem);
        charIdItem = new CEGUI::ListboxTextItem((CEGUI::utf8*)"Whisper /w", 1);
        ((CEGUI::Combobox*)btn)->addItem(charIdItem);
        ((CEGUI::Combobox*)btn)->setReadOnly(true);

        ((CEGUI::Combobox*)btn)->setItemSelectState(charIdItem, true);
      } // end CreateDropList()

      void ChatInputWindow::AddChatMessage(const char* nick, const char* msg)
      {
        CEGUI::MultiLineEditbox* chatlog = static_cast<CEGUI::MultiLineEditbox*>
          (winMgr->getWindow("Chatlog/Chatlog"));
        CEGUI::String nickstr = (CEGUI::String)(nick);
        CEGUI::String new_text = (CEGUI::String)(msg);
        if (!new_text.empty())
        {
          nickstr = "<"+nickstr+"> ";
          // append newline to this entry
          new_text += '\n';
          // append new text to history output
          chatlog->setText(chatlog->getText() + nickstr + new_text);
          // scroll to bottom of history output
          chatlog->setCaratIndex((size_t)-1);
        }
      } // end AddChatMessage()

      void ChatInputWindow::AddMessage(const char* msg)
      {
        CEGUI::MultiLineEditbox* chatlog = static_cast<CEGUI::MultiLineEditbox*>
          (winMgr->getWindow("Chatlog/Chatlog"));
        CEGUI::String new_text = (CEGUI::String)(msg);
        if (!new_text.empty())
        {
          // append newline to this entry
          new_text += '\n';
          // append new text to history output
          chatlog->setText(chatlog->getText() + new_text);
          // scroll to bottom of history output
          chatlog->setCaratIndex((size_t)-1);
        }
      } // end AddMessage()

      bool ChatInputWindow::Create()
      {
        ReloadWindow();
        return true;
      } // end Create()

      bool ChatInputWindow::ReloadWindow()
      {
        window = GUIWindow::LoadLayout ("client/chatinput.layout");
        GUIWindow::AddToRoot(window);
        winMgr = cegui->GetWindowManagerPtr ();

        CreateDropList();

        btn = winMgr->getWindow("Chatinput/ChatDropList");
        btn->subscribeEvent(CEGUI::Combobox::EventListSelectionAccepted,
          CEGUI::Event::Subscriber(&ChatInputWindow::OnDropList, this));
/*
        // input box enter button behaviour
        btn = winMgr->getWindow("Chatinput/InputBox");
        btn->subscribeEvent(CEGUI::Window::EventKeyDown,
          CEGUI::Event::Subscriber(&ChatInputWindow::OnRootKeyDown, this));
*/
        HideWindow();
        return true;
      } // end ReloadWindow()

    } // Windows namespace
  } // GUI namespace
} // PT namespace

