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

#ifndef CMDRELOCATE_H
#define CMDRELOCATE_H

#include <cssysdef.h>

#include "commanddefault.h"

#include "client/pointer/pointer.h"

#include "client/network/network.h"
#include "common/network/netmessage.h"

namespace PT
{
  namespace Commands
  {
    //--------------------------------------------------------------------------
    class cmdRelocate : public CommandDefault
    {
    public:
      cmdRelocate () : CommandDefault("relocate") { }
      virtual ~cmdRelocate () { }

      virtual std::string HelpSynopsis (const char*) const
      { return "Teleport yourself to the start location."; }
      virtual std::string HelpUsage (const char*) const
      { return "Usage: '/relocate'"; }

      virtual void Execute (const StringArray& args)
      {
        Network* network = PointerLibrary::getInstance()->getNetwork();
        if(!network) return;

        // Element 0 is '/', 1 is 'relocate'
        if (args.size() < 2) throw BadUsage();

        RelocateMessage msg;
        network->send(&msg);
      }
    };
    //--------------------------------------------------------------------------
  } // Commands namespace
} // PT namespace

#endif // CMDRELOCATE_H
