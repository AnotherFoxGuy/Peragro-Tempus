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

#ifndef ICOMMANDMANAGER_H
#define ICOMMANDMANAGER_H

#include <string>
#include <vector>

#include <csutil/scf.h>
#include <csutil/scf_implementation.h>

#include "include/iclientcommand.h"

#define PT_COMMAND_MANAGER_NAME "peragro.commands.manager"

namespace PT 
{
  namespace Command
  {

    struct iCommandManager : public virtual iBase
    {
      SCF_INTERFACE(iCommandManager, 1,0,0);

      /** process the input string, running a command if appropriate */
      virtual std::string ProcessString(const char*) = 0;

      /** Execute the given command.  First parameter = command name,
          second parameter = arg list */
      virtual std::string Execute (const char*, const StringArray&) = 0;

      /** Returns a list of all commands being handled */
      virtual const std::vector<Commandp>& GetAllCommands() const = 0;

      /** Tries to guess the complete name of a command, given several
          characters.  If there's no match, then the original string is
          returned */
      virtual std::string Complete (const char*) = 0;

    };

  } // Command namespace
} // PT namespace

#endif // ICOMMANDMANAGER_H
