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

#include "server/server.h"

#include "server/database/tablemanager.h"
#include "server/database/table-entities.h"

Entity::~Entity()
{
}

void Entity::LoadFromDB()
{
  if(GetId() == Common::Entity::Entity::NoEntity)
    throw PT_EX(InvalidEntity("Invalid entity, id equals NoEntity"));
}

void Entity::SaveToDB()
{
  EntityTable* table = Server::getServer()->GetTableManager()->Get<EntityTable>();
  table->Insert(GetId(), GetType());
}

void Entity::DeleteFromDB()
{
  EntityTable* table = Server::getServer()->GetTableManager()->Get<EntityTable>();
  table->Delete(GetId());
}

/*
unsigned short Entity::GetSector() const
{
  return Server::getServer()->GetSectorManager()->
    GetSectorId(ptString(sectorName.c_str(), strlen(sectorName.c_str()) ));
}

void Entity::SetSector(unsigned short id)
{
  mutex.lock();
  sectorName = *(Server::getServer()->GetSectorManager()->GetSectorName(id));
  mutex.unlock();
}
*/

void Entity::SetSectorName(const std::string& value)
{
/*
  unsigned short id = Server::getServer()->
    GetSectorManager()->GetSectorId(ptString(value.c_str(), strlen(value.c_str()) ));
  if (id == Sector::NoSector)
  {
    printf("Player %s is trying to get to a non existing sector %s!\n", name.c_str(), value.c_str());
  }
  else
*/
  {
    mutex.lock();
    Common::Entity::Entity::SetSectorName(value);
    mutex.unlock();
  }
}
