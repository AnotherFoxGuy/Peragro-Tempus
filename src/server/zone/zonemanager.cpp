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
#include "server/database/table-zones.h"
#include "server/database/table-zonenodes.h"
#include "server/server.h"

#include "zonemanager.h"

WFMath::Point<3> ZoneManager::Zone::GetRandomPosition() const
{
  //TODO: if the zone is some odd polygon this doesn't guarantee
  // the position is actually in the zone.
  // Do several random tries with IsInArea()??
  float minX, minZ; minX = minZ = 10000000000000.0f;
  float maxX, maxZ; maxX = maxZ = -10000000000000.0f;
  std::vector<WFMath::Point<2> >::const_iterator it;
  for (it = coords.begin(); it != coords.end(); it++)
  {
    WFMath::Point<2> p = *it;
    if (p.x() < minX) minX = p.x();
    if (p.x() > maxX) maxX = p.x();

    if (p.y() < minZ) minZ = p.y();
    if (p.y() > maxZ) maxZ = p.y();
  }

  WFMath::Point<3> pos;
  pos[0] = (rand() % (int)(maxX - minX + 1)) + minX;
  pos[2] = (rand() % (int)(maxZ - minZ + 1)) + minZ;

  return pos;
}

void ZoneManager::delAll()
{
  Server::getServer()->GetTableManager()->Get<ZonesTable>()->DeleteAll();
  Server::getServer()->GetTableManager()->Get<ZoneNodesTable>()->DeleteAll();
  zones.clear();
}

void ZoneManager::LoadFromDB()
{
  ZonesTable* zonestable = Server::getServer()->GetTableManager()->Get<ZonesTable>();
  ZoneNodesTable* zonenodestable = Server::getServer()->GetTableManager()->Get<ZoneNodesTable>();
  ZonesTableVOArray rows = zonestable->GetAll();
  for (size_t i = 0; i < rows.size(); ++i)
  {
    Zone zone;
    zone.type = rows[i]->type;
    ZoneNodesTableVOArray nodes = zonenodestable->Get(rows[i]->id);
    for(size_t j = 0; j < nodes.size(); ++j)
    {
      zone.coords.push_back(nodes[j]->coordinate);
    }
    AddZone(rows[i]->id, zone);
  }
}

ptString ZoneManager::GetZone(float x, float z)
{
  WFMath::Point<2> position(x, z);
  std::map<size_t, Zone>::iterator it;
  for (it = zones.begin(); it != zones.end(); it++)
  {
    if (PT::Math::IsInArea(&it->second.coords.front(), it->second.coords.size(),
      position))
    {
      return it->second.type;
    }
  }
  return ptString("", 0);
}

std::vector<ptString> ZoneManager::GetZones(float x, float z)
{
  std::vector<ptString> inzones;
  WFMath::Point<2> position(x, z);
  std::map<size_t, Zone>::iterator it;
  for (it = zones.begin(); it != zones.end(); it++)
  {
    if (PT::Math::IsInArea(&it->second.coords.front(), it->second.coords.size(),
      position))
    {
      inzones.push_back(it->second.type);
    }
  }
  return inzones;
}

WFMath::Point<3> ZoneManager::GetRandomPosition(size_t zoneId)
{
  std::map<size_t, Zone>::const_iterator it;
  it = zones.find(zoneId);
  if (it != zones.end())
    return it->second.GetRandomPosition();
  else
    return WFMath::Point<3>(0);
}
