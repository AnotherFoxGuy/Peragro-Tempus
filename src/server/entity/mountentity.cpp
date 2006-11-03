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


#include "server/entity/user.h"
#include "server/entity/character.h"
#include "server/entity/entity.h"
#include "server/entity/pcentity.h"
#include "server/entity/mountentity.h"

#include <time.h>

void MountEntity::addPassanger(const PcEntity* e)
{
  ptMonitor<PcEntity> ent = e->getRef();
  passangers.add(ent); 
}

const PcEntity* MountEntity::getPassanger(size_t i) const
{ 
  if (i > passangers.getCount()) return 0;
  return passangers.get(i).get(); 
}
