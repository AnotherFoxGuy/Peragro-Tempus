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

#ifndef PT_BOOK_EVENT_H
#define PT_BOOK_EVENT_H

#include <cssysdef.h>

#include "client/event/event.h"

namespace PT
{
  namespace Events
  {
    /**
    * Interface event base class.
    */
    class BookEvent : public Event
    {
    public:
      BookEvent(EventID name, bool broadCast) : Event(name, broadCast) {}
      virtual ~BookEvent() {}
    };

    /**
    * InterfaceEvent helper function.
    */
    template <class T>
    T GetBookEvent(Eventp ev)
    {
      BookEvent* bookEv = static_cast<BookEvent*> (ev.get());
      if (!bookEv)
      {
        printf("E: Not a Book event!\n");
        return 0;
      }
      T tEv = static_cast<T> (bookEv);
      if (!tEv)
      {
        printf("E: Wasn't listening for this %s event!\n", ev->name.c_str());
        return 0;
      }

      return tEv;
    }

    /**
    * Book Read Event event.
    */
    class BookReadEvent : public BookEvent
    {
    public:
      std::string title;
      std::string text;
    public:
      BookReadEvent() : BookEvent("book.read", true) {}
      virtual ~BookReadEvent() {}
    };

    /**
    * Book Write Event event.
    */
    class BookWriteEvent : public BookEvent
    {
    public:
      unsigned int variationId;
    public:
      BookWriteEvent() : BookEvent("book.write", true) {}
      virtual ~BookWriteEvent() {}
    };


  } // Events namespace 
} // PT namespace 

#endif // PT_BOOK_EVENT_H
