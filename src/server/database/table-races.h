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

#ifndef TABLE_RACES_H
#define TABLE_RACES_H

#include "table.h"

#include "common/util/ptvector3.h"

class Database;
class Race;
class ptString;

/**
 * Provides an interface to the database to handle storage of races.
 */
class RaceTable : public Table
{
public:
  /**
   * Constructor for the RaceTable.
   * If no skill table is found in the database, the createTable function
   * will be called in order to populate the database.
   * @param db A pointer to the database.
   */
  RaceTable(Database* db);
  /**
   * Creates a table in the database that will store races.
   */
  void createTable();
  /**
   * Inserts a race into the database.
   * @param id The id number of this race.
   * @param name The name of the race.
   * @param mesh The name of the mesh for this race.
   * @param pos An array containing the position.
   * @param sector The sector in which the race is located.
   */
  void insert(int id, const ptString& name, const ptString& mesh,
              const PtVector3& pos, const ptString& sector);
  /**
   * Returns the highest id any race have.
   * @return The highest race id.
   */
  int getMaxId();
  /**
   * Removes all races currently stored in the database.
   */
  void dropTable();
  /**
   * Removes the race matching the given id.
   * @param id The id of the race to be removed from the datbase.
   */
  void remove(int id);
  /**
   * Checks if a race exists in the database.
   * @param name The name of the race to search for in the database.
   * @return True if found, false otherwise.
   */
  bool existsRace(const ptString& name);
  /**
   * Searches the database for a race based on it's id.
   * The caller is responsible for freeing the returned race.
   * @param id The id of the race to search for.
   * @return The race if found, otherwise 0.
   */
  Race* findRaceById(int id);
  /**
   * This function will load all skills from the database.
   * The caller is responsible for freeing all races in the array.
   * @param races An array that will contain all races.
   */
  void getAllRaces(Array<Race*>& races);
};

#endif // TABLE_RACES_H
