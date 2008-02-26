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

#ifndef STATEMANAGER_H_
#define STATEMANAGER_H_

namespace PT
{
  enum eSTATE
  {
    STATE_INITIAL = 0,
    STATE_INTRO = 1,
    STATE_CONNECTED = 2,
    STATE_LOGGED_IN = 3,
    STATE_SELECTING_CHAR = 4,
    STATE_PLAY = 5,
    STATE_RECONNECTED = 6
  };

  class StateManager
  {
  private:
    eSTATE state;

  public:
    StateManager();
    ~StateManager();

    bool Initialize();

    ///Get the current state of the game.
    eSTATE GetState()
    {
      return state;
    }

    ///Set which state of the game we are in.
    void SetState(eSTATE v)
    {
      state = v;
    }

  };
}

#endif // STATEMANAGER_H_
