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

#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

#include <cssysdef.h>

struct iEvent;

namespace PT
{
  namespace Events
  {
    class EventHandlerCallback
    {
    public:
      virtual bool HandleEvent(iEvent& ev) = 0;
      virtual ~EventHandlerCallback() { };
    };

    template <class Class> class EventHandler : public EventHandlerCallback
    {
    public:
      typedef bool (Class::*Func)(iEvent&);

      EventHandler(Func function, Class *classy)
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

#endif // EVENT_HANDLER_H
