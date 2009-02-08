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

#ifndef CLIENTCMD_H
#define CLIENTCMD_H

#include <iutil/comp.h>

#include "include/iclientcommand.h"

namespace PT
{
  namespace Command
  {
    class ptClientCommand : public scfImplementation2<ptClientCommand, iClientCommand, iComponent>
    {
    protected:
      iObjectRegistry* object_reg;

    public:
      ptClientCommand(iBase* parent);
      virtual ~ptClientCommand();

      // from iComponent.
      virtual bool Initialize (iObjectRegistry*);

      // virtual bool CommandHandled (const char* cmd) const = 0;
      // virtual StringArray GetAllCommands() const = 0;
      // virtual std::string HelpUsage(const char* cmd) const = 0;
      // virtual std::string HelpSynopsis(const char* cmd) const = 0;
      // virtual std::string HelpFull(const char* cmd) const = 0;
      // virtual std::string Execute (const StringArray& args) = 0;

      std::string Help (const char* cmd, int detail) const;

    };

  } // Command namespace
} // PT namespace

#endif // CLIENTCMD_H
