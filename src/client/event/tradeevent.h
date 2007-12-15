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

#ifndef PT_TRADE_EVENT_H
#define PT_TRADE_EVENT_H

#include <cssysdef.h>

#include "client/event/event.h"

namespace PT
{
  namespace Events
  {
    /**
    * Trade event base class.
    */
    class TradeEvent : public Event
    {
    public:
      unsigned int entityId;

    public:
      TradeEvent(EventID name, bool broadCast) : Event(name, broadCast) {}
      virtual ~TradeEvent() {}
    };

    /**
    * TradeEvent helper function.
    */
    template <class T>
    T GetTradeEvent(Eventp ev)
    {
      TradeEvent* tradeEv = static_cast<TradeEvent*> (ev.get());
      if (!tradeEv)
      {
        printf("E: Not a Trade event!\n");
        return 0;
      }
      T tEv = static_cast<T> (tradeEv);
      if (!tEv)
      {
        printf("E: Wasn't listening for this %s event!\n", ev->name.c_str());
        return 0;
      }

      return tEv;
    }

    /**
    * Trade pickup event.
    */
    class TradePickUpEvent : public TradeEvent
    {
    public:
      unsigned int itemId;
      unsigned int variationId;
      unsigned int slotId;

    public:
      TradePickUpEvent() : TradeEvent("trade.pickup", true) {}
      virtual ~TradePickUpEvent() {}
    };

    /**
    * Trade drop event.
    */
    class TradeDropEvent : public TradeEvent
    {
    public:
      //unsigned int itemId;
      unsigned int slotId;

    public:
      TradeDropEvent() : TradeEvent("trade.drop", true) {}
      virtual ~TradeDropEvent() {}
    };

  } // Events namespace 
} // PT namespace 

#endif // PT_TRADE_EVENT_H
