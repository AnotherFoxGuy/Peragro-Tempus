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

#ifndef TABLE_DOORS_H
#define TABLE_DOORS_H

class Database;
class ResultSet;

class Mesh;

#include "common/util/ptstring.h"
#include "common/database/table.h"


/**
 * Class holding data associated with a door.
 */
class DoorsTableVO
{
public:
  /// The id of the door.
  unsigned short id;
  /// The name of the door.
  ptString name;
  /// The locked state.
  unsigned char islocked;
  /// The open state.
  unsigned char isopen;
  /// The sector the door exists in
  ptString sector;
  /// The mesh of the door
  unsigned int mesh;
  /// Animation, or CEL Quest
  ptString animation;
  /// X coordinate of the door
  float x;
  /// Y coordinate of the door
  float y;
  /// Z coordinate of the door
  float z;
};

/**
 * Provides an interface to the database to handle storage of doors.
 */
class DoorsTable : public Table
{
private:
  /**
   * Parses a single database result set.
   * The caller is responsible for freeing the returned object.
   * @param rs The database result set.
   * @param row The row in the result table to use, default row is 0.
   * @return Pointer to DoorsTableVO which holds all data associated with door.
   */
  DoorsTableVO* parseSingleResultSet(ResultSet* rs, size_t row = 0);

  /**
   * Will parse all data return from the database query and insert it into
   * an array.
   * The caller is responsible for freeing the objects in the returned array.
   * @param rs The result set containing the database query result.
   * @return An array containing all DoorsTableVO found in the Result set.
   */
  Array<DoorsTableVO*> parseMultiResultSet(ResultSet* rs);
public:
  /**
   * Constructor for DoorsTable class.
   * If database is empty, the createTable function will be called.
   * @param db Pointer to the database.
   */
  DoorsTable(Database* db);
  /**
   * Creates a table in the database that will store doors table.
   */
  void createTable();
  /**
   * Removes all doors table from the database.
   */
  void dropTable();
  /**
   * Insert a door table into the database.
   * @param vo The doors table to insert.
   */
  void insert(DoorsTableVO* vo);
  /**
   * Removes a door from the database.
   * @param id The id of the door to remove.
   */
  void remove(int id);
  /**
   * Returns the number of door tables stored in the database.
   * @return The amount of door tables.
   */
  unsigned int getCount();
  /**
   * Checks if a door table exists in the database.
   * @param name The name of the door table to search for.
   * @return True if the door table is found, otherwise false.
   */
  bool existsByName(ptString name);
  /**
   * Checks if a door table exists in the database.
   * @param id The ID of the door table to search for.
   * @return True if the door table is found, otherwise false.
   */
  bool existsById(int id);
  /**
   * Returns a door table given its name.
   * The caller is responsible for freeing the returned door table.
   * @param name The name of the door table to search for.
   * @return The door table VO class, or 0 if not found.
   */
  DoorsTableVO* getByName(ptString name);
  /**
   * Returns a door table given its name.
   * The caller is responsible for freeing the returned door table.
   * @param id The ID of the door table to search for.
   * @return The door table VO class, or 0 if not found.
   */
  DoorsTableVO* getById(int id);
  /**
   * This function will load all door tables from the database
   * and return an array containing them.
   * The caller is responsible for freeing all door tables in the array.
   * @return An array containing all available door table VO's.
   */
  Array<DoorsTableVO*> getAll();
};

#endif // TABLE_DOORS_H
