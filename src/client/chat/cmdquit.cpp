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

#include "cmdquit.h"

#include "client/pointer/pointer.h"

#include "client/client.h"

namespace PT
{
  namespace Chat
  {

    cmdQuit::cmdQuit () : CommandDefault("quit") { }

    cmdQuit::~cmdQuit () { }

    std::string cmdQuit::HelpUsage (const char*) const
    { return "'/quit'"; }

    std::string cmdQuit::HelpSynopsis (const char*) const
    { return "Quit the game."; }

    std::string cmdQuit::HelpFull (const char*) const
    { return "Logs out from the game and closes the window."; }

    void cmdQuit::Execute (const StringArray& args)
    {
      if (args.size() != 2) throw BadUsage();

      PointerLibrary::getInstance()->getClient()->Quit();
    } // cmdQuit::Execute


  } // Chat namespace
} // PT namespace
