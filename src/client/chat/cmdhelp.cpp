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

#include "cmdhelp.h"
#include "chatmanager.h"

#include "client/pointer/pointer.h"

#include "client/gui/guimanager.h"
#include "client/gui/chat-gui.h"

namespace PT
{
  namespace Chat
  {

    cmdHelp::cmdHelp () : CommandDefault("help") { }

    cmdHelp::~cmdHelp () { }

    std::string cmdHelp::HelpUsage (const char*) const
    { return "'/help [<command>]'"; }

    std::string cmdHelp::HelpSynopsis (const char*) const
    { return "Prints this help."; }

    std::string cmdHelp::HelpFull (const char*) const
    { return "Prints a helpful command overview."; }

    void cmdHelp::Execute (const StringArray& args)
    {
      using namespace PT::GUI;
      using namespace PT::GUI::Windows;

      // Element 0 is '/', 1 is 'help'
      if (args.size() < 2 || args.size() > 3) throw BadUsage();

      const char* cmd = NULL;
      if (args.size() == 3) cmd = args[2].c_str();   // '/help cmd'

      GUIManager* guimanager = PointerLibrary::getInstance()->getGUIManager();
      if(!guimanager) return;
      ChatWindow* chatWindow = guimanager->GetWindow<ChatWindow>(CHATWINDOW);

      PT::Chat::ChatManager* chatmgr = PointerLibrary::getInstance()->getChatManager();
      if(!chatmgr) return;
      std::vector<Commandp> commands = chatmgr->GetAllCommands();

      std::vector<Commandp>::const_iterator it;
      for(it = commands.begin(); it != commands.end(); ++it)
      {
        if (cmd)
        {
          if (it->get()->CommandHandled(cmd))
          {
            std::string s = it->get()->Help(cmd, CMD_HELP_FULL);
            chatWindow->AddMessage(s.c_str());
            return;
          }
        }
        else
        {
          StringArray cmd_sub = it->get()->GetAllCommands();
          StringArray::const_iterator it_sub;
          for (it_sub = cmd_sub.begin(); it_sub != cmd_sub.end(); it_sub++)
          {
            const char* cmd = it_sub->c_str();
            std::string text = *it_sub;
            text += ": " + it->get()->Help(cmd, CMD_HELP_SYNOPSIS);
            chatWindow->AddMessage (text.c_str());
          } // for
        } // else
      } // for

    } // cmdHelp::Execute


  } // Chat namespace
} // PT namespace
