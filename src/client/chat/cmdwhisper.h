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

#ifndef CMDWHISPER_H
#define CMDWHISPER_H

#include <cssysdef.h>

#include "commanddefault.h"

#include "client/pointer/pointer.h"
#include "common/reporter/reporter.h"

#include "client/network/network.h"
#include "common/network/netmessage.h"

#include "client/gui/whisper-gui.h"

namespace PT
{
  namespace Chat
  {
    //--------------------------------------------------------------------------
    class cmdWhisper : public CommandDefault
    {
    public:
      cmdWhisper () : CommandDefault("whisper") { }
      virtual ~cmdWhisper () { }

      virtual const char* HelpSynopsis (const char*) const 
      { return "Start a private conversation with the target."; }

      virtual const char* HelpUsage (const char*) const
      { return "Usage: '/whisper <target> <message>'"; }

      virtual const char* HelpFull (const char*) const
      {
         return "Enter the name of a person to speak to, and a message to send"
          " to them, and a private conversation will open up with them, if"
          " they are available.";
      }

      virtual void Execute (const StringArray& args)
      {
        using namespace PT::GUI;
        using namespace PT::GUI::Windows;

        GUIManager* guimanager = PointerLibrary::getInstance()->getGUIManager();
        if(!guimanager) return;
        Network* network = PointerLibrary::getInstance()->getNetwork();
        if(!network) return;

        // Element 0 is '/', 1 is 'whisper'
        if (args.size() < 4) throw BadUsage();
        else
        {
          std::string nick = args[2];

          std::string text;
          for(size_t i = 3; i < args.size(); i++)
          {
            text += args[i];
            text += " ";
          }

          // Get your own nickname.
          std::string ownnick = Entity::PlayerEntity::Instance()->GetName();
          // Add your own text to the whisper.
          WhisperWindow* whisperWindow = guimanager->GetWindow<WhisperWindow>(WHISPERWINDOW);
          whisperWindow->AddWhisper(nick.c_str(), text.c_str(), ownnick.c_str());
          // Send the whisper to the network.
          WhisperToMessage nmsg;
          nmsg.setListenerName(ptString(nick.c_str(), nick.size())); //<-- name of who you want to talk to...
          nmsg.setMessage(text.c_str());
          network->send(&nmsg);

          return;
        }
      }
    };
    //--------------------------------------------------------------------------
  } // Chat namespace
} // PT namespace

#endif // CMDWHISPER_H
