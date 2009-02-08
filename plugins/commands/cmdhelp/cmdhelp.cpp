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

#include "cmdhelp.h"

using namespace PT::Command;

CS_IMPLEMENT_PLUGIN
SCF_IMPLEMENT_FACTORY(cmdHelp)

cmdHelp::cmdHelp (iBase* parent) : ptCommandDefault(parent, "help") { }

cmdHelp::~cmdHelp () { }

bool cmdHelp::Initialize (iObjectRegistry* objreg)
{
  ptClientCommand::Initialize(objreg);

  cmdmanager = csLoadPluginCheck<iCommandManager> (objreg, PT_COMMAND_MANAGER_NAME);

  return true;
}

std::string cmdHelp::HelpUsage (const char*) const
{ return "'/help [<command>]'"; }

std::string cmdHelp::HelpSynopsis (const char*) const
{ return "Prints this help."; }

std::string cmdHelp::HelpFull (const char*) const
{ return "Prints a helpful command overview."; }

std::string cmdHelp::Execute (const StringArray& args)
{

  // Element 0 is '/', 1 is 'help'
  if (args.size() < 2 || args.size() > 3) throw BadUsage();

  const char* cmd = NULL;
  if (args.size() == 3) cmd = args[2].c_str();   // '/help cmd'

  if (!cmdmanager) return "";
  const std::vector<Commandp>& commands = cmdmanager->GetAllCommands();

  std::string text = "";

  std::vector<Commandp>::const_iterator it;
  for(it = commands.begin(); it != commands.end(); it++)
  {
    if (cmd)
    {
      if ((*it)->CommandHandled(cmd))
        return (*it)->Help(cmd, CMD_HELP_FULL);
    }
    else
    {
      StringArray cmd_sub = (*it)->GetAllCommands();
      StringArray::const_iterator it_sub;
      for (it_sub = cmd_sub.begin(); it_sub != cmd_sub.end(); it_sub++)
      {
        const char* cmd = it_sub->c_str();
        if (text != "") text += "\n";
        text += *it_sub;
        text += ": " + (*it)->Help(cmd, CMD_HELP_SYNOPSIS);
      } // for
    } // else
  } // for

  return text; 

} // cmdHelp::Execute
