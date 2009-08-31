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

#include "chatmanager.h"

#include <iutil/objreg.h>
#include <iutil/plugin.h>

#include "client/pointer/pointer.h"
#include "common/reporter/reporter.h"

#include "common/eventcs/eventmanager.h"
#include "client/network/network.h"
#include "common/network/netmessage.h"

#include "client/gui/guimanager.h"
#include "client/gui/chat-gui.h"
#include "client/gui/chatinput-gui.h"
#include "client/gui/whisper-gui.h"

#include "common/eventcs/chatevent.h"
#include "common/eventcs/entityevent.h"

// to determine the identity of the player character
#include "client/entity/player/playerentity.h"

namespace PT
{
  using namespace Command;

  namespace Chat
  {

    ChatManager::ChatManager ()
    {
      this->obj_reg = PointerLibrary::getInstance()->getObjectRegistry();

      network = PointerLibrary::getInstance()->getNetwork();
      guimanager = PointerLibrary::getInstance()->getGUIManager();
    }

    ChatManager::~ChatManager ()
    {
    }

    bool ChatManager::Initialize ()
    {
      using namespace PT::GUI;
      using namespace PT::GUI::Windows;

      PT_SETUP_HANDLER
      PT_REGISTER_LISTENER(ChatManager, HandleSay, "chat.say")
      PT_REGISTER_LISTENER(ChatManager, HandleWhisper, "chat.whisper")
      PT_REGISTER_LISTENER(ChatManager, HandleGroup, "chat.group")

      PT_REGISTER_LISTENER(ChatManager, ProcessEvents, "entity.add")
      PT_REGISTER_LISTENER(ChatManager, ProcessEvents, "entity.remove")

      PT_REGISTER_LISTENER(ChatManager, UpdateOptions, "interface.options.chat")
      UpdateOptions();

      // Handle submit.
      CEGUI::SlotFunctorBase* function = new
        CEGUI::MemberFunctionSlot<ChatManager>(&ChatManager::OnSubmit, this);
      GUIManager* guimanager = PointerLibrary::getInstance()->getGUIManager();
      ChatInputWindow* chatinputWindow = guimanager->GetWindow<ChatInputWindow>(CHATINPUTWINDOW);
      chatinputWindow->SetSubmitEvent(function);

      cmd_mgr = csLoadPluginCheck<iCommandManager> (obj_reg, PT_COMMAND_MANAGER_NAME);

      historypointer = 0;

      return true;
    } // end Initialize ()

    bool ChatManager::HandleSay(iEvent& ev)
    {
      using namespace PT::Events;
      using namespace PT::GUI;
      using namespace PT::GUI::Windows;

      // It's a whisper.
      if (ChatHelper::GetVolume(&ev) == 0)
        return HandleWhisper(ev);

      std::string nick = ChatHelper::GetNickName(&ev);
      std::string message = ChatHelper::GetMessage(&ev);

      ChatWindow* chatWindow = guimanager->GetWindow<ChatWindow>(CHATWINDOW);
      // TODO: input command handler.
      if (strncmp (message.c_str(),"/me",3) == 0)
      {
        std::string text = nick + message.substr(3, message.size());
        chatWindow->AddMessage (text.c_str());
        chatLogger.LogMessage (CHATLOGGER_MESSAGE_EMOTE, text.c_str());
      }
      else if (strncmp (message.c_str(),"/greet",3) == 0)
      {
        std::string text = nick + " waves at" +
          message.substr(6, message.size());
        chatWindow->AddMessage (text.c_str());
        chatLogger.LogMessage (CHATLOGGER_MESSAGE_EMOTE, text.c_str());
      }
      else
      {
        chatWindow->AddChatMessage (nick.c_str(), message.c_str());
        chatLogger.LogMessage (CHATLOGGER_MESSAGE_SAY, nick.c_str(),
          message.c_str());
      }
      return true;
    } // end HandleSay ()

