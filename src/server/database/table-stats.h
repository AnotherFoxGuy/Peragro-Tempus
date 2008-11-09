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

#ifndef TABLE_STATS_H
#define TABLE_STATS_H

#include "common/database/table.h"

class Database;
class Stat;
class ptString;

/**
 * Provides an interface to the database to handle storage of stats.
 */
class StatTable : public Table
{
public:
  StatTable(Database* db);
  /**
   * Creates a table in the database that will store stats.
   */
  void createTable();
  /**
   * Insert a stat into the database.
   * @param name The name of the stat
   */
  void insert(ptString name);
  /**
   * Removes all stats from the database.
   */
  void dropTable();
  /**
   * Checks if a stat exists in the database
   * @return True if the stat existed, false otherwise.
   */
  bool existsStat(ptString name);
  /**
   * Does a lookup in the database to find a stat.
   * The caller is responsible for freeing the stat returned.
   * @returns Stat if found, otherwise 0.
   */
  Stat* getStat(ptString name);
  /**
   * This function will load all stats from the database.
   * The caller is responsible for freeing all stats in the array.
   * @param stats An array that will contain all stats.
   */
  void getAllStats(Array<Stat*>& stats);
};

#endif //TABLE_STATS_H
