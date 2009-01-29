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

#ifndef TABLE_ZONES_H
#define TABLE_ZONES_H

class Database;
class ResultSet;

#include "common/util/ptstring.h"
#include <wfmath/point.h>
#include "common/database/table.h"

/**
 * Class holding data associated with a zone.
 */
class ZonesTableVO
{
public:
  /// The id of the zone.
  int id;
  /// Which type of zone it is.
  std::string type;

  /**
   * Simple empty contstructor.
   */
  ZonesTableVO() {}
  /**
   * More advanced construtor.
   * @param id The id of the zone.
   * @param type Which type of zone it is.
   */
  ZonesTableVO(int id, std::string type)
  : id(id), type(type)
  {
  }
};

/**
 * Provides an interface to the database to handle storage of zones.
 */
class ZonesTable : public Table
{
private:
  /**
   * Parses a single database result set.
   * The caller is responsible for freeing the returned object.
   * @param rs The database result set.
   * @param row The row in the result table to use, default row is 0.
   * @return Pointer to ZonesTableVO which hold all data associated with a zone.
   */
  ZonesTableVO* parseSingleResultSet(ResultSet* rs, size_t row = 0);
  /**
   * Will parse all data returned from the database query and insert it
   * into an array.
   * The caller is responsible for freeing the objects in the returned array.
   * @param rs The result set containing the database query result.
   * @return An array containing all ZonesTableVO found in the result set.
   */
  Array<ZonesTableVO*> parseMultiResultSet(ResultSet* rs);

public:
  /**
   * Construtor for ZonesTable class.
   * If database is empty, the createTable function will be called.
   * @param db Pointer to the database.
   */
  ZonesTable(Database* db);

  /**
   * Creates a table in the database that will store the zones table.
   */
  void createTable();
  /**
   * Removes all zones table from the database.
   */
  void dropTable();

  /**
   * Insert a zone table into the database.
   * @param vo The zones table to insert.
   */
  void insert(ZonesTableVO* vo);
  /**
   * Removes a zone from the database.
   * Matches the zone by its id.
   * @param id The ID of the zone that should be removed.
   */
  void remove(unsigned int id);
  /**
   * Removes all zones from the database.
   */
  void removeAll();
  /**
   * Gives the total number of zones in the database.
   * @return The number of zones.
   */
  unsigned int getCount();
  /**
   * Checks if a zone exists based on its id.
   * @param id The id of the zone to search for.
   * return True if the zone was found, otherwise false.
   */
  bool existsById(int id);
  /**
   * Returns a zone table given its name.
   * The caller is responsible for freeing the returned zone table.
   * @param id The id of the zone to return.
   * @return The zone VO class.
   */
  ZonesTableVO* getById(int id);
  /**
   * This function will load all zone tables from the database
   * and return an array containing them.
   * The caller is responsible for freeing all zone tables in the array.
   * @return An array containing all available zones.
   */
  Array<ZonesTableVO*> getAll();
};

#endif // TABLE_ZONES_H
