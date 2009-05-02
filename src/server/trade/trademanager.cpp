/*
    Copyright (C) 2009 Development Team of Peragro Tempus

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

#include "trademanager.h"

#include <stdio.h>

#include "exchange.h"
#include "buy.h"

namespace PT
{
  namespace Trade
  {
    TradeManager::TradeManager()
      : exchange(0), buy(0)
    {
    }

    TradeManager::~TradeManager()
    {
      delete exchange;
      delete buy;
    }

    bool TradeManager::Initialize()
    {
      // Create and Initialize the Exchange.
      delete exchange;
      exchange = new PT::Trade::Exchange ();
      if (!exchange->Initialize())
      {
        printf("Failed to initialize Exchange!");
        return false;
      }

      // Create and Initialize the buy.
      delete buy;
      buy = new PT::Trade::Buy ();
      if (!buy->Initialize())
      {
        printf("Failed to initialize buy!");
        return false;
      }

      return true;
    }

  } // Trade namespace
} // PT namespace
