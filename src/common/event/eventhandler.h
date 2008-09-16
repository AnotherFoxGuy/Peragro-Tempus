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
 * @file eventhandler.h
 *
 * @brief Event handler template.
 */

#ifndef EVENT_HANDLER_CALLBACK_H
#define EVENT_HANDLER_CALLBACK_H


#undef CALLBACK_HANDLER_LISTENERS
#define CALLBACK_HANDLER_LISTENERS                            \
  csRefArray<PT::Events::EventHandlerCallback> eventHandlers; \

#undef SETUP_HANDLER
#define SETUP_HANDLER                                                   \
using namespace PT::Events;                                             \
EventManager* evmgr = PointerLibrary::getInstance()->getEventManager(); \

#undef SETUP_HANDLER_PLUGIN
#define SETUP_HANDLER_PLUGIN(pointerlib)             \
using namespace PT::Events;                          \
EventManager* evmgr = pointerlib->getEventManager(); \

/**
 * A macro to help register an eventhandler.
 * @param Class The class which is registering the eventhandler.
 * @param funct The function which will handle the event.
 * @param ev The event which to subscribe to.
 */
#undef REGISTER_LISTENER
#define REGISTER_LISTENER(Class, funct, ev)           \
REGISTER_LISTENER2(Class, funct, ev, FUNC(funct))     \

/**
 * A macro to help register an entity eventhandler.
 * @param Class The class which is registering the eventhandler.
 * @param funct The function which will handle the event.
 * @param ev The event which to subscribe to.
 * @param specific Whether or not to make this event entity specific.
 */
#undef REGISTER_LISTENER_ENTITY
#define REGISTER_LISTENER_ENTITY(Class, funct, ev, specific)  \
REGISTER_LISTENER_E2(Class, funct, ev, specific, FUNC(funct)) \

// Some macro magic to generate unique variable names.
#define FUNC2(x,y) cb##x##y
#define FUNC1(x,y) FUNC2(x,y)
#ifdef __COUNTER__
  #define FUNC(x) FUNC1(x,__COUNTER__)
#else
  #define FUNC(x) FUNC1(x,__LINE__)
#endif

//-------------------------------------------------------------------------------------
#define REGISTER_LISTENER2(Class, funct, ev, var)                                     \
using namespace PT::Events;                                                           \
csRef<EventHandlerCallback> var;                                                      \
var.AttachNew(new EventHandler</**/Class>(&/**/Class::/**/funct, this));              \
evmgr->AddListener(ev, var);                                                          \
eventHandlers.Push(var);                                                              \
//-------------------------------------------------------------------------------------
#define REGISTER_LISTENER_E2(Class, funct, ev, specific, var)                         \
using namespace PT::Events;                                                           \
csRef<EventHandlerCallback> var;                                                      \
var.AttachNew(new EventHandler</**/Class>(&/**/Class::/**/funct, this));              \
if (specific)                                                                         \
 evmgr->AddListener(EntityHelper::MakeEntitySpecific(ev, entity->GetId()), var);      \
else                                                                                  \
 evmgr->AddListener(ev, var);                                                         \
eventHandlers.Push(var);                                                              \
//-------------------------------------------------------------------------------------


#include <cssysdef.h>
#include <csutil/scf_implementation.h>

struct iEvent;

namespace PT
{
  namespace Events
  {
    /**
     * @ingroup events
     * The interface to a to an event handler callback class.
     */
    struct EventHandlerCallback : public virtual iBase
    {
      SCF_INTERFACE(EventHandlerCallback, 1,0,0);

      /**
       * Handle an event, by calling the stored function pointer.
       * @param ev The event.
       * @return Whether the event was handled.
       */
      virtual bool HandleEvent(iEvent& ev) = 0;
    };

    /**
     * @ingroup events
     * The event handler callback template.
     */
    template <class Class>
    class EventHandler : public scfImplementation1<EventHandler<Class>, EventHandlerCallback>
    {
    public:
      /// Function pointer typedef, of class "Class" named "Func".
      typedef bool (Class::*Func)(iEvent&);

      /**
       * Constructor. Creates a handler of the class for the function.
       * @param function A function pointer of the class.
       * @param classy The class.
       */
      EventHandler(Func function, Class *classy) :
                   scfImplementation1<EventHandler<Class>,
                                      EventHandlerCallback> (this)
      {
        theclass = classy;
        thefunc = function;
      }

      /**
       * Handle an event, by calling the stored function pointer.
       * @param ev The event.
       * @return Whether the event was handled.
       */
      bool HandleEvent(iEvent& ev)
      {
        return (theclass->*thefunc)(ev);
      }

      /// A pointer to the class to handle events for.
      Class *theclass;
      /// The function pointer.
      Func thefunc;
    };
  } // Events namespace
} // PT namespace

#endif // EVENT_HANDLER_CALLBACK_H
