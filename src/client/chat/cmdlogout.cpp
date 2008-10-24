/*
    Copyright (C) 2008 Development Team of Peragro Tempus

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

#include "cmdlogout.h"

#include "client/pointer/pointer.h"

#include "client/user/usermanager.h"

namespace PT
{
  namespace Chat
  {

    cmdLogout::cmdLogout () : CommandDefault("logout") { }

    cmdLogout::~cmdLogout () { }

    std::string cmdLogout::HelpUsage (const char*) const
    { return "'/logout'"; }

    std::string cmdLogout::HelpSynopsis (const char*) const
    { return "Log out."; }

    std::string cmdLogout::HelpFull (const char*) const
    { return "Log out so that you can login on another account/character."; }

    void cmdLogout::Execute (const StringArray& args)
    {
      if (args.size() != 2) throw BadUsage();

      PointerLibrary::getInstance()->getUserManager()->Logout();
    } // cmdLogout::Execute


  } // Chat namespace
} // PT namespace
