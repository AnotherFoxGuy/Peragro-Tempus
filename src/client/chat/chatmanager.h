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

#include "common/event/eventmanager.h"

#include "command.h"

#include <CEGUI.h>
#include <ivaria/icegui.h>

struct iObjectRegistry;

class Network;

namespace PT
{
  namespace GUI
  {
    class GUIManager;
  } // GUI namespace

  namespace Chat
  {
    class ChatManager
    {
    private:
      csRef<iObjectRegistry> obj_reg;
      Network* network;
      PT::GUI::GUIManager* guimanager;

      std::vector<Commandp> commands;
      std::vector<CEGUI::String> history;
      int historypointer;
      std::map<unsigned int, std::string> playernames;

    private:
      void HandleOutput (const char* texti);
      StringArray ParseString (const char* texti);
      void Execute (const char* cmd, const StringArray& args);
      void TabCompletion ();

    public: // CEGUI events.
      bool OnSubmit (const CEGUI::EventArgs& e);
      /**
       * Get the previous message in the list, remembering which one was
       * accessed last and choosing the one before it.
       * @return The previous message in the message history.
       */
      const char* PreviousMessage();
      /**
       * Get the next message in the list, remembering which one was
       * accessed last and choosing the one after it.
       * @return The next message in the message history.
       */
      const char* NextMessage();

    public: // PT events.
      bool HandleSay(iEvent& ev);
      bool HandleWhisper(iEvent& ev);
      bool HandleGroup(iEvent& ev);
      bool ProcessEvents(iEvent& ev);

    public:
      ChatManager ();
      ~ChatManager ();

      bool Initialize ();
      void RegisterCommand (Command* cmd);

      const std::vector<Commandp>& GetAllCommands() { return commands; }
    };

  } // Chat namespace
} // PT namespace

#endif // PT_TRADE_MANAGER_H
