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

#ifndef CMDGROUP_H
#define CMDGROUP_H

#include <cssysdef.h>

#include "commanddefault.h"

#include "client/pointer/pointer.h"
#include "common/reporter/reporter.h"

#include "client/network/network.h"
#include "common/network/netmessage.h"

namespace PT
{
  namespace Chat
  {
    //--------------------------------------------------------------------------
    class cmdGroup : public CommandDefault
    {
    public:
      cmdGroup () : CommandDefault("group") { }
      virtual ~cmdGroup () { }

      virtual std::string HelpSynopsis (const char*) const
      { return "Send a message to all group members."; }
      virtual std::string HelpUsage (const char*) const
      { return "Usage: '/group <message>'"; }

      virtual void Execute (const StringArray& args)
      {
        Network* network = PointerLibrary::getInstance()->getNetwork();
        if(!network) return;

        // Element 0 is '/', 1 is 'say'
        if (args.size() < 3) throw BadUsage();
        else
        {
          std::string text;
          for(size_t i = 2; i < args.size(); i++)
          {
            text += args[i];
            text += " ";
          }

          Report(PT::Debug, "Group: %s", text.c_str());
          GroupMessage msg;
          msg.setMessage(text.c_str());
          msg.setChannel(ptString::create("#general"));
          network->send(&msg);

          return;
        }
      }
    };
    //--------------------------------------------------------------------------
  } // Chat namespace
} // PT namespace

#endif // CMDGROUP_H