    bool ChatManager::HandleWhisper(iEvent& ev)
    {
      using namespace PT::Events;
      using namespace PT::GUI;
      using namespace PT::GUI::Windows;

      std::string nick = ChatHelper::GetNickName(&ev);
      std::string message = ChatHelper::GetMessage(&ev);

      WhisperWindow* whisperWindow =
        guimanager->GetWindow<WhisperWindow>(WHISPERWINDOW);

      if (nick != "")
      {
        whisperWindow->AddWhisper(nick.c_str(), message.c_str());
        chatLogger.LogMessage (CHATLOGGER_MESSAGE_WHISPER, nick.c_str(),
          message.c_str());
      }
      else
      {
        nick = ChatHelper::GetTargetNick(&ev);
        std::string ownnick = Entity::PlayerEntity::Instance()->GetName();
        whisperWindow->AddWhisper(nick.c_str(), message.c_str(), ownnick.c_str());
      }

      return true;
    } // end HandleWhisper ()

    bool ChatManager::HandleGroup(iEvent& ev)
    {
      using namespace PT::Events;
      using namespace PT::GUI;
      using namespace PT::GUI::Windows;

      std::string nick = ChatHelper::GetNickName(&ev);
      std::string message = ChatHelper::GetMessage(&ev);
      std::string channel = ChatHelper::GetChannel(&ev);

      /* TODO: Message is only added to main chat window, because there's
         no distinct window yet */
      std::string text = "[" + channel + "] <" + nick + "> " + message;
      ChatWindow* chatWindow = guimanager->GetWindow<ChatWindow>(CHATWINDOW);
      chatWindow->AddMessage (text.c_str());
      chatLogger.LogMessage (CHATLOGGER_MESSAGE_GROUP, text.c_str());

      return true;
    } // end HandleGroup ()

    bool ChatManager::OnSubmit (const CEGUI::EventArgs& e)
    {
      using namespace CEGUI;
      using namespace PT::GUI::Windows;

      const KeyEventArgs& keyArgs = static_cast<const KeyEventArgs&>(e);

      // If TAB is pressed, do tabcompletion.
      if (keyArgs.scancode == Key::Tab)
      {
        TabCompletion();
        return true;
      }
      // If Return is pressed, handle it for output.
      else if (keyArgs.scancode == Key::Return)
      {
        CEGUI::WindowManager* winMgr =
          guimanager->GetCEGUI()->GetWindowManagerPtr();
        CEGUI::Window* btn = winMgr->getWindow("Chatinput/InputBox");
        if (!btn)
        {
          Report(PT::Error, "Inputbox of Chat not found!");
          return false;
        } // end if

        CEGUI::String text = btn->getText();

        // If the submitted text is empty, hide the InputPanel.
        if (text.empty())
        {
          winMgr->getWindow("Chatinput/Frame")->setVisible(false);
          winMgr->getWindow("Chatlog/Frame")->activate();
          return true;
        } // end if

        // Handle submitted text.
        if (cmd_mgr)
        {
          std::string resultstr = cmd_mgr->ProcessString(text.c_str());
          ChatWindow* chatWindow =
            guimanager->GetWindow<ChatWindow>(CHATWINDOW);
          chatWindow->AddMessage(resultstr.c_str());
        }

        history.push_back(text);
        historypointer=0; // Reset pointer

        // Erase the text.
        btn->setText(text.erase());

        if (hideAfterSend)
        {
          winMgr->getWindow("Chatinput/Frame")->setVisible(false);
          winMgr->getWindow("Chatlog/Frame")->activate();
        } // end if

        return true;
      }
      // Get previous message in history
      else if (keyArgs.scancode == Key::ArrowUp)
      {
        CEGUI::WindowManager* winMgr =
          guimanager->GetCEGUI()->GetWindowManagerPtr();
        CEGUI::Window* btn = winMgr->getWindow("Chatinput/InputBox");
        if (!btn)
        {
          Report(PT::Error, "Inputbox of Chat not found!");
          return false;
        } // end if
        btn->setText(PreviousMessage());
        static_cast<CEGUI::Editbox*>(btn)->setCaratIndex((size_t)-1);
        return true;
      }
      // Get next message in history
      else if (keyArgs.scancode == Key::ArrowDown)
      {
        CEGUI::WindowManager* winMgr =
          guimanager->GetCEGUI()->GetWindowManagerPtr();
        CEGUI::Window* btn = winMgr->getWindow("Chatinput/InputBox");
        if (!btn)
        {
          Report(PT::Error, "Inputbox of Chat not found!");
          return false;
        } // end if
        btn->setText(NextMessage());
        static_cast<CEGUI::Editbox*>(btn)->setCaratIndex((size_t)-1);
        return true;
      } // end if

      return false;

    } // end OnSubmit ()

