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

#include "server/entity/mountentity.h"

#include <wfmath/point.h>
#include <wfmath/vector.h>

#include "server/entity/user.h"
#include "server/entity/character/character.h"
#include "server/entity/pcentity.h"

void MountEntity::AddPassenger(boost::shared_ptr<Character> e)
{
  passengers.push_back(e);
}

void MountEntity::RemovePassenger(boost::shared_ptr<Character> e)
{
  passengers.remove_if(Equal(e));
}

void MountEntity::SetPosition(const WFMath::Point<3>& p)
{
  Entity::SetPosition(p);
  std::list<boost::weak_ptr<Character> >::const_iterator it;
  for ( it=passengers.begin() ; it != passengers.end(); it++ )
  {
    if ((*it).lock())
      (*it).lock()->SetPosition(GetPosition());
  } // end for
}

void MountEntity::LoadFromDB()
{
  Character::LoadFromDB();
}

void MountEntity::SaveToDB()
{
  Character::SaveToDB();
}
