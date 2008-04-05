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
#include <string>
#include <csutil/scf_implementation.h>

#include <iutil/event.h>
#include <iutil/eventh.h>
#include <iutil/eventq.h>

#include <csutil/refarr.h>

#include <vector> 

#include <boost/shared_ptr.hpp>

#include "eventhandler.h"

#include "common/util/mutex.h"

#ifdef _WINBASE_
#undef _WINBASE_
#endif

namespace PT
{
  namespace Events
  {
    namespace Helper
    {
      /*
      void DisplayEvent(iEvent* event)
      {
        csRef<iEventAttributeIterator> it = event->GetAttributeIterator();
        printf("------------------\n");
        printf("event:\n");
        while (it->HasNext())
        {
          const char* at = it->Next();
          printf("%s\n", at);
        }
        printf("------------------\n");
      }*/
    }

    class EventManager : public scfImplementation1<EventManager, iEventHandler>
    {
    private:
      struct Listener
      {
        csEventID eventId;
        boost::shared_ptr<EventHandlerCallback> handler;
      };

    private:
      csRefArray<iEvent> events;
      std::vector<Listener> listeners;

      Mutex mutex;

      csRef<iEventQueue> eventQueue;
      csRef<iEventNameRegistry> nameRegistry;

      virtual bool HandleEvent(iEvent& ev);
      CS_EVENTHANDLER_NAMES ("peragro.events")
      CS_EVENTHANDLER_NIL_CONSTRAINTS
      //CS_DECLARE_EVENT_SHORTCUTS;

    public:
      EventManager();
      ~EventManager();

      bool Initialize();

      csRef<iEvent> CreateEvent(csEventID eventId, bool fromNetwork = false);

      csRef<iEvent> CreateEvent(const std::string& name, bool fromNetwork = false)
      {
        csRef<iEvent> ev = CreateEvent(Retrieve(name.c_str()), fromNetwork);
        return ev;
      }

      void AddEvent(iEvent* ev);

      void AddListener(csEventID eventId, EventHandlerCallback* handler);
      void AddListener(const std::string& eventId, EventHandlerCallback* handler);

      const char* Retrieve(csStringID id)
      {
        return nameRegistry->GetString(id);
      }

      csStringID Retrieve(const char* id)
      {
        return nameRegistry->GetID(id);
      }


      void Handle();

    };

  } // Events namespace 
} // PT namespace 

#endif // EVENT_MANAGER_H
