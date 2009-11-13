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

#include <cssysdef.h>
#include <iutil/objreg.h>
#include <iutil/plugin.h>

#include "include/ipointerplug.h"
#include "client/pointer/pointer.h"
#include "common/network/entitymessages.h"
#include "client/network/network.h"

#include "cmdrelocate.h"

using namespace PT::Command;

SCF_IMPLEMENT_FACTORY(cmdRelocate)

cmdRelocate::cmdRelocate (iBase* parent)
  : ptCommandDefault(parent, "relocate")
{ }

cmdRelocate::~cmdRelocate () { }

std::string cmdRelocate::HelpSynopsis (const char*) const
{ return "Teleport yourself to the start location."; }

std::string cmdRelocate::HelpUsage (const char*) const
{ return "Usage: '/relocate'"; }

std::string cmdRelocate::Execute (const StringArray& args)
{
  // Element 0 is '/', 1 is 'relocate'
  if (args.size() < 2) throw PT_EX(IncorrectParameters());

  PointerLibrary* ptrlib = PT::getPointerLibrary(object_reg);
  if (!ptrlib) return "";
  Network* network = ptrlib->getNetwork();
  if(!network) return "";

  RelocateMessage msg;
  network->send(&msg);
  return "";
}
