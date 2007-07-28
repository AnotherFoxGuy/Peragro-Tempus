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

#ifndef PT_TRADE_MANAGER_H
#define PT_TRADE_MANAGER_H

#include <cssysdef.h>
#include <csutil/ref.h>

#include "client/event/chatevent.h"

#include "command.h"

#include <CEGUI.h>
#include <ivaria/icegui.h>

struct iObjectRegistry;

class Network;
class GUIManager;

namespace PT
{
	namespace Chat
	{
		class ChatManager
		{
		private:
			csRef<iObjectRegistry> obj_reg;
			Network* network;
			GUIManager* guimanager;

			std::vector<Commandp> commands;

		private:
			void HandleOutput (const char* texti);
			StringArray ParseString (const char* texti);
			void Execute (const char* cmd, const StringArray& args);

		public: // CEGUI events.
			bool OnSubmit (const CEGUI::EventArgs& e);

		public: // PT events.
			bool HandleSay(PT::Events::Eventp ev);
			bool HandleWhisper(PT::Events::Eventp ev);

		public:
			ChatManager (iObjectRegistry* obj_reg);
			~ChatManager ();

			bool Initialize ();
			void RegisterCommand (Command* cmd);
		};

	} // Chat namespace 
} // PT namespace 

#endif // PT_TRADE_MANAGER_H
