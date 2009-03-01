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

#include "entity.h"
#include "entitymanager.h"

#include "pcentity.h"
#include "npcentity.h"
#include "doorentity.h"
#include "itementity.h"
#include "mountentity.h"

#include "user.h"

#include "sectormanager.h"

#include "server/server.h"

Entity::~Entity()
{
  delete (pc_entity.get());
  delete (npc_entity.get());
  delete (item_entity.get());
  delete (door_entity.get());
  delete (mount_entity.get());
}

void Entity::setPlayerEntity(const PcEntity* pc)
{
  pc_entity = pc->getRef();
}

void Entity::setNpcEntity(const NpcEntity* npc)
{
  npc_entity = npc->getRef();
}

void Entity::setItemEntity(const ItemEntity* item)
{
  item_entity = item->getRef();
}

void Entity::setDoorEntity(const DoorEntity* door)
{
  door_entity = door->getRef();
}

void Entity::setMountEntity(const MountEntity* mount)
{
  mount_entity = mount->getRef();
}

unsigned short Entity::GetSector() const
{
  return Server::getServer()->GetSectorManager()->
    GetSectorId(ptString(sectorName.c_str(), strlen(sectorName.c_str()) ));
}

void Entity::SetSector(unsigned short id)
{
  sectorName = *(Server::getServer()->GetSectorManager()->GetSectorName(id));
}

void Entity::SetPosition(const WFMath::Point<3>& p)
{
  Common::Entity::Entity::SetPosition(p);

  // If this is a mount, update passengers aswell.
  if (getMountEntity())
  {
    ptScopedMonitorable<MountEntity> me (getMountEntity());
    for (size_t i = 0; i < me->getPassengerCount(); i++)
    {
      ptScopedMonitorable<Entity> entity (me->getPassenger(i)->getEntity());
      entity->SetPosition(GetPosition());
    } // end for
  } // end if
}

void Entity::SetSectorName(const std::string& value)
{
  unsigned short id = Server::getServer()->
    GetSectorManager()->GetSectorId(ptString(value.c_str(), strlen(value.c_str()) ));
  if (id == Sector::NoSector)
  {
    printf("Player %s is trying to get to a non existing sector %s!\n", name.c_str(), value.c_str());
  }
  else
  {
    Common::Entity::Entity::SetSectorName(value);
  }
}
