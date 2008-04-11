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

#ifndef EVENT_HANDLER_CALLBACK_H
#define EVENT_HANDLER_CALLBACK_H

#include <cssysdef.h>
#include <csutil/scf_implementation.h>

#include "eventmanager.h"

#include "client/reporter/reporter.h"
#include "client/pointer/pointer.h"

struct iEvent;

namespace PT
{
  namespace Events
  {

    struct EventHandlerCallback : public virtual iBase
    {
      SCF_INTERFACE(EventHandlerCallback, 1,0,0);
      virtual bool HandleEvent(iEvent& ev) = 0;
    };

    template <class Class>
    class EventHandler : public scfImplementation1<EventHandler<Class>, EventHandlerCallback>
    {
    public:
      typedef bool (Class::*Func)(iEvent&);

      EventHandler(Func function, Class *classy) :
                   scfImplementation1<EventHandler<Class>,
                                      EventHandlerCallback> (this)
      {
        theclass = classy;
        thefunc = function;
      }

      bool HandleEvent(iEvent& ev)
      {
        return (theclass->*thefunc)(ev);
      }

      Class *theclass;
      Func thefunc;
    };
  } // Events namespace
} // PT namespace

#endif // EVENT_HANDLER_CALLBACK_H
