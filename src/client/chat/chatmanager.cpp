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
			if (text.empty()) 
			{
				winMgr->getWindow("InputPanel/Frame")->setVisible(false);
				winMgr->getWindow("Chatlog/Frame")->activate();
				return true;
			}

			// If HandleCommand returns true, the text was a command
			// and we can return.
			if ( HandleCommand(text.c_str()) ) 
			{
				btn->setText(text.erase());
				return true;
			}

			printf("Say: %s\n", text.c_str());
			SayMessage msg;
			msg.setMessage(text.c_str());
			network->send(&msg);
			btn->setText(text.erase());
			return true;
		}

		bool ChatManager::HandleCommand (const char* texti)
		{
			csString text = texti;

			// Check if the first character is "/"
			// then we know its a command.
			if (strncmp (texti,"/",1) == 0)
			{
				// Find the end of the first word, which is our command.
				size_t pos = text.FindFirst(" ");
				csString command;
				csArray<csString> arg;

				// The command is one word.
				if ( pos == (size_t)-1 ) 
					command = text.Slice(1, text.Length());
				// The command is several words.
				else 
				{
					command = text.Slice(1, pos-1);

					csString args = text.Slice(pos+1, text.Length());
					csString tail = args;

					// Push the arguments on an array.
					while (tail.Length() > 0)
					{
						size_t pos = tail.FindFirst(" ");
						if ( pos == (size_t)-1 ) 
						{
							arg.Push( tail.Slice(0, tail.Length()+1) );
							printf("ChatWindow: HandleCommand: Added argument: %s\n", tail.Slice(0, tail.Length()).GetData() );
							tail.Clear();
						}
						else
						{
							arg.Push( tail.Slice(0, pos) );
							printf("ChatWindow: HandleCommand: Added argument: %s\n", tail.Slice(0, pos).GetData() );
							tail = tail.Slice(pos+1, tail.Length());
						}
					}

				}

				printf("ChatWindow: HandleCommand: found %s\n", command.GetData());


				if (command.Compare("relocate"))
				{
					RelocateMessage msg;
					network->send(&msg);
				}
				else if (command.Compare("guild"))
				{
					printf("ChatWindow: HandleCommand: handled guild!\n");
				}
				else if (command.Compare("whisper"))
				{
					printf("ChatWindow: HandleCommand: handled whisper!\n");
					if (!arg.GetSize() || arg.GetSize() < 2) return true;
					csString nick = arg.Get(0);
					csString msg;

					for(uint i = 1; i < arg.GetSize(); i++) 
					{
						msg += arg.Get(i);
						msg += " ";
					}
					// Get your own nickname.
					csString ownnick = PointerLibrary::getInstance()->getEntityManager()->GetOwnName();
					// Add your own text to the whisper.
					guimanager->GetWhisperWindow()->AddWhisper(nick.GetData(), msg.GetData(), ownnick.GetData());
					// Send the whisper to the network.
					WhisperToMessage nmsg;
					nmsg.setListenerName(ptString(nick.GetData(), nick.Length())); //<-- name of who you want to talk to...
					nmsg.setMessage(msg.GetData());
					network->send(&nmsg);
				}
				else
					printf("ChatWindow: HandleCommand: Unknown command!\n"); 

				return true;
			}

			return false;
		}

	} // Chat namespace 
} // PT namespace 
