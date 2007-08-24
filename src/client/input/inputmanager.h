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

#include <csutil/event.h>
#include <csutil/cfgmgr.h>
#include <csutil/cfgfile.h>
#include <csutil/cfgacc.h>

#include <csutil/hash.h>

#include <string>

namespace PT 
{
  enum keyMap 
  {
    PTKEY_UP = 0,
    PTKEY_DOWN,
    PTKEY_LEFT,
    PTKEY_RIGHT,
    PTKEY_SPACE,
    PTKEY_PAGEUP,
    PTKEY_PAGEDOWN,
    PTKEY_A,
    PTKEY_B,
    PTKEY_C,
    PTKEY_D,
    PTKEY_E,
    PTKEY_F,
    PTKEY_G,
    PTKEY_H,
    PTKEY_I,
    PTKEY_J,
    PTKEY_K,
    PTKEY_L,
    PTKEY_M,
    PTKEY_N,
    PTKEY_O,
    PTKEY_P,
    PTKEY_Q,
    PTKEY_R,
    PTKEY_S,
    PTKEY_T,
    PTKEY_U,
    PTKEY_V,
    PTKEY_W,
    PTKEY_X,
    PTKEY_Y,
    PTKEY_Z,
    PTKEY_ESC,
    PTKEY_SIZE // MUST BE LAST
  };

  class Client;

  class InputManager
  {
  private:
    csHash< std::string, int > functions;

    int InputManager::GetKeyCode (const char* keystring, bool& shift, bool& alt, bool& ctrl);

  public:
    InputManager();

    bool Initialize();

    bool ProcessEvent(iEvent &);
  };
}
#endif
