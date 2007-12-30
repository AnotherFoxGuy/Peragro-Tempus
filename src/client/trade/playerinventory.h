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

#ifndef PT_PLAYERINVENTORY_H
#define PT_PLAYERINVENTORY_H

#include <cssysdef.h>

#include "client/event/tradeevent.h"

namespace PT
{
  namespace Trade
  {
    class PlayerInventory
    {
    public:
      bool Add(PT::Events::Eventp ev);

    public:
      PlayerInventory ();
      ~PlayerInventory ();

      bool Initialize ();
    };

  } // Trade namespace 
} // PT namespace 

#endif // PT_PLAYERINVENTORY_H
