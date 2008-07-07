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
      CommandDefault(const char* cmd) : CommandInterface(), cmdname(cmd) {}
      virtual ~CommandDefault() { };

      virtual bool CommandHandled (const char* cmd) const 
      { return cmdname == cmd; }

      virtual StringArray GetAllCommands() const
      { StringArray s;  s.push_back(cmdname);  return s; }

      virtual const char* HelpUsage(const char* cmd) const 
      { std::string s = "'/" + cmdname + "'";  return s.c_str(); }

      virtual const char* HelpSynopsis(const char* cmd) const { return ""; }

      virtual const char* HelpFull(const char* cmd) const 
      { return HelpSynopsis(cmd); }

      virtual void Execute (const StringArray& args) {}
    };

  } // Chat namespace
} // PT namespace

#endif // COMMANDDEFAULT_H
