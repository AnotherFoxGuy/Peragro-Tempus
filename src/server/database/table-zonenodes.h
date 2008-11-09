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

#ifndef TABLE_ZONENODES_H
#define TABLE_ZONENODES_H

class Database;
class ResultSet;

#include "common/database/table.h"

/**
 * Class holding data associated with a zone.
 */
class ZonenodesTableVO
{
public:
  /// The id of the zone.
  int id;
  /// The x coord of the node.
  float x;
  /// The z coord of the node.
  float z;

  /**
   * Simple empty contstructor.
   */
  ZonenodesTableVO() {}
  /**
   * More advanced construtor.
   * @param id The id of the zone the node belongs to.
   * @param x The x coordinate of the node.
   * @param z The z coordinate of the node.
   */
  ZonenodesTableVO(int id, float x, float z)
  : id(id), x(x), z(z)
  {
  }
};

/**
 * Provides an interface to the database to handle storage of zonenodes.
 */
class ZonenodesTable : public Table
{
private:
  /**
   * Parses a single database result set.
   * The caller is responsible for freeing the returned object.
   * @param rs The database result set.
   * @param row The row in the result table to use, default row is 0.
   * @return Pointer to ZonenodesTableVO which hold all data associated with a zone.
   */
  ZonenodesTableVO* parseSingleResultSet(ResultSet* rs, size_t row = 0);
  /**
   * Will parse all data returned from the database query and insert it
   * into an array.
   * The caller is responsible for freeing the objects in the returned array.
   * @param rs The result set containing the database query result.
   * @return An array containing all ZonenodesTableVO found in the result set.
   */
  Array<ZonenodesTableVO*> parseMultiResultSet(ResultSet* rs);

public:
  /**
   * Construtor for ZonenodesTable class.
   * If database is empty, the createTable function will be called.
   * @param db Pointer to the database.
   */
  ZonenodesTable(Database* db);

  /**
   * Creates a table in the database that will store the zonenodes table.
   */
  void createTable();
  /**
   * Removes all zonenodes table from the database.
   */
  void dropTable();

  /**
   * Insert a zone table into the database.
   * @param vo The zonenodes table to insert.
   */
  void insert(ZonenodesTableVO* vo);
  /**
   * Removes a zone from the database.
   * Matches the zone by its id.
   * @param id The ID of the zone that should be removed.
   */
  void remove(unsigned int id);
  /**
   * Removes all zonenodes from the database.
   */
  void removeAll();
  /**
   * Gives the total number of zonenodes in the database.
   * @return The number of zonenodes.
   */
  unsigned int getCount();
  /**
   * Checks if a zone exists based on its id.
   * @param id The id of the zone to search for.
   * return True if the zone was found, otherwise false.
   */
  bool existsById(int id);
  /**
   * Returns a set of nodes given their zone's id.
   * The caller is responsible for freeing the zonenode rows in the array.
   * @param id The id of the zone to return.
   * @return An array containing the zone VOs for the zone.
   */
  Array<ZonenodesTableVO*> getById(int id);
  /**
   * This function will load all zonenode tables from the database
   * and return an array containing them.
   * The caller is responsible for freeing all zonenode tables in the array.
   * @return An array containing all available zonenodes.
   */
  Array<ZonenodesTableVO*> getAll();
};

#endif // TABLE_ZONENODES_H
