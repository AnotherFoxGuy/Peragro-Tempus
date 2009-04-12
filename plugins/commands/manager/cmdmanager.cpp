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
#include <iutil/stringarray.h>

#include "cmdmanager.h"

using namespace PT::Command;

CS_IMPLEMENT_PLUGIN
SCF_IMPLEMENT_FACTORY(CommandManager)

CommandManager::CommandManager (iBase* parent) :
  scfImplementationType (this, parent), object_reg(0), commands()
{
}

CommandManager::~CommandManager ()
{
}

bool CommandManager::Initialize (iObjectRegistry* objreg)
{
  object_reg = objreg;
  if (!object_reg) return false;

  csRef<iStringArray> classlist = csPtr<iStringArray> (
    iSCF::SCF->QueryClassList ("peragro.commands."));

  while (classlist && classlist->GetSize())
  {
    const char* classname = classlist->Get(0);
    if (classname && classname[0] && strcasecmp(classname, PT_COMMAND_MANAGER_NAME))
    {
      csRef<iClientCommand> plugin = csLoadPluginCheck<iClientCommand> (objreg, classname);
      if (plugin) commands.push_back(plugin);
    }

    classlist->DeleteIndex(0);
  }

  return true;
}

StringArray CommandManager::ParseString(const char* texti)
{
      std::string text;
      StringArray arg;
      size_t beginPos = 0;

      if (!texti) texti = "";
      text = texti;

      // Push the special character on the array and set the offset.
      if ((strncmp (texti,"/",1) == 0) ||
        (strncmp (texti,"!",1) == 0))
      {
        arg.push_back(text.substr(0, 1));
        beginPos = 1;
      } // end if

      std::string args = text.substr(beginPos, text.size());
      std::string tail = args;

      // Push the seperate words on an array.
      while (tail.size() > 0)
      {
        size_t pos = tail.find_first_of(" ");
        if ( pos == std::string::npos )
        {
          arg.push_back(tail.substr(0, tail.size()+1));
          tail.clear();
        }
        else
        {
          // Don't add an empty string, happens with double spaces.
          if (pos != 0)
            arg.push_back(tail.substr(0, pos));
          tail = tail.substr(pos+1, tail.size());
        } // end if
      } // end while

      return arg;
}

std::string CommandManager::ProcessString(const char* texti)
{
  StringArray arg = ParseString(texti);
  if (arg.size() > 1 && arg[0].compare("/") == 0)
    return Execute(arg[1].c_str(), arg);
  else
    return Execute(PT_DEFAULT_COMMAND, arg);
}

std::string CommandManager::Execute(const char* cmd, const StringArray& args)
{
  std::vector<Commandp>::iterator it;
  std::string output;
  if (!cmd) return "No command specified!";

  for (it = commands.begin(); it != commands.end(); it++)
  {
    if ((*it)->CommandHandled(cmd))
    {
      try { output = (*it)->Execute(args); }
      catch (IncorrectParameters& /* error */)
      {
        output = "Usage: " + (*it)->Help(cmd, CMD_HELP_USAGE);
      }
      return output;
    }
  }

  output = "Unknown Command '";
  output += cmd;
  output += "'!";
  return output;
}

const std::vector<Commandp>& CommandManager::GetAllCommands() const
{
  return commands;
}

std::string CommandManager::Complete(const char* cmd)
{
  /* TODO... doesn't do anything right now, just echos back what you entered */
  if (!cmd) return "";
  return cmd;
}
