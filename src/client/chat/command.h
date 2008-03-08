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

#ifndef COMMAND_H
#define COMMAND_H

#include <cssysdef.h>

#include <string>
#include <vector>
#include <queue> 

#include <boost/shared_ptr.hpp>

namespace PT
{
  namespace Chat
  {
    typedef std::vector<std::string> StringArray;

    class Command
    {
    public:
      virtual const char* GetCommand () = 0;
      virtual const char* GetDescription () = 0;
      virtual void Help () = 0;
      virtual void Execute (const StringArray& args) = 0;
      virtual ~Command() { };
    };

    typedef boost::shared_ptr<Command> Commandp;

  } // Chat namespace 
} // PT namespace 

#endif // COMMAND_H
