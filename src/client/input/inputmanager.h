/*
    Copyright (C) 2007 Development Team of Peragro Tempus

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

#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <csutil/csbaseeventh.h>
#include <csutil/event.h>
#include <csutil/cfgmgr.h>
#include <csutil/cfgfile.h>
#include <csutil/cfgacc.h>

#include <csutil/hash.h>

#include <string>

namespace PT 
{
  class Client;

  class InputManager : public csBaseEventHandler
  {
  private:
    csHash< std::string, int > functions;

    int GetKeyCode (const char* keystring, bool& shift, bool& alt, bool& ctrl);
    bool OnMouse(iEvent& ev);

  public:
    InputManager();

    bool Initialize();

    bool OnKeyboard(iEvent &);
    bool OnMouseDown(iEvent& ev);
    bool OnMouseUp(iEvent& ev);
  };
}
#endif
