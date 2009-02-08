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

#include "cmdgroup.h"

#include "client/pointer/pointer.h"
#include "common/reporter/reporter.h"

#include "client/network/network.h"
#include "common/network/netmessage.h"

using namespace PT::Chat;

#ifdef SendMessage
  #undef SendMessage
#endif

cmdGroup::cmdGroup()
{
}

cmdGroup::~cmdGroup() 
{ 
}

bool cmdGroup::CommandHandled (const char* cmd) const
{
  return cmd && ( cmd == std::string("g") || cmd == std::string("group") );
}

StringArray cmdGroup::GetAllCommands () const
{
  StringArray s;
  s.push_back("g");
  s.push_back("group");
  return s;
}

std::string cmdGroup::HelpSynopsis (const char* cmd) const
{ 
  if (strlen(cmd) == 1) return "Send a message to all group members.";
  return "Send a message to all members of the default group."; 
}

std::string cmdGroup::HelpUsage (const char* cmd) const
{ 
  if (strlen(cmd) == 1) return "Usage: '/g <channel name> <message>'";
  return "Usage: '/group <message>'"; 
}

std::string cmdGroup::HelpFull (const char* cmd) const
{
  if (strlen(cmd) == 1) 
    return "Enter the name of a chat group, and a message, and the message "
           "will be sent to all members of that group.";
  return "Send the given message to all members of the default chat group.";
}

void cmdGroup::Execute (const StringArray& args)
{
  // Element 0 is '/', 1 is 'group'
  if (args.size() < 3) throw BadUsage();
  
  std::string channel = PT_DEFAULT_CHAT_GROUP;
  std::string text = "";
  size_t i = 2;

  if (args[1].size() == 1) 
  {
    if (args.size() < 4) throw BadUsage();
    channel = args[2];
    i = 3;
  }

  for(; i < args.size(); i++)
  {
    text += args[i];
    text += " ";
  }

  SendMessage(channel, text); 
}

void cmdGroup::SendMessage(const std::string& channel, const std::string& text)
{
  Network* network = PointerLibrary::getInstance()->getNetwork();
  if(!network) return;

  Report(PT::Debug, "Group: %s", text.c_str());
  GroupMessage msg;
  msg.setMessage(text.c_str());
  msg.setChannel(ptString::create(channel.c_str()));
  network->send(&msg);
}
