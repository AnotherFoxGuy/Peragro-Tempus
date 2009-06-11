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

#ifndef LOCATIONMANAGER_H
#define LOCATIONMANAGER_H

#include <wfmath/point.h>
#include "common/util/ptstring.h"

#include <vector>
#include <map>

class Server;
class LocationsTable;

/**
 * LocationsManager
 * Holds locations 
 */
class LocationManager
{
public:
  struct Location
  {
    std::string name;
    WFMath::Point<3> location;
  };
private:
  Server* server;
  std::map<size_t, Location> locations;
public:
  /// Clear all locations.
  void delAll();

  /// Add a location.
  void AddLocation(size_t locationId, Location location)
  {
    locations[locationId] = location;
  }

  /**
   * Standard loadFromDB function.
   * \param locationstable The database table containing the locations to load.
   */
  void LoadFromDB();
  void Insert(size_t locationId, std::string name, WFMath::Point<3> location);
  void Insert(std::string name, WFMath::Point<3> location);

  std::string GetName(size_t locationId);
  size_t GetId(const std::string& name);
  WFMath::Point<3> GetLocation(size_t locationId);
  WFMath::Point<3> GetLocation(const std::string& name);
};

#endif // LOCATIONMANAGER_H
