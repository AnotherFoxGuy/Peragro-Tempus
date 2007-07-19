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

#include "trademanager.h"

#include <iutil/objreg.h>

#include "client/pointer/pointer.h"

#include "client/event/eventmanager.h"

namespace PT
{
	namespace Trade
	{

		TradeManager::TradeManager (iObjectRegistry* obj_reg)
		{
			this->obj_reg = obj_reg;

			engine =  csQueryRegistry<iEngine> (obj_reg);

			vfs =  csQueryRegistry<iVFS> (obj_reg);

			vc =  csQueryRegistry<iVirtualClock> (obj_reg);
		}

		TradeManager::~TradeManager ()
		{
		}

		bool TradeManager::Initialize ()
		{
			using namespace PT::Events;

			// Register listener for TradePickEvent.
			EventHandler<TradeManager>* cbPickUp = new EventHandler<TradeManager>(&TradeManager::PickUp, this);
			PointerLibrary::getInstance()->getEventManager()->AddListener("TradePickEvent", cbPickUp);

			return true;
		}

		bool TradeManager::PickUp(PT::Events::Eventp ev)
		{
			using namespace PT::Events;

			TradePickEvent* pickUpEv = GetTradeEvent<TradePickEvent*>(ev);
			if (!pickUpEv) return false;

			// Do something.

			return true;
		}

	} // Trade namespace 
} // PT namespace 
