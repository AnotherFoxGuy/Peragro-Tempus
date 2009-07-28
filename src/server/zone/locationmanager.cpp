/*
    Copyright (C) 2008 Development Team of Peragro Tempus

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

#include "common/util/math.h"

#include "common/database/database.h"
#include "server/database/tablemanager.h"
#include "server/database/table-locations.h"
#include "server/server.h"

#include "locationmanager.h"

void LocationManager::delAll()
{
  Server::getServer()->GetTableManager()->Get<LocationsTable>()->DeleteAll();
  locations.clear();
}

void LocationManager::LoadFromDB()
{
  LocationsTable* locationsTable = Server::getServer()->GetTableManager()->Get<LocationsTable>();
  LocationsTableVOArray rows = locationsTable->GetAll();
  for (size_t i = 0; i < rows.size(); ++i)
  {
    Location location;
    location.name = rows[i]->name;
    location.location = rows[i]->location;
    AddLocation(rows[i]->id, location);
  }
}

std::string LocationManager::GetName(size_t locationId)
{
  std::map<size_t, Location>::const_iterator it;
  it = locations.find(locationId);
  if (it != locations.end())
    return it->second.name;
  else
    return 0;
}

WFMath::Point<3> LocationManager::GetLocation(size_t locationId)
{
  std::map<size_t, Location>::const_iterator it;
  it = locations.find(locationId);
  if (it != locations.end())
    return it->second.location;
  else
    return 0;
}

WFMath::Point<3> LocationManager::GetLocation(const std::string& name)
{
  std::map<size_t, Location>::iterator it;
  for (it = locations.begin(); it != locations.end(); it++)
  {
    if ( it->second.name.compare(name) == 0 )
    {
      return it->second.location;
    }
  }
  printf("Warning Location %s not found\n",name.c_str());
  return 0;
}

size_t LocationManager::GetId(const std::string& name)
{
  std::map<size_t, Location>::iterator it;
  for (it = locations.begin(); it != locations.end(); it++)
  {
    if ( it->second.name.compare(name) == 0 )
    {
      return it->first;
    }
  }
  printf("Warning Location %s not found\n",name.c_str());
  return 0;
}

void LocationManager::Insert(size_t locationId, std::string name, WFMath::Point<3> location)
{

  LocationsTable* locationsTable = Server::getServer()->GetTableManager()->Get<LocationsTable>();
  try
  {

    if (locationId == 0) locationId = locationsTable->GetMaxId() + 1;

    locationsTable->Insert(locationId,name,location);
    std::map<size_t, Location>::const_iterator it;
    it = locations.find(locationId);

    Location loc;

    loc.name = name;
    loc.location = location;
    AddLocation( locationId, loc);

  } catch( char *str )
  {
       printf("Error Inserting location : %s\n" , str);
  }
}

void LocationManager::Insert(std::string name, WFMath::Point<3> location)
{
  size_t locationId = 0;
  locationId = GetId(name);
  Insert( locationId, name, location);
}


