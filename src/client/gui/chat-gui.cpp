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
#include "client/entity/entitymanager.h"

#include "common/reporter/reporter.h"

namespace PT
{
  namespace GUI
  {
    namespace Windows
    {
      ChatWindow::ChatWindow(GUIManager* guimanager)
        : GUIWindow (guimanager)
      {
        windowName = CHATWINDOW;
        sumbitEventSubscriber = 0;
      } // end ChatWindow()

      ChatWindow::~ChatWindow()
      {
        if (sumbitEventSubscriber)
          delete sumbitEventSubscriber;
      } // end ~ChatWindow()

      void ChatWindow::HideWindow()
      {
        winMgr->getWindow("Chatlog/Frame")->setVisible(false);
        winMgr->getWindow("InputPanel/Frame")->setVisible(false);

        visible = false;
      } // end HideWindow()

      void ChatWindow::ShowWindow()
      {
        winMgr->getWindow("Chatlog/Frame")->setVisible(true);
        //winMgr->getWindow("InputPanel/Frame")->setVisible(true);

        visible = true;
      } // end ShowWindow()

      void ChatWindow::SetSubmitEvent(CEGUI::SlotFunctorBase* subscriber)
      {
        sumbitEventSubscriber = subscriber;

        // set up submitting message on enter
        CEGUI::Window* btn = winMgr->getWindow("InputPanel/InputBox");
        if (btn) btn->subscribeEvent(CEGUI::Editbox::EventKeyDown,
            sumbitEventSubscriber);
      } // end SetSubmitEvent()

      bool ChatWindow::OnDropList(const CEGUI::EventArgs& e)
      {
        Report(PT::Debug, "success.");
        return true;
      } // end OnDropList()

      bool ChatWindow::OnSlider(const CEGUI::EventArgs& e)
      {
        using namespace CEGUI;

        // we know it's a slider.
        Slider* s =
          static_cast<Slider*>(static_cast<const WindowEventArgs&>(e).window);

        // get value from slider and set it as the current alpha
        float val = s->getCurrentValue();
        winMgr->getWindow("Chatlog/ChatlogWidget")->setAlpha(val);

        // indicate the event was handled here.
        return true;
      } // end OnSlider()

      bool ChatWindow::OnRootKeyDown(const CEGUI::EventArgs& e)
      {
        using namespace CEGUI;

        const KeyEventArgs& keyArgs = static_cast<const KeyEventArgs&>(e);

        //Report(PT::Debug, "OnRootKeyDown: key %d pressed.", keyArgs.scancode);

        switch (keyArgs.scancode)
        {
        case Key::Return:
          if (!visible) return false;
          winMgr->getWindow("InputPanel/Frame")->setVisible(true);
          winMgr->getWindow("InputPanel/InputBox")->activate();
          return true;
          break;

        default: return false;
        }

        return false;
      } // end OnRootKeyDown()

      void ChatWindow::CreateDropList()
      {
        btn = winMgr->getWindow("InputPanel/ChatDropList");
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

      void ChatWindow::AddChatMessage(const char* nick, const char* msg)
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

      void ChatWindow::AddMessage(const char* msg)
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

      bool ChatWindow::Create()
      {
        window = GUIWindow::LoadLayout ("client/chat.xml");
        GUIWindow::AddToRoot(window);
        window = GUIWindow::LoadLayout ("client/chatlog.xml");
        GUIWindow::AddToRoot(window);
        winMgr = cegui->GetWindowManagerPtr ();

        CreateDropList();

        winMgr->getWindow("Chatlog/ChatlogWidget")->setAlpha(0.3f);

        CEGUI::Slider* slider =
          static_cast<CEGUI::Slider*>(winMgr->getWindow("Chatlog/Slider"));
        // set up slider config
        slider->setCurrentValue(0.3f);
        slider->setClickStep(0.1f);
        slider->subscribeEvent(CEGUI::Slider::EventValueChanged,
          CEGUI::Event::Subscriber(&ChatWindow::OnSlider, this));

        // input box enter button behaviour
        btn = winMgr->getWindow("Root");
        btn->subscribeEvent(CEGUI::Window::EventKeyDown,
          CEGUI::Event::Subscriber(&ChatWindow::OnRootKeyDown, this));

        btn = winMgr->getWindow("InputPanel/ChatDropList");
        btn->subscribeEvent(CEGUI::Combobox::EventListSelectionAccepted,
          CEGUI::Event::Subscriber(&ChatWindow::OnDropList, this));

        HideWindow();

        return true;
      } // end Create()

    } // Windows namespace
  } // GUI namespace
} // PT namespace

