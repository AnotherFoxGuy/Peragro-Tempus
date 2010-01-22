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

#ifndef CMDMANAGER_H
#define CMDMANAGER_H

#include <iutil/comp.h>

#include "include/icommandmanager.h"

#define PT_DEFAULT_COMMAND "say"

namespace PT
{
  namespace Command
  {
    class CommandManager : public scfImplementation2<CommandManager, iCommandManager, iComponent>
    {
    private:
      iObjectRegistry* object_reg;
      std::vector<Commandp> commands;

      StringArray ParseString(const char* texti);

    public:
      CommandManager(iBase* parent);
      virtual ~CommandManager();

      // from iComponent.
      virtual bool Initialize (iObjectRegistry*);

      // from iCommandManager
      virtual bool Initialize ();
      virtual std::string ProcessString(const char* texti);
      virtual std::string Execute(const char* cmd, const StringArray& args);
      virtual const std::vector<Commandp>& GetAllCommands() const;
      virtual std::string Complete(const char* cmd);

    };

  } // Command namespace
} // PT namespace

#endif // CMDCOMMAND_H
