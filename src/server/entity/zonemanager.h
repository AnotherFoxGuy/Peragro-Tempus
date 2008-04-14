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

#ifndef ZONEMANAGER_H
#define ZONEMANAGER_H

#include "server/database/table-zones.h"

#include <vector>

class Server;
class PtVector2;

class ZoneManager
{
private:
  struct Zone{
    int type;
    std::vector<PtVector2> coords;
  };
  Server* server;
  std::vector<Zone> zones;
public:
  enum ZoneType
  {
    Ground=0,
    Water=1, // Water and other swimmable fluids (including lava if you find a way to survive the heat?)
    Slow=2 // Anything that would slow you down, like high grass or sticky goo
  };

  void loadFromDB(ZonesTable* zonestable);

  ZoneType GetZone(float x, float z);
};

#endif // ZONEMANAGER_H
