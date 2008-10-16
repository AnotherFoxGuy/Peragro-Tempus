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

#ifndef CMDQUIT_H
#define CMDQUIT_H

#include "commanddefault.h"

namespace PT
{
  namespace Chat
  {
    //--------------------------------------------------------------------------
    class cmdQuit : public CommandDefault
    {
    public:
      cmdQuit ();
      virtual ~cmdQuit ();

      virtual std::string HelpSynopsis (const char*) const;
      virtual std::string HelpFull (const char*) const;
      virtual std::string HelpUsage (const char*) const;

      virtual void Execute (const StringArray& args);
    };
    //--------------------------------------------------------------------------
  } // Chat namespace
} // PT namespace

#endif // CMDQUIT_H
