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

#include "zonemanager.h"

void ZoneManager::loadFromDB(ZonesTable* zonestable)
{
  Array<ZonesTableVO*>rows=zonestable->getAll();
  Zone zone;
  // The nodes need to be inverted here to be in the right order when checking
  // if a position is within the zone.
  for(int i=rows.getCount()-1; i>-1; i--)
  {
    zone.coords.push_back(rows[i]->coords);
    if(i<1 || rows[i]->id!=rows[i-1]->id)
    {
      zone.type=rows[i]->type;
      zones.push_back(zone);
      zone.coords.clear();
    }
  }
}

ZoneManager::ZoneType ZoneManager::GetZone(float x, float z)
{
  PtVector2 position(x,z);
  for(size_t i=0; i<zones.size(); i++)
  {
    if(Math::IsInArea(&zones[i].coords.front(), zones[i].coords.size(), position))
    {
      return ZoneType(zones[i].type);
    }
  }
  return GROUND;
}
