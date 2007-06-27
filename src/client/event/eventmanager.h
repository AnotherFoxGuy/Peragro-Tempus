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

#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H

#include <cssysdef.h>

#include <vector>
#include <queue>  

#include "event.h"
#include "eventhandler.h"

#include "common/util/mutex.h"

namespace PT
{
  namespace Events
  {
    class EventManager
    {
    private:
      struct Listener
      {
        EventID eventId;
        EventHandlerCallback* handler;
      };

    private:
      std::queue<Event*> events;
      std::vector<Listener> listeners;

      Mutex mutex;

    public:
      EventManager();
      ~EventManager();

      bool Initialize();

      void AddEvent(Event* ev);
      void AddListener(EventID eventId, EventHandlerCallback* handler);

      void Handle();

      // Test stuff
      bool EventManager::TestHandler(Event* ev);

    };

  } // Events namespace 
} // PT namespace 

#endif // EVENT_MANAGER_H
