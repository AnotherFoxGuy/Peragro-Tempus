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

/**
 * ZoneManager
 * Holds zones and determines what type of zone a coordinate is in
 *   (from a player probably)
 */
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
    GROUND=0,
    WATER=1, // Water and other swimmable fluids (including lava if you find a way to survive the heat?)
    SLOW=2 // Anything that would slow you down, like high grass or sticky goo
  };

  /// Standard loadFromDB function.
  void loadFromDB(ZonesTable* zonestable);

  /**
   * Get the zonetype a coordinate is in.
   * \param x The X position of the coordinate to get the zonetype for.
   * \param y The Y position of the coordinate to get the zonetype for.
   * \return Zonetype for the given coordinate, returns Ground (0) if no zone is found.
   */
  ZoneType GetZone(float x, float z);
};

#endif // ZONEMANAGER_H
