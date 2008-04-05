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

#include "client/pointer/pointer.h"
#include "client/reporter/reporter.h"

#include "client/event/eventmanager.h"
#include "client/network/network.h"
#include "common/network/netmessage.h"

#include "client/gui/guimanager.h"
#include "client/gui/chat-gui.h"
#include "client/gui/whisper-gui.h"

#include "client/event/chatevent.h"

#include "commands.h"

namespace PT
{
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
      using namespace PT::Events;

      // Register listener for ChatSayEvent.
      EventHandler<ChatManager>* cbSay = new EventHandler<ChatManager>(&ChatManager::HandleSay, this);
      PointerLibrary::getInstance()->getEventManager()->AddListener("chat.say", cbSay);

      // Register listener for ChatWhisperEvent.
      EventHandler<ChatManager>* cbWhisper = new EventHandler<ChatManager>(&ChatManager::HandleWhisper, this);
      PointerLibrary::getInstance()->getEventManager()->AddListener("chat.whisper", cbWhisper);

      // Handle submit.
      CEGUI::SlotFunctorBase* function = new CEGUI::MemberFunctionSlot<ChatManager>(&ChatManager::OnSubmit, this);
      GUIManager* guimanager = PointerLibrary::getInstance()->getGUIManager();
      guimanager->GetChatWindow()->SetSubmitEvent(function);

      EventHandler<ChatManager>* cb = new EventHandler<ChatManager>(&ChatManager::ProcessEvents, this);
      // Register listener for EntityAddEvent.
      PointerLibrary::getInstance()->getEventManager()->AddListener("entity.add", cb);
      // Register listener for EntityRemoveEvent.
      PointerLibrary::getInstance()->getEventManager()->AddListener("entity.remove", cb);

      // Register commands.
      Command* cmd = new cmdHelp(); RegisterCommand(cmd);
      cmd = new cmdSay(); RegisterCommand(cmd);
      cmd = new cmdSayMe(); RegisterCommand(cmd);
      cmd = new cmdWhisper(); RegisterCommand(cmd);
      cmd = new cmdRelocate(); RegisterCommand(cmd);
      cmd = new cmdGreet(); RegisterCommand(cmd);
      cmd = new cmdSit(); RegisterCommand(cmd);
      cmd = new cmdDbg(); RegisterCommand(cmd);

      historypointer=0;

