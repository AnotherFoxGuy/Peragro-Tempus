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

#include <cssysdef.h>
#include <iutil/objreg.h>
#include <iutil/event.h>
#include <iutil/eventq.h>
#include <csutil/eventnames.h>

#include "eventcmd.h"

using namespace PT::Command;

cmdEventLaunch::cmdEventLaunch (iBase* parent, const char* cmd, const char* evname)
  : ptCommandDefault(parent, cmd), eventname("")
{
  if (evname) eventname = evname;
}

cmdEventLaunch::~cmdEventLaunch () { }

std::string cmdEventLaunch::Execute (const StringArray& args)
{
  if (args.size() != 2) throw PT_EX(IncorrectParameters());

  csRef<iEventQueue> evQueue = csQueryRegistry<iEventQueue> (object_reg);
  csRef<iEventNameRegistry> nameReg = csEventNameRegistry::GetRegistry(object_reg);
  if (!evQueue || !nameReg) return "";

  csEventID eventID = nameReg->GetID(eventname.c_str());

  csRef<iEvent> event = evQueue->CreateBroadcastEvent(eventID);
  if (!event) return "";
  evQueue->Post(event);

  return "";
}
