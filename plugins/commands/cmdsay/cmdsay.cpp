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
#include "common/network/chatmessages.h"
#include "client/network/network.h"

#include "cmdsay.h"

using namespace PT::Command;

CS_IMPLEMENT_PLUGIN
SCF_IMPLEMENT_FACTORY(cmdSay)

cmdSay::cmdSay (iBase* parent, const char* cmd)
  : ptCommandDefault(parent, cmd)
{ }

cmdSay::~cmdSay () { }

std::string cmdSay::HelpUsage(const char*) const
{ return "'/say <message>'"; }

std::string cmdSay::HelpSynopsis (const char*) const
{ return "Say something to the world."; }

std::string cmdSay::HelpFull (const char*) const
{ return "Use this command to enter a full message to say to others around you."; }

std::string cmdSay::Execute (const StringArray& args)
{
  Say(2, args);
  return "";
}

void cmdSay::Say(unsigned char volume, const StringArray& args, const char* pre)
{
  // determine start of text string, depending on whether the
  // command was invoked with a '/' or not
  size_t i = 0;
  if (args.size() > 0 && args[0].compare("/") == 0) i = 2;

  if (args.size() <= i) throw PT_EX(IncorrectParameters());

  PointerLibrary* ptrlib = PT::getPointerLibrary(object_reg);
  if (!ptrlib) return;
  Network* network = ptrlib->getNetwork();
  if(!network) return;

  // compose the text string to send
  std::string text = pre;
  for(; i < args.size(); i++)
  {
    if (text != "") text += " ";
    text += args[i];
  }

//  Report(PT::Debug, "Say: %s", text.c_str());

  ChatMessage msg;
  msg.setVolume(volume);
  msg.setMessage(text.c_str());
  network->send(&msg);

}

//--------------------------------------------------------------------------

SCF_IMPLEMENT_FACTORY(cmdShout)

cmdShout::cmdShout (iBase* parent)
  : cmdSay(parent, "shout")
{ }

cmdShout::~cmdShout () { }

std::string cmdShout::HelpSynopsis (const char*) const
{ return "Shout something to the world."; }

std::string cmdShout::HelpUsage (const char*) const
{ return "'/shout <message>'"; }

std::string cmdShout::Execute (const StringArray& args)
{
  Say(20, args);
  return "";
}

//--------------------------------------------------------------------------

SCF_IMPLEMENT_FACTORY(cmdSayMe)

cmdSayMe::cmdSayMe (iBase* parent)
  : cmdSay(parent, "me")
{ }

cmdSayMe::~cmdSayMe () { }

std::string cmdSayMe::HelpSynopsis (const char*) const
{ return "Say something in third person to the world."; }

std::string cmdSayMe::HelpUsage (const char*) const
{ return "'/me <message>'"; }

std::string cmdSayMe::HelpFull (const char*) const
{
  return "Use this command to enter a full message to say to those"
    " around you.  The /me at the beginning will be replaced by the"
    " name of the speaker.";
}

std::string cmdSayMe::Execute (const StringArray& args)
{
  Say(2, args, "/me");
  return "";
}
