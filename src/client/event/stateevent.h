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

#ifndef PT_STATE_EVENT_H
#define PT_STATE_EVENT_H

#include <cssysdef.h>

#include "client/event/event.h"

namespace PT
{
  namespace Events
  {
    /**
    * Game states.
    */
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

    /**
    * State event base class.
    */
    class StateEvent : public Event
    {
    public:
      eSTATE newState;

    public:
      StateEvent(EventID name, bool broadCast) : Event(name, broadCast) {}
      virtual ~StateEvent() {}
    };

    /**
    * StateEvent helper function.
    */
    template <class T>
    T GetStateEvent(Eventp ev)
    {
      StateEvent* stateEv = static_cast<StateEvent*> (ev.get());
      if (!stateEv)
      {
        printf("E: Not a State event!\n");
        return 0;
      }
      T tEv = static_cast<T> (stateEv);
      if (!tEv)
      {
        printf("E: Wasn't listening for this %s event!\n", ev->name.c_str());
        return 0;
      }

      return tEv;
    }

    /**
    * State Play event.
    */
    class StateConnectedEvent : public StateEvent
    {
    public:
      StateConnectedEvent() : StateEvent("state.connected", true) 
      { newState = STATE_CONNECTED; }
      virtual ~StateConnectedEvent() {}
    };

    /**
    * State Logged In event.
    */
    class StateLoggedInEvent : public StateEvent
    {
    public:
      bool error;
      std::string errorMessage;
      bool isAdmin;

    public:
      StateLoggedInEvent() : StateEvent("state.loggedin", true) 
      { newState = STATE_LOGGED_IN; }
      virtual ~StateLoggedInEvent() {}
    };

    /**
    * State Play event.
    */
    class StatePlayEvent : public StateEvent
    {
    public:
      unsigned int ownEntityId;

    public:
      StatePlayEvent() : StateEvent("state.play", true) 
      { newState = STATE_PLAY; }
      virtual ~StatePlayEvent() {}
    };

  } // Events namespace 
} // PT namespace 

#endif // PT_STATE_EVENT_H
