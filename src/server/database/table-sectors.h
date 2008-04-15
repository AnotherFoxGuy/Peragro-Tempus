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

#ifndef TABLE_SECTORS_H
#define TABLE_SECTORS_H

class Database;
class ResultSet;

#include "common/util/ptstring.h"
#include "table.h"


/**
 * Class holding data associated with a sector.
 */
class SectorsTableVO
{
public:
  /// The id of the sector.
  unsigned short id;
  /// The name of the sector.
  ptString name;
  /// The region corresponding to the sector.
  ptString region;
};

/**
 * Provides an interface to the database to handle storage of sectors.
 */
class SectorsTable : public Table
{
private:
  /**
   * Parses a single database result set.
   * The caller is responsible for freeing the returned object.
   * @param rs The database result set.
   * @param row The row in the result table to use, default row is 0.
   * @return Pointer to SectorsTableVO which holds all data associated with sector.
   */ 
  SectorsTableVO* parseSingleResultSet(ResultSet* rs, size_t row = 0);
 
  /**
   * Will parse all data return from the database query and insert it into
   * an array.
   * The caller is responsible for freeing the objects in the returned array.
   * @param rs The result set containing the database query result.
   * @return An array containing all SectorsTableVO found in the Result set.
   */
  Array<SectorsTableVO*> parseMultiResultSet(ResultSet* rs);
public:
  /**
   * Constructor for SectorsTable class.
   * If database is empty, the createTable function will be called.
   * @param db Pointer to the database.
   */
  SectorsTable(Database* db);
  /**
   * Creates a table in the database that will store sector rows.
   */
  void createTable();
  /**
   * Removes the sectors table from the database.
   */
  void dropTable();
  /**
   * Insert a sector row into the database.
   * @param vo The sectors row to insert.
   */
  void insert(SectorsTableVO* vo);
  /**
   * Removes a sector from the database.
   * @param id The id of the sector to remove.
   */
  void remove(unsigned short id);
  /**
   * Returns the number of sector rows stored in the database.
   * @return The amount of sector rows.
   */
  unsigned int getCount();
  /**
   * Checks if a sector row exists in the database.
   * @param name The name of the sector row to search for.
   * @return True if the sector is found, otherwise false.
   */
  bool existsByName(ptString name);
  /**
   * Checks if a sector table exists in the database.
   * @param id The ID of the sector to search for.
   * @return True if the sector is found, otherwise false.
   */
  bool existsById(int id);
  /**
   * Returns a sector row given its name.
   * The caller is responsible for freeing the returned sector row.
   * @param name The name of the sector row to search for.
   * @return The sector row VO class, or 0 if not found.
   */
  SectorsTableVO* getByName(ptString name);
  /**
   * Returns a sector row given its ID.
   * The caller is responsible for freeing the returned sector row.
   * @param id The ID of the sector row to search for.
   * @return The sector row VO class, or 0 if not found.
   */
  SectorsTableVO* getById(int id);
  /**
   * This function will load all sector rows from the database 
   * and return an array containing them.
   * The caller is responsible for freeing all sector rows in the array.
   * @return An array containing all available sector row VO's.
   */
  Array<SectorsTableVO*> getAll();
};

#endif // TABLE_SECTORS_H
