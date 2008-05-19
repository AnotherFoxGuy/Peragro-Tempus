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

#include "client/reporter/reporter.h"

namespace PT
{
  namespace GUI
  {
    namespace Windows
    {

      WhisperWindow::WhisperWindow(GUIManager* guimanager)
        : GUIWindow (guimanager)
      {
        windowName = WHISPERWINDOW;
        iObjectRegistry* obj_reg =
          PointerLibrary::getInstance()->getObjectRegistry();
        vfs = csQueryRegistry<iVFS> (obj_reg);
      } // end WhisperWindow()

      WhisperWindow::~WhisperWindow()
      {
      } // end ~WhisperWindow()

      bool WhisperWindow::OnSay(const CEGUI::EventArgs& args)
      {
        const CEGUI::WindowEventArgs& ddea =
          static_cast<const CEGUI::WindowEventArgs&>(args);

        CEGUI::String text = ddea.window->getText();
        if (text.empty()) return true;

        if (!ddea.window->getParent()->isUserStringDefined("Nickname"))
        {
          Report(PT::Error, "WhisperWindow: Nickname wasn't defined!");
          return true;
        }

        if (!ddea.window->getParent()->isUserStringDefined("OwnNickname"))
        {
          Report(PT::Error, "WhisperWindow: Own nickname wasn't defined!");
          return true;
        }

        CEGUI::String nick =
          ddea.window->getParent()->getUserString("Nickname");
        Report(PT::Debug, "Whisper: %s to %s.", text.c_str(), nick.c_str());

        // Send the whisper to the network.
        WhisperToMessage nmsg;
        // Name of who you want to talk to.
        nmsg.setListenerName(ptString(nick.c_str(), nick.length()));
        nmsg.setMessage(text.c_str());
        network->send(&nmsg);

        // Get the chatlog and add your own message.
        CEGUI::String ownnick =
          ddea.window->getParent()->getUserString("OwnNickname");
        btn = winMgr->getWindow(nick+"Whisper/Chatlog");
        text = "<"+ownnick+">"+text;
        AddText(btn, text);

        ddea.window->setText(text.erase());
        return true;
      } // end OnSay()

      bool WhisperWindow::OnRollup(const CEGUI::EventArgs& args)
      {
        using namespace CEGUI;

        const WindowEventArgs& ddea =
          static_cast<const WindowEventArgs&>(args);
        ddea.window->setProperty("CaptionColour", "FFFFFFFF");

        return true;
      } // end OnRollup()

      bool WhisperWindow::OnCloseButton(const CEGUI::EventArgs& args)
      {
        using namespace CEGUI;

        const WindowEventArgs& ddea =
          static_cast<const WindowEventArgs&>(args);
        winMgr->destroyWindow(ddea.window);

        return true;
      } // end OnCloseButton()

      bool WhisperWindow::OnCaptureGained(const CEGUI::EventArgs& args)
      {
        using namespace CEGUI;

        const WindowEventArgs& ddea =
          static_cast<const WindowEventArgs&>(args);
        ddea.window->setProperty("CaptionColour", "FFFFFFFF");

        return true;
      } // end OnCaptureGained()

      void WhisperWindow::AddText(CEGUI::Window* logwindow, CEGUI::String msg)
      {
        CEGUI::MultiLineEditbox* chatlog =
          static_cast<CEGUI::MultiLineEditbox*>(btn);
        if (!msg.empty())
        {
          // append newline to this entry
          msg += '\n';
          // append new text to history output
          chatlog->setText(chatlog->getText() + msg);
          // scroll to bottom of history output
          chatlog->setCaratIndex((size_t)-1);
        }
      } // end AddText()

      void WhisperWindow::AddWhisper(const char* nick, const char* msg,
                                     const char* ownnick)
      {
        CEGUI::String nickstr = (CEGUI::String)(nick);
        CEGUI::String window_nick = nickstr;
        CEGUI::String window_nick_input = nickstr;
        CEGUI::String window_nick_log = nickstr;
        window_nick += "Whisper/Frame";
        window_nick_input += "Whisper/InputBox";
        window_nick_log += "Whisper/Chatlog";

        // If the window doesn't exist already, create it.
        if (!winMgr->isWindowPresent(window_nick))
        {
          vfs->ChDir ("/peragro/gui/client/");
          CEGUI::Window* root = winMgr->getWindow("Root");
          btn = winMgr->loadWindowLayout("whisper.xml", nickstr);
          root->addChildWindow(btn);
          btn->setUserString("Nickname",nickstr);
          btn->setText(nickstr);
          ((CEGUI::FrameWindow*)btn)->toggleRollup();
          btn->subscribeEvent(CEGUI::FrameWindow::EventRollupToggled,
            CEGUI::Event::Subscriber(&WhisperWindow::OnRollup, this));
          btn->subscribeEvent(CEGUI::FrameWindow::EventCloseClicked,
            CEGUI::Event::Subscriber(&WhisperWindow::OnCloseButton, this));
          btn->subscribeEvent(CEGUI::FrameWindow::EventActivated,
            CEGUI::Event::Subscriber(&WhisperWindow::OnCaptureGained, this));
          CEGUI::String ownnickstr = static_cast<CEGUI::String>
            (PT::Entity::PlayerEntity::Instance()->GetName());
          btn->setUserString("OwnNickname",ownnickstr);
        }

        // Color the titlebar text to signal new text.
        btn = winMgr->getWindow(window_nick);
        btn->setProperty("CaptionColour", "FFFF0000");

        // Set some additional things when ownnick is given.
        CEGUI::String message = (CEGUI::String)(msg);
        if (ownnick)
        {
          CEGUI::String ownnickstr = (CEGUI::String)(ownnick);
          message = "<"+ownnickstr+"> "+message;
        }
        else
        {
          message = "<"+nickstr+"> "+message;
        }

        // set up submitting message on enter
        btn = winMgr->getWindow(window_nick_input);
        btn->subscribeEvent(CEGUI::Editbox::EventTextAccepted,
          CEGUI::Event::Subscriber(&WhisperWindow::OnSay, this));

        // Get the log and add the message.
        btn = winMgr->getWindow(window_nick_log);
        AddText(btn, message);
      } // end AddWhisper()

      bool WhisperWindow::Create()
      {
        ReloadWindow();
        return true;
      } // end Create()

      bool WhisperWindow::ReloadWindow()
      {
        winMgr = cegui->GetWindowManagerPtr ();
        return true;
      } // end ReloadWindow()

    } // Windows namespace
  } // GUI namespace
} // PT namespace

