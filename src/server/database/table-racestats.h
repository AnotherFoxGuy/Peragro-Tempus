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

#ifndef TABLE_RACESTATS_H
#define TABLE_RACESTATS_H

#include "common/util/array.h"
#include "common/database/table.h"

class Database;
class Stat;
class RaceStat;

/**
 * Provides an interface to the database to handle storage of race stats.
 */
class RaceStatsTable : public Table
{
public:
  RaceStatsTable(Database* db);
  /**
   * Creates a table in the database that will store race stats.
   */
  void createTable();
  /**
   * Insert a race stat into the database.
   * @param race The type of race.
   * @param stat The id of the stat
   * @param level The level of the race.
   */
  void insert(int race, int stat, int level);
  /**
   * Wrapper around insert.
   * @param race The type of race
   * @param stat The stat for this race
   * @param level The level of this race.
   */
  void set(int race, Stat* stat, int level);
  /**
   * Used to find the level of a race.
   * @param race The type of race.
   * @param stat The stat for this race.
   * @return The level of the race with matching stat, or 0 in case of failure.
   */
  int get(int race, Stat* stat);
  /**
   * Removes all race stats from the database.
   */
  void dropTable();
  /**
   * This function will load all race stats for one particular race from the
   * database.
   * The caller is responsible for freeing all races in the array.
   * @param entries An array that will contain all races stats
   * @param id The race type to return the race stats for.
   */
  void getAllEntries(Array<RaceStat*>& entries, int id);
};

#endif //TABLE_RACESTATS_H
