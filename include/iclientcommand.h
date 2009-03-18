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

#ifndef ICLIENTCOMMAND_H
#define ICLIENTCOMMAND_H

#include <string>
#include <vector>

#include <csutil/scf.h>
#include <csutil/scf_implementation.h>

namespace PT
{
  namespace Command
  {
    typedef std::vector<std::string> StringArray;

    enum
    {
      CMD_HELP_USAGE,      // command usage message
      CMD_HELP_SYNOPSIS,   // simple one-line command synopsis
      CMD_HELP_FULL        // in-depth help message
    };

    /* exception thrown by iClientCommand::Execute when bad command
       syntax is used */
    struct BadUsage {};

    struct iClientCommand : public virtual iBase
    {
      SCF_INTERFACE(iClientCommand, 1,0,0);

      /** Returns true if cmd is handled by this object. */
      virtual bool CommandHandled (const char* cmd) const = 0;

      /** Returns a list of all commands handled by this object. */
      virtual StringArray GetAllCommands() const = 0;

      virtual std::string HelpUsage(const char* cmd) const = 0;
      virtual std::string HelpSynopsis(const char* cmd) const = 0;
      virtual std::string HelpFull(const char* cmd) const = 0;

      /** Returns a help message.
        * cmd is the name of the command to return help for, and detail is
        * one of CMD_HELP_USAGE, CMD_HELP_SYNOPSIS, and CMD_HELP_FULL,
        * depending on how detailed a help message is expected.
        */
      virtual std::string Help (const char* cmd, int detail) const = 0;

      /** Execute the command.
        * args[0] is '/', args[1] is the command name, for normal commands.
        * return value is an (optional) output string
        */
      virtual std::string Execute (const StringArray& args) = 0;

    };

    typedef csRef<iClientCommand> Commandp;

  } // Command namespace
} // PT namespace

#endif // ICLIENTCOMMAND_H
