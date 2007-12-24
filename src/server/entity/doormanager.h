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

#ifndef _DOORMANAGER_H_
#define _DOORMANAGER_H_

#include "common/util/array.h"
#include "doorentity.h"
#include "server/database/table-doors.h"
#include "server/server.h"

class DoorManager
{
private:
  Array<DoorEntity*> doors;

public:
  DoorManager() {}

  ~DoorManager() { doors.delAll(); }

  size_t getDoorCount()
  {
    return doors.getCount();
  }

  DoorEntity* getDoor(size_t index)
  {
    return doors.get(index);
  }

  void addDoor(DoorEntity* door)
  {
    doors.add(door);
  }

  void delDoor(DoorEntity* door)
  {
    if (!door) return;
    for (size_t i = 0; i<doors.getCount(); i++)
    {
      DoorEntity* _door = doors.get(i);
      if (_door->getDoorId() == door->getDoorId())
      {
        doors.remove(i);
        return;
      }
    }
  }

  bool exists(DoorEntity* door)
  {
    if (!door) return false;
    for (size_t i = 0; i<doors.getCount(); i++)
    {
      DoorEntity* _door = doors.get(i);

      if (_door->getDoorId() == door->getDoorId())
        return true;
    }
    return false;
  }

  DoorEntity* findById(unsigned int id)
  {
    for (size_t i = 0; i<doors.getCount(); i++)
    {
      DoorEntity* door = doors.get(i);
      if (door->getDoorId() == id)
      {
        return door;
      }
    }
    return 0;
  }

  void loadFromDB(DoorsTable* dt)
  {
    //Load all Doors from Database
    Array<DoorsTableVO*> loadedDoors=dt->getAll();
    for (size_t i = 0; i<loadedDoors.getCount(); i++)
    {
      DoorEntity* door=new DoorEntity();
      door->setDoorId(loadedDoors[i]->id);
      door->setLocked(loadedDoors[i]->islocked > 0);
      door->setOpen(loadedDoors[i]->isopen > 0);
      door->setAnimation(loadedDoors[i]->animation);

      EntityManager* ent_mgr = Server::getServer()->getEntityManager();
      DoorEntity* door_ent = new DoorEntity();
      Entity* ent = door_ent->getEntity()->getLock();

      ent->setName(loadedDoors[i]->name);
      ent->setSector(ptString(*loadedDoors[i]->sector,strlen(*loadedDoors[i]->sector)));
      ent->setMesh(loadedDoors[i]->mesh);
      ent->setPos(loadedDoors[i]->x,loadedDoors[i]->y,loadedDoors[i]->z);
      ent->freeLock();

      door_ent->setDoorId(loadedDoors[i]->id);
      door_ent->setLocked(loadedDoors[i]->islocked > 0);
      door_ent->setOpen(loadedDoors[i]->isopen > 0);
      door_ent->setAnimation(loadedDoors[i]->animation);
      ent_mgr->addEntity(ent);

      doors.add(door);
    }
  }

  void clear()
  {
    doors.removeAll();
  }
};

#endif // _DOORMANAGER_H_
