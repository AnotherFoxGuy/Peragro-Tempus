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

#include <iostream>
#include <fstream>

#include "file-doors.h"
#include "server/database/database.h"
#include "server/database/table-doors.h"
#include "server/entity/entitymanager.h"
#include "server/server.h"

void DoorsFile::load()
{
  EntityManager* ent_mgr = Server::getServer()->getEntityManager();

  // Loading Doors!
  std::ifstream file ("data/server/peragro_data", std::ios::in|std::ios::ate|std::ios::binary);
  if (file.is_open())
  {
    std::streamsize size = file.tellg();
    char* data = new char [size+1];
    file.seekg (0, std::ios::beg);
    file.read (data, size);
    data[size] = '\0';
    file.close();

    for (std::streamsize i=0; i<size; i++)
    {
      char* name = data+i;

      while (data[++i] != ',');
      data[i] = '\0'; i++;

      char* mesh = data+i;

      while (data[++i] != ',');
      data[i] = '\0'; i++;

      char* str_a = data+i;

      while (data[++i] != ',');
      data[i] = '\0'; i++;

      char* str_b = data+i;

      while (data[++i] != ',');
      data[i] = '\0'; i++;

      char* str_x = data+i;

      while (data[++i] != ',');
      data[i] = '\0'; i++;

      char* str_y = data+i;

      while (data[++i] != ',');
      data[i] = '\0'; i++;

      char* str_z = data+i;

      while (data[++i] != ',');
      data[i] = '\0'; i++;

      char* sector = data+i;

      while (data[++i] > 32);
      data[i] = '\0'; i++;

      ptString ptName(name, strlen(name));

      DoorsTable* doors = Server::getServer()->getDatabase()->getDoorsTable();
      
      DoorsTableVO* vo = doors->getByName(ptName);

      if (vo == 0) // new door!
      {
        vo = new DoorsTableVO();
        vo->id = doors->getCount() + 1; // first door has id 1
        vo->name = ptName;
        vo->isopen = false;
        vo->islocked = true;
        doors->insert(vo);
      }

      DoorEntity* door_ent = new DoorEntity();

      Entity* ent = door_ent->getEntity()->getLock();
      ent->setName(ptString(name, strlen(name)));
      ent->setSector(ptString(sector, strlen(sector)));
      ent->setMesh(ptString(mesh, strlen(mesh)));
      ent->setPos((float)atof(str_x),(float)atof(str_y),(float)atof(str_z));
      ent->freeLock();

      door_ent->setDoorId(vo->id);
      door_ent->setLocked(vo->islocked > 0);
      door_ent->setOpen(vo->isopen > 0);

      ent_mgr->addEntity(ent);

      delete vo;
    }

    delete[] data;
  }
}
