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

#ifndef COMMANDDEFAULT_H
#define COMMANDDEFAULT_H

#include "commandiface.h"

namespace PT
{
  namespace Chat
  {
    class CommandDefault : public CommandInterface
    {
    protected:
      std::string cmdname;

    public:
      CommandDefault(const char* cmd);
      virtual ~CommandDefault();

      virtual bool CommandHandled (const char* cmd) const;

      virtual StringArray GetAllCommands() const;

      virtual std::string HelpUsage(const char* cmd) const; 
      virtual std::string HelpSynopsis(const char* cmd) const;
      virtual std::string HelpFull(const char* cmd) const;

      virtual void Execute (const StringArray& args);
    };

  } // Chat namespace
} // PT namespace

#endif // COMMANDDEFAULT_H
