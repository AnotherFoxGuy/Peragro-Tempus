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

#ifndef PT_TRADE_MANAGER_H
#define PT_TRADE_MANAGER_H

#include <cssysdef.h>
#include <csutil/ref.h>
#include <iutil/vfs.h>
#include <iutil/virtclk.h>
#include <iengine/engine.h>
#include <csgeom/path.h>
#include <csgeom/math3d.h>
#include <iengine/camera.h>
#include <iutil/object.h>

#include <physicallayer/pl.h>

#include "client/event/tradeevent.h"

struct iObjectRegistry;

namespace PT
{
	namespace Trade
	{
		class TradeManager
		{
		private:
			csRef<iEngine> engine;
			csRef<iVFS> vfs;
			csRef<iVirtualClock> vc;
			csRef<iCelPlLayer> pl;
			csRef<iObjectRegistry> obj_reg;

		public:
			bool PickUp(PT::Events::Eventp ev);

		public:
			TradeManager (iObjectRegistry* obj_reg);
			~TradeManager ();

			bool Initialize ();
		};

	} // Trade namespace 
} // PT namespace 

#endif // PT_TRADE_MANAGER_H
