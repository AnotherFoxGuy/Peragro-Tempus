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

#include "cmddefault.h"

using namespace PT::Command;

ptCommandDefault::ptCommandDefault(iBase* parent, const char* cmd)
  : ptClientCommand(parent), cmdname(cmd)
{ }

ptCommandDefault::~ptCommandDefault()
{ }

bool ptCommandDefault::CommandHandled (const char* cmd) const
{ return cmd && (cmdname == cmd); }

StringArray ptCommandDefault::GetAllCommands() const
{
  StringArray s;
  s.push_back(cmdname);
  return s;
}

std::string ptCommandDefault::HelpUsage(const char* cmd) const
{ return cmd ? "'/" + cmdname + "'" : "";  }

std::string ptCommandDefault::HelpSynopsis(const char* cmd) const
{ return ""; }

std::string ptCommandDefault::HelpFull(const char* cmd) const
{ return HelpSynopsis(cmd); }

std::string ptCommandDefault::Execute (const StringArray& args)
{ return ""; }
