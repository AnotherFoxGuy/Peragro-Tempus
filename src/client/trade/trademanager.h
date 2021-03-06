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

#ifndef PT_TRADEMANAGER_H
#define PT_TRADEMANAGER_H

#include <cssysdef.h>

#include "common/eventcs/tradeevent.h"
#include "common/eventcs/eventmanager.h"

struct iEvent;

namespace PT
{
  namespace Trade
  {
    class PlayerInventory;
    class Exchange;
    class Buy;

    class TradeManager
    {
    private:
      PT_CALLBACK_HANDLER_LISTENERS

      PlayerInventory* playerInventory;
      Exchange* exchange;
      Buy* buy;

    public:
      TradeManager ();
      ~TradeManager ();

      bool Initialize ();

      PlayerInventory* GetPlayerInventory () { return playerInventory; }
      Exchange* GetExchange () { return exchange; }
      Buy* GetBuy () { return buy; }
    };

  } // Trade namespace
} // PT namespace

#endif // PT_TRADE_MANAGER_H
