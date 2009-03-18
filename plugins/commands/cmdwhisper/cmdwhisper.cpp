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
#include <iutil/event.h>
#include <iutil/eventq.h>
#include <csutil/eventnames.h>

#include "include/ipointerplug.h"
#include "client/pointer/pointer.h"
#include "common/network/chatmessages.h"
#include "client/network/network.h"

#include "cmdwhisper.h"

using namespace PT::Command;

CS_IMPLEMENT_PLUGIN
SCF_IMPLEMENT_FACTORY(cmdWhisper)

cmdWhisper::cmdWhisper (iBase* parent)
  : ptCommandDefault(parent, "whisper")
{ }

cmdWhisper::~cmdWhisper () { }

std::string cmdWhisper::HelpSynopsis (const char*) const
{ return "Start a private conversation with the target."; }

std::string cmdWhisper::HelpUsage (const char*) const
{ return "'/whisper <target> <message>'"; }

std::string cmdWhisper::HelpFull (const char*) const
{
   return "Enter the name of a person to speak to, and a message to send"
    " to them, and a private conversation will open up with them, if"
    " they are available.";
}

std::string cmdWhisper::Execute (const StringArray& args)
{

  // Element 0 is '/', 1 is 'whisper'
  if (args.size() < 4) throw BadUsage();

  // nick of the person you're whispering to
  std::string nick = args[2];

  // construct the text string to send
  std::string text;
  for(size_t i = 3; i < args.size(); i++)
  {
    text += args[i];
    text += " ";
  }

  PointerLibrary* ptrlib = PT::getPointerLibrary(object_reg);
  if (!ptrlib) return "";
  Network* network = ptrlib->getNetwork();
  if(!network) return "";

  // Send the whisper to the network.
  WhisperToMessage nmsg;
  nmsg.setListenerName(ptString(nick.c_str(), nick.size()));
  nmsg.setMessage(text.c_str());
  network->send(&nmsg);

  // Generate an event to add your own text to the whisper window.
  csRef<iEventQueue> evQueue = csQueryRegistry<iEventQueue> (object_reg);
  csRef<iEventNameRegistry> nameReg = csEventNameRegistry::GetRegistry(object_reg);
  if (!evQueue || !nameReg) return "";
  csEventID eventID = nameReg->GetID("chat.whisper");
  csRef<iEvent> event = evQueue->CreateBroadcastEvent(eventID);
  if (!event) return "";

  event->Add("speakerName", "");
  event->Add("targetName", nick.c_str());
  event->Add("message", text.c_str());
  evQueue->Post(event);

  return "";
}
