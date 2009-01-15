/*
    Copyright (C) 2008 Development Team of Peragro Tempus

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

#include "cmddate.h"
#include "chatmanager.h"

#include <sstream>

#include "client/pointer/pointer.h"

#include "client/environment/clock.h"
#include "client/environment/environmentmanager.h"

#include "client/gui/guimanager.h"
#include "client/gui/chat-gui.h"

#include "common/util/printhelper.h"

using namespace PT::Commands;

    cmdDate::cmdDate () : CommandDefault("date") { }

    cmdDate::~cmdDate () { }

    std::string cmdDate::HelpUsage (const char*) const
    { return "'/date [short|long|full]'"; }

    std::string cmdDate::HelpSynopsis (const char*) const
    { return "Prints the in-game time and date."; }

    std::string cmdDate::HelpFull (const char*) const
    { return "Prints the in-game time and date as received from the server."; }

    void cmdDate::Execute (const StringArray& args)
    {
      using namespace PT::GUI;
      using namespace PT::GUI::Windows;

      // Element 0 is '/', 1 is 'date'
      if (args.size() != 2 && args.size() != 3) throw BadUsage();

      GUIManager* guimanager = PointerLibrary::getInstance()->getGUIManager();
      if(!guimanager) return;
      ChatWindow* chatWindow = guimanager->GetWindow<ChatWindow>(CHATWINDOW);

      Environment::EnvironmentManager* envmanager = PointerLibrary::getInstance()->getEnvironmentManager();
      if(!envmanager) return;
      const Environment::Clock* clock = envmanager->GetClock();
      if(!clock) return;

      std::stringstream ss;

      if (args.size() == 3)
      {
        if (args[2].compare("short") == 0)
        {
          ss << clock->GetDayTime();
        }
        else if (args[2].compare("long") == 0)
        {
          ss << clock->GetSplitDate();
        }
        else if (args[2].compare("full") == 0)
        {
          //ss << clock->GetFullDate();
        }
        else
        {
          ss << "Unrecognised date format.";
        }
      }
      else
      {
        ss << clock->GetDayTime();
      }

      chatWindow->AddMessage(ss.str().c_str());
    } // cmdDate::Execute
