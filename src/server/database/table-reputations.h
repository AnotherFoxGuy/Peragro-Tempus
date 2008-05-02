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

#ifndef TABLE_REPUTATIONS_H
#define TABLE_REPUTATIONS_H

#include "table.h"

class Database;
class Reputation;
class ptString;

/**
 * Provides an interface to the database to handle storage of reputations.
 */
class ReputationsTable : public Table
{
public:
  ReputationsTable(Database* db);
  /**
   * Creates a table in the database that will store eputations.
   */
  void createTable();
  /**
   * Insert a reputation into the database.
   * @param name The name of the reputation
   */
  int insert(ptString name);
  /**
   * Removes all reputations from the database.
   */
  void dropTable();
  /**
   * Checks if a reputation exists in the database
   * @return True if the reputation existed, false otherwise.
   */
  bool existsReputation(ptString name);
  /**
   * Does a lookup in the database to find a reputation.
   * The caller is responsible for freeing the reputation returned.
   * @returns Reputation if found, otherwise 0.
   */
  Reputation* getReputation(ptString name);
  /**
   * This function will load all reputations from the database.
   * The caller is responsible for freeing all reputations in the array.
   * @param reputations An array that will contain all reputations.
   */
  void getAllReputations(Array<Reputation*>& reputations);
};

#endif //TABLE_REPUTATIONS_H