    const char* ChatManager::PreviousMessage()
    {
      historypointer++;
      if (historypointer > static_cast<int>(history.size()))
      {
        historypointer = 0;
        return "";
      }
      return history[history.size() - historypointer].c_str();
    }

    const char* ChatManager::NextMessage()
    {
      if (historypointer > 0) { historypointer--; }
      if (historypointer == 0) { return ""; }
      return history[history.size() - historypointer].c_str();
    }

    void ChatManager::TabCompletion ()
    {
      CEGUI::WindowManager* winMgr =
        guimanager->GetCEGUI()->GetWindowManagerPtr();
      CEGUI::Window* btn = winMgr->getWindow("InputPanel/InputBox");
      if (!btn)
      {
        Report(PT::Error, "Inputbox of Chat not found!");
        return;
      } // end if

      CEGUI::String text = btn->getText();
      if (text.empty()) return;

      const char* text_str = text.c_str();
      const char* textword;
      for (textword = text_str + text.size(); textword > text_str && (isalnum(*textword) || !(*textword)); textword--);

      std::string incompleteWord = textword;

      Report(PT::Error, "INCOMPLETE WORD: %s", incompleteWord.c_str());

      std::map<unsigned int, std::string>::iterator p;
      for (p = playernames.begin(); p!=playernames.end(); ++p)
      {
        std::string playername = p->second;
        // Convert playername to lowercase for compare.
        std::transform(playername.begin(), playername.end(),
          playername.begin(), tolower);
        if (playername.find(incompleteWord.c_str(), 0, incompleteWord.length())
          != std::string::npos )
        {
          // Restore playername.
          playername = p->second;
          // Cut off the incomplete word and add the playername.
          text = text.substr(0, text.length() - incompleteWord.length());
          text += playername.c_str();
          // Update the text.
          btn->setText(text);
          ((CEGUI::Editbox*)btn)->setCaratIndex(text.length());
          break;
        } // end if
      } // end for
    } // end TabCompletion ()

    bool ChatManager::ProcessEvents(iEvent& ev)
    {
      using namespace PT::Events;
      using namespace PT::GUI;
      using namespace PT::GUI::Windows;

      EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();

      std::string id = evmgr->Retrieve(ev.GetName());

      if (id.compare("entity.add") == 0)
      {
        if (EntityHelper::GetEntityType(&ev, evmgr) ==
          Common::Entity::PCEntityType)
        {
          const char * nick = 0;
          ev.Retrieve("entityName", nick);
          unsigned int id = EntityHelper::GetEntityID(&ev);

          csString string; string.Format("%s has joined.", nick);
          ChatWindow* chatWindow = guimanager->GetWindow<ChatWindow>(CHATWINDOW);
          chatWindow->AddMessage (string.GetData());
          playernames.insert(std::pair<unsigned int, std::string>(id, nick));
        } // end if
      } // end if
      else if (id.compare("entity.remove") == 0)
      {
        unsigned int id = EntityHelper::GetEntityID(&ev);

        std::string playerName = playernames[id];
        if (playerName.size() > 0)
        {
          csString string; string.Format("%s has left.", playerName.c_str());
          ChatWindow* chatWindow = guimanager->GetWindow<ChatWindow>(CHATWINDOW);
          chatWindow->AddMessage (string.GetData());
        }

        playernames.erase(id);
      } // end if

      return true;
    } // end ProcessEvents ()

    bool ChatManager::UpdateOptions(iEvent& ev)
    {
      UpdateOptions();
      return true;
    } // end UpdateOptions(iEvent& ev)

    void ChatManager::UpdateOptions()
    {
      csRef<iConfigManager> app_cfg = csQueryRegistry<iConfigManager>(
        PointerLibrary::getInstance()->getObjectRegistry());

      hideAfterSend = app_cfg->GetBool("Peragro.Chat.HideAfterSending",
        hideAfterSend);
    } // end UpdateOptions()

  } // Chat namespace
} // PT namespace
