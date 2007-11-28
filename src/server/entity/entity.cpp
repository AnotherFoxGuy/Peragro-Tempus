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

const ptString& Entity::getSectorName() const
{
  return Server::getServer()->getSectorManager()->getSectorName(sector_id);
}

void Entity::setSector(unsigned short id)
{
  if (pc_entity.get() && pc_entity.get()->getUser())
  {
    if (sector_id != id)
    {
      SectorManager* sectorMgr = Server::getServer()->getSectorManager();
      const ptString& old_region = sectorMgr->getRegionName(sector_id);
      const ptString& new_region = sectorMgr->getRegionName(id);

      sector_id = id;

      if ( ! ( old_region == new_region ) ) 
      {
        EntityManager* ent_mgr = Server::getServer()->getEntityManager();
        ent_mgr->lock();
        for (size_t i = 0; i < ent_mgr->getEntityCount(); i++)
        {
          const ptString& region = sectorMgr->getRegionName(ent_mgr->getEntity(i)->getSector());

          User* user = pc_entity.get()->getUser()->getLock();
          if (region == old_region)
          {
            user->sendRemoveEntity(ent_mgr->getEntity(i));
          }
          else if (region == new_region)
          {
            user->sendAddEntity(ent_mgr->getEntity(i));
          }
          user->freeLock();
        }
        ent_mgr->unlock();
      }
    }
  }
  sector_id = id;
}

void Entity::setSector(ptString name)
{
  unsigned short id = Server::getServer()->getSectorManager()->getSectorId(name);
  if (id == Sector::NoSector)
  {
    printf("Player %s is trying to get to a non existing sector!\n", *name_id);
  }
  else
  {
    setSector(id);
  }
}
