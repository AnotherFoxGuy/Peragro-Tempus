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

#include "common/geom/vector2.h"
#include "common/util/ptstring.h"

#include <vector>

class Server;
class ZonesTable;
class ZonenodesTable;

/**
 * ZoneManager
 * Holds zones and determines what type of zone a coordinate is in
 *   (coordinate from a player probably)
 */
class ZoneManager
{
public:
  struct Zone
  {
    ptString type;
    std::vector<PtVector2> coords;
  };
private:
  Server* server;
  std::vector<Zone> zones;
public:
  /// Clear all zones.
  void delAll();

  /// Add a zone.
  void addZone(Zone zone)
  {
    zones.push_back(zone);
  }

  /**
   * Standard loadFromDB function.
   * \param zonestable The database table containing the zones to load.
   * \param zonenodestable The database table containing the nodes of the zones to load.
   */
  void loadFromDB(ZonesTable* zonestable, ZonenodesTable* zonenodestable);

  /**
   * Get the zonetype a coordinate is in.
   * \param x The X position of the coordinate to get the zonetype for.
   * \param z The Z position of the coordinate to get the zonetype for.
   * \return ptString with the zonetype for the given coordinate, returns "" if no zone is found.
   */
  ptString GetZone(float x, float z);

  /**
   * Get the zonetypes a coordinate is in (for multiple zones).
   * \param x The X position of the coordinate to get the zonetypes for.
   * \param z The Z position of the coordinate to get the zonetypes for.
   * \return std::vector&lt;ptString&gt; with the zonetypes for the
             given coordinate, returns an empty vector if no zone is found.
   */
  std::vector<ptString> GetZones(float x, float z);
};

#endif // ZONEMANAGER_H
