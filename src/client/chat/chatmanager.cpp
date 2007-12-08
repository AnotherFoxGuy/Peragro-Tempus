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
      PointerLibrary::getInstance()->getEventManager()->AddListener("ChatSayEvent", cbSay);

      // Register listener for ChatWhisperEvent.
      EventHandler<ChatManager>* cbWhisper = new EventHandler<ChatManager>(&ChatManager::HandleWhisper, this);
      PointerLibrary::getInstance()->getEventManager()->AddListener("ChatWhisperEvent", cbWhisper);

      // Handle submit.
      CEGUI::SlotFunctorBase* function = new CEGUI::MemberFunctionSlot<ChatManager>(&ChatManager::OnSubmit, this);
      GUIManager* guimanager = PointerLibrary::getInstance()->getGUIManager();
      guimanager->GetChatWindow()->SetSubmitEvent(function);

      EventHandler<ChatManager>* cb = new EventHandler<ChatManager>(&ChatManager::ProcessEvents, this);
      // Register listener for EntityAddEvent.
      PointerLibrary::getInstance()->getEventManager()->AddListener("EntityAddEvent", cb);
      // Register listener for EntityRemoveEvent.
      PointerLibrary::getInstance()->getEventManager()->AddListener("EntityRemoveEvent", cb);

      // Register commands.
      Command* cmd = new cmdHelp(); RegisterCommand(cmd);
      cmd = new cmdSay(); RegisterCommand(cmd);
      cmd = new cmdSayMe(); RegisterCommand(cmd);
      cmd = new cmdWhisper(); RegisterCommand(cmd);
      cmd = new cmdRelocate(); RegisterCommand(cmd);
      cmd = new cmdGreet(); RegisterCommand(cmd);
      cmd = new cmdSit(); RegisterCommand(cmd);
      cmd = new cmdTest(); RegisterCommand(cmd);

      return true;
    } // end Initialize ()

    bool ChatManager::HandleSay(PT::Events::Eventp ev)
    {
      using namespace PT::Events;

      ChatSayEvent* chatEv = GetChatEvent<ChatSayEvent*>(ev);
      if (!chatEv) return false;

      // TODO: input command handler.
      if (strncmp (chatEv->message.c_str(),"/me",3) == 0)
      {
        std::string ownnick = chatEv->nickName;
        std::string text = ownnick + chatEv->message.substr(3, chatEv->message.size());
        guimanager->GetChatWindow ()->AddMessage (text.c_str());
      }
      else if (strncmp (chatEv->message.c_str(),"/greet",3) == 0)
      {
        std::string text = chatEv->nickName + " waves at" + chatEv->message.substr(6, chatEv->message.size());
        guimanager->GetChatWindow ()->AddMessage (text.c_str());
      }
      else
        guimanager->GetChatWindow ()->AddChatMessage (chatEv->nickName.c_str(), chatEv->message.c_str());

      return true;
    } // end HandleSay ()

    bool ChatManager::HandleWhisper(PT::Events::Eventp ev)
    {
      using namespace PT::Events;

      ChatSayEvent* chatEv = GetChatEvent<ChatSayEvent*>(ev);
      if (!chatEv) return false;

      guimanager->GetWhisperWindow()->AddWhisper(chatEv->nickName.c_str(), chatEv->message.c_str());

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

        // Erase the text.
        btn->setText(text.erase());

        // TODO: Push the messages on a stack to get some 
        // 'command history'.

        return true;
      } // end if

      return false;

    } // end OnSubmit ()

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

    bool ChatManager::ProcessEvents(PT::Events::Eventp ev)
    {
      using namespace PT::Events;

      if (ev->GetEventID().compare("EntityAddEvent") == 0)
      {
        EntityAddEvent* entityAddEv = GetEntityEvent<EntityAddEvent*>(ev);
        if (!entityAddEv) return false;

        if (entityAddEv->entityType == PT::Entity::PCEntityType)
        {
          playernames.insert(std::pair<unsigned int, std::string>(entityAddEv->entityId, entityAddEv->entityName));
        } // end if
      } // end if
      else if (ev->GetEventID().compare("EntityRemoveEvent") == 0)
      {
        EntityRemoveEvent* entityRemoveEv = GetEntityEvent<EntityRemoveEvent*>(ev);
        if (!entityRemoveEv) return false;

          playernames.erase(entityRemoveEv->entityId);
      } // end if

      return true;
    } // end ProcessEvents ()

  } // Chat namespace 
} // PT namespace 
