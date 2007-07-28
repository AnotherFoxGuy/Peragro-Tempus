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

		ChatManager::ChatManager (iObjectRegistry* obj_reg)
		{
			this->obj_reg = obj_reg;

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

			// Register commands.
			Command* cmd = new cmdSay(); RegisterCommand(cmd);
			cmd = new cmdWhisper(); RegisterCommand(cmd);
			cmd = new cmdRelocate(); RegisterCommand(cmd);

			return true;
		}

		bool ChatManager::HandleSay(PT::Events::Eventp ev)
		{
			using namespace PT::Events;

			ChatSayEvent* chatEv = GetChatEvent<ChatSayEvent*>(ev);
			if (!chatEv) return false;

			guimanager->GetChatWindow ()->AddChatMessage (chatEv->nickName.c_str(), chatEv->message.c_str());

			return true;
		}

		bool ChatManager::HandleWhisper(PT::Events::Eventp ev)
		{
			using namespace PT::Events;

			ChatSayEvent* chatEv = GetChatEvent<ChatSayEvent*>(ev);
			if (!chatEv) return false;

			guimanager->GetWhisperWindow()->AddWhisper(chatEv->nickName.c_str(), chatEv->message.c_str());

			return true;
		}

		bool ChatManager::OnSubmit (const CEGUI::EventArgs& e)
		{
			CEGUI::WindowManager* winMgr = guimanager->GetCEGUI()->GetWindowManagerPtr();
			CEGUI::Window* btn = winMgr->getWindow("InputPanel/InputBox");
			if (!btn)
			{
				printf("Inputbox of Chat not found!\n");
				return false;
			}

			CEGUI::String text = btn->getText();

			// If the submitted text is empty, hide the InputPanel.
			if (text.empty()) 
			{
				winMgr->getWindow("InputPanel/Frame")->setVisible(false);
				winMgr->getWindow("Chatlog/Frame")->activate();
				return true;
			}

			// Handle submitted text.
			HandleOutput(text.c_str());

			// Erase the text.
			btn->setText(text.erase());

			// TODO: Push the messages on a stack to get some 
			// 'command history'.

			return true;
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
				arg.push(text.substr(0, 1));
				beginPos = 1;
			}

			std::string args = text.substr(beginPos, text.size());
			std::string tail = args;

			// Push the seperate words on an array.
			while (tail.size() > 0)
			{
				size_t pos = tail.find_first_of(" ");
				if ( pos == std::string::npos ) 
				{
					arg.push( tail.substr(0, tail.size()+1) );
					printf("I: ParseString: Added argument: %s\n", tail.substr(0, tail.size()).c_str() );
					tail.clear();
				}
				else
				{
					arg.push( tail.substr(0, pos) );
					printf("I: ParseString: Added argument: %s\n", tail.substr(0, pos).c_str() );
					tail = tail.substr(pos+1, tail.size());
				} // else
			} //while

/*
			std::string test;
			for(size_t i = 0; i < arg.size(); i++) 
			{
				test += arg.c[i];
				test += " | ";
			}
			printf("T: %s\n", test.c_str());
*/

			return arg;

		} //ParseString

		void ChatManager::Execute (const char* cmd, const StringArray& args)
		{
			std::vector<Commandp>::iterator it;
			for(it = commands.begin(); it != commands.end(); ++it)
			{
				if (strcmp (it->get()->GetCommand(), cmd) == 0)
				{
					it->get()->Execute(args);
					return;
				} // if
			} // for

			printf("W: Unknown command '%s'!\n", cmd);
		}

		void ChatManager::RegisterCommand (Command* cmd)
		{
			Commandp cmdp(cmd);
      commands.push_back(cmdp);
		}

		void ChatManager::HandleOutput (const char* texti)
		{
			StringArray arg = ParseString (texti);

			// It's a command.
			if (arg.size() > 1 && arg.c[0].compare("/") == 0)
			{
				Execute(arg.c[1].c_str(), arg);
			}
			else
				Execute("say", arg); // Special case.
		}

	} // Chat namespace 
} // PT namespace 
