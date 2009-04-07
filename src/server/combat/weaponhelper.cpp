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

#include "weaponhelper.h"

#include "server/entity/itementity.h"

#include "server/server.h"

unsigned int WeaponHelper::GetReach(boost::shared_ptr<ItemEntity> item)
{
  // TODO This should not be here, just till the DB is setup
  // to correctly set Reach to something...
  return 5;   
}

unsigned int WeaponHelper::GetHeft(boost::shared_ptr<ItemEntity> item)
{
  // TODO This should not be here, just till the DB is setup
  // to correctly set Heft to something...
  return 5;   
}

unsigned int WeaponHelper::GetBaseDamage(boost::shared_ptr<ItemEntity> item)
{  
  return 1; //TODO
}

unsigned int WeaponHelper::GetDamage(boost::shared_ptr<ItemEntity> item)
{  
  return GetBaseDamage(item); //TODO * item->GetQuality()/item->GetMaxQuality(); 
}