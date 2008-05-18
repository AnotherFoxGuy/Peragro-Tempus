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
/**
 * @file eventmanager.h
 *
 * @brief The Event manager class.
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
#include <csutil/weakref.h>
#include <csutil/parray.h>

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
    /**
     * @ingroup events
     * The event manager.
     */
    class EventManager : public scfImplementation1<EventManager, iEventHandler>
    {
    private:
      struct Listener;

      /// The events waiting to be queued.
      csRefArray<iEvent> events;
      /// The current listeners.
      csRefArray<Listener> listeners;

      /**
       * Event listener.
       */
      struct Listener : public scfImplementation1<Listener, iEventHandler>
      {
        /// The event manager.
        EventManager* evmgr;

        /// Constructor.
        Listener(EventManager* evmgr, csEventID eventId, EventHandlerCallback* handler);
        /// Destructor.
        ~Listener();

        /**
         * Handle an event.
         * @param ev The event.
         * @return Whether the event was handled.
         */
        virtual bool HandleEvent(iEvent& ev);

        /// The event ID.
        csEventID eventId;
        /// The event handler.
        csWeakRef<EventHandlerCallback> handler;

        CS_EVENTHANDLER_NAMES ("peragro.listener")
        CS_EVENTHANDLER_NIL_CONSTRAINTS
      };
      friend struct Listener;

      /// The pointer library.
      PointerLibrary* pointerlib;

      /// A mutex used while accessing the event queue.
      Mutex mutex;

      /// Handle the next event in the queue.
      void Handle();

      /// The queue of events waiting to be handled.
      csRef<iEventQueue> eventQueue;
      /// The event name registry, used to convert event names to IDs and back.
      csRef<iEventNameRegistry> nameRegistry;

      /// Handle the "crystalspace.frame" event, for the Handle() function.
      virtual bool HandleEvent(iEvent& ev);

      CS_EVENTHANDLER_NAMES ("peragro.events")
      CS_EVENTHANDLER_NIL_CONSTRAINTS

    public:
      /// Constructor.
      EventManager();
      /// Destructor.
      ~EventManager();

      /**
       * Initialize the event manager.
       * @param The pointer library.
       */
      bool Initialize(PointerLibrary* pl);

      /**
       * Create a new event from an event ID.
       * @param eventId ID of the event to create.
       * @param fromNetwork Whether the event is from the network, or local.
       * @return A pointer to the new event.
       */
      csRef<iEvent> CreateEvent(csEventID eventId, bool fromNetwork = false);

      /**
       * Create a new event from an event name.
       * @param name Name of the event to create.
       * @param fromNetwork Whether the event is from the network, or local.
       * @return A pointer to the new event.
       */
      csRef<iEvent> CreateEvent(const std::string& name, bool fromNetwork = false)
      {
        csRef<iEvent> ev = CreateEvent(Retrieve(name.c_str()), fromNetwork);
        return ev;
      }

      /**
       * Add an event to the event queue.
       * @param ev The event.
       */
      void AddEvent(iEvent* ev);

      /**
       * Add a listener to an event by event ID.
       * @param eventId The event ID.
       * @param handler An event handler callback.
       */
      void AddListener(csEventID eventId, EventHandlerCallback* handler);

      /**
       * Add a listener to an event by event name.
       * @param eventId The event name.
       * @param handler An event handler callback.
       */
      void AddListener(const std::string& eventId, EventHandlerCallback* handler);

      /**
       * Remove a specific listener.
       * @param handler The listener to remove.
       */
      void RemoveListener (Listener* handler);

      /**
       * Remove a handler for _all_ events.
       * @param handler The handler to remove.
       */
      void RemoveListener (EventHandlerCallback* handler);

      /**
       * Remove a handler for a specific event.
       * @param handler The handler to remove.
       * @param eventId The event ID of the handler.
       */
      void RemoveListener (EventHandlerCallback* handler, csEventID eventId);

      /**
       * Retrieve the event name of an event ID.
       * @param id The event ID.
       * @return The event name.
       */
      const char* Retrieve(csStringID id)
      {
        return nameRegistry->GetString(id);
      }

      /**
       * Retrieve the event ID of an event name.
       * @param id The event name.
       * @return The event ID.
       */
      csStringID Retrieve(const char* id)
      {
        return nameRegistry->GetID(id);
      }

      /**
       * Determine whether the first ID is descended from the second.
       * @param child The child event ID.
       * @param parent The parent event ID.
       * @return True if the name of the first ID is equivalent to or a child
       * of the name of the second ID, for example "input.move" and "input".
       */
      bool IsKindOf(csStringID child, csStringID parent)
      {
        return nameRegistry->IsKindOf(child, parent);
      }

      /**
       * Determine whether the string of the ID parameter is descended from
       * the string parameter.
       * @param child The child event ID.
       * @param parent The parent event ID.
       * @return True if the name of the ID is equivalent to or a child of
       * the parent parameter, for example "input.move" and "input".
       */
      bool IsKindOf(csStringID child, const char* parent)
      {
        return IsKindOf(child, Retrieve(parent));
      }

    };

  } // Events namespace
} // PT namespace

#endif // EVENT_MANAGER_H