      return true;
    } // end Initialize ()

    bool ChatManager::HandleSay(iEvent& ev)
    {
      using namespace PT::Events;

      std::string nick = ChatHelper::GetNickName(&ev);
      std::string message = ChatHelper::GetMessage(&ev);

      // TODO: input command handler.
      if (strncmp (message.c_str(),"/me",3) == 0)
      {
        std::string text = nick + message.substr(3, message.size());
        guimanager->GetChatWindow ()->AddMessage (text.c_str());
      }
      else if (strncmp (message.c_str(),"/greet",3) == 0)
      {
        std::string text = nick + " waves at" + message.substr(6, message.size());
        guimanager->GetChatWindow ()->AddMessage (text.c_str());
      }
      else
        guimanager->GetChatWindow ()->AddChatMessage (nick.c_str(), message.c_str());

      return true;
    } // end HandleSay ()

    bool ChatManager::HandleWhisper(iEvent& ev)
    {
      using namespace PT::Events;

      std::string nick = ChatHelper::GetNickName(&ev);
      std::string message = ChatHelper::GetMessage(&ev);

      guimanager->GetWhisperWindow()->AddWhisper(nick.c_str(), message.c_str());

      return true;
    } // end HandleWhisper ()

    bool ChatManager::OnSubmit (const CEGUI::EventArgs& e)
    {
      using namespace CEGUI;

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
        CEGUI::WindowManager* winMgr = guimanager->GetCEGUI()->GetWindowManagerPtr();
        CEGUI::Window* btn = winMgr->getWindow("InputPanel/InputBox");
        if (!btn)
        {
          Report(PT::Error, "Inputbox of Chat not found!");
          return false;
        } // end if

        CEGUI::String text = btn->getText();

        // If the submitted text is empty, hide the InputPanel.
        if (text.empty()) 
        {
          winMgr->getWindow("InputPanel/Frame")->setVisible(false);
          winMgr->getWindow("Chatlog/Frame")->activate();
          return true;
        } // end if

        // Handle submitted text.
        HandleOutput(text.c_str());

        history.push_back(text);
        historypointer=0; // Reset pointer

        // Erase the text.
        btn->setText(text.erase());

        return true;
      }
      // Get previous message in history
      else if (keyArgs.scancode == Key::ArrowUp)
      {
        CEGUI::WindowManager* winMgr = guimanager->GetCEGUI()->GetWindowManagerPtr();
        CEGUI::Window* btn = winMgr->getWindow("InputPanel/InputBox");
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
        CEGUI::WindowManager* winMgr = guimanager->GetCEGUI()->GetWindowManagerPtr();
        CEGUI::Window* btn = winMgr->getWindow("InputPanel/InputBox");
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
      if(historypointer>(int)history.size()){historypointer=0;return "";}
      return history[history.size()-historypointer].c_str();
    }

    const char* ChatManager::NextMessage()
    {
      if(historypointer>0){historypointer--;}
      if(historypointer==0){return "";}
      return history[history.size()-historypointer].c_str();
    }

    StringArray ChatManager::ParseString (const char* texti)
    {
      std::string text = texti;
      StringArray arg;
      size_t beginPos = 0;


      // Push the special character on the array and set the offset.
      if ((strncmp (texti,"/",1) == 0) ||
        (strncmp (texti,"!",1) == 0))
      {
        arg.push_back(text.substr(0, 1));
        beginPos = 1;
      } // end if

      std::string args = text.substr(beginPos, text.size());
      std::string tail = args;

      // Push the seperate words on an array.
      while (tail.size() > 0)
      {
        size_t pos = tail.find_first_of(" ");
        if ( pos == std::string::npos ) 
        {
          arg.push_back( tail.substr(0, tail.size()+1) );
          Report(PT::Notify, "ParseString: Added argument: %s", tail.substr(0, tail.size()).c_str() );
          tail.clear();
        }
        else
        {
          arg.push_back( tail.substr(0, pos) );
          Report(PT::Notify, "ParseString: Added argument: %s", tail.substr(0, pos).c_str() );
          tail = tail.substr(pos+1, tail.size());
        } // end if
      } // end while

      return arg;

    } // end ParseString ()

    void ChatManager::Execute (const char* cmd, const StringArray& args)
    {
      std::vector<Commandp>::iterator it;
      for (it = commands.begin(); it != commands.end(); ++it)
      {
        if (strcmp (it->get()->GetCommand(), cmd) == 0)
        {
          it->get()->Execute(args);
          return;
        } // end if
      } // end for

      Report(PT::Warning, "Unknown command '%s'!", cmd);

    } // end Execute ()

    void ChatManager::RegisterCommand (Command* cmd)
    {
      Commandp cmdp(cmd);
      commands.push_back(cmdp);
    } // end RegisterCommand ()

    void ChatManager::HandleOutput (const char* texti)
    {
      StringArray arg = ParseString (texti);

      // It's a command.
      if (arg.size() > 1 && arg[0].compare("/") == 0)
      {
        Execute(arg[1].c_str(), arg);
      } // end if
      else
        Execute("say", arg); // Special case.

    } // end HandleOutput ()

    void ChatManager::TabCompletion ()
    {
      CEGUI::WindowManager* winMgr = guimanager->GetCEGUI()->GetWindowManagerPtr();
      CEGUI::Window* btn = winMgr->getWindow("InputPanel/InputBox");
      if (!btn)
      {
        Report(PT::Error, "Inputbox of Chat not found!");
        return;
      } // end if

      CEGUI::String text = btn->getText();
      if (text.empty()) return;

      StringArray words = ParseString (text.c_str());

      std::string incompleteWord = words[words.size()-1];

      Report(PT::Error, "INCOMPLETE WORD: %s", incompleteWord.c_str());

      std::map<unsigned int, std::string>::iterator p;
      for (p = playernames.begin(); p!=playernames.end(); ++p)
      {
        std::string playername = p->second;
        // Convert playername to lowercase for compare.
        std::transform(playername.begin(), playername.end(),playername.begin(),tolower);
        if(playername.find(incompleteWord.c_str(), 0, incompleteWord.length()) != std::string::npos )
        {
          // Restore playername.
          playername = p->second;
          // Cut off the incomplete word and add the playername.
          text = text.substr(0, text.length()-incompleteWord.length());
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

      std::string id = PointerLibrary::getInstance()->getEventManager()->Retrieve(ev.GetName());

      if (id.compare("entity.add") == 0)
      {
        if (EntityHelper::GetEntityType(&ev) == PT::Entity::PCEntityType)
        {
          const char * nick = 0;
          ev.Retrieve("entityName", nick);
          unsigned int id = EntityHelper::GetEntityID(&ev);

          csString string; string.Format("%s has joined.", nick);
          guimanager->GetChatWindow ()->AddMessage (string.GetData());
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
          guimanager->GetChatWindow ()->AddMessage (string.GetData());
        }

        playernames.erase(id);
      } // end if

      return true;
    } // end ProcessEvents ()

  } // Chat namespace 
} // PT namespace 
