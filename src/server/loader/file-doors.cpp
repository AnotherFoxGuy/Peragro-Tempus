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

#include "ext/tinyxml/tinyxml.h"

void DoorsFile::load()
{
  // If the XML is not consistant, we just segfault!
  EntityManager* ent_mgr = Server::getServer()->getEntityManager();

  TiXmlDocument doc;
  if (!doc.LoadFile("data/xml/doors/doors.xml"))
    return;

  TiXmlElement* items = doc.FirstChildElement("doors");

  TiXmlElement* itemNode = items->FirstChildElement("door");
  while (itemNode)
  {
    // --[Parsing Data]------------------------------------------------------

    unsigned int door_id = atoi(itemNode->FirstChildElement("id")
      ->FirstChild()->ToText()->Value());
    const char* name = itemNode->FirstChildElement("name")
      ->FirstChild()->ToText()->Value();
    const char* mesh = itemNode->FirstChildElement("mesh")
      ->FirstChild()->ToText()->Value();
    const char* sector = itemNode->FirstChildElement("sector")
      ->FirstChild()->ToText()->Value();
    //unsigned int key = atoi(itemNode->FirstChildElement("keyId")
    //  ->FirstChild()->ToText()->Value());

    const char* str_x = itemNode->FirstChildElement("position")->Attribute("x");
    const char* str_y = itemNode->FirstChildElement("position")->Attribute("y");
    const char* str_z = itemNode->FirstChildElement("position")->Attribute("z");

    // --[Creating Entity]---------------------------------------------------

    DoorsTable* doors = Server::getServer()->getDatabase()->getDoorsTable();
    
    DoorsTableVO* vo = doors->getById(door_id);

    if (vo == 0) // new door!
    {
      vo = new DoorsTableVO();
      vo->id = door_id;
      vo->name = ptString(name, strlen(name));
      vo->isopen = false;
      vo->islocked = true;
      vo->sector = ptString(sector, strlen(sector));
      vo->mesh = ptString(mesh, strlen(mesh));
      vo->x = atof(str_x);
      vo->y = atof(str_y);
      vo->z = atof(str_z);
      doors->insert(vo);

      // If it's not new then we have already loaded it from the database
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
    }

    delete vo;

    itemNode = itemNode->NextSiblingElement("door");
  }
}
