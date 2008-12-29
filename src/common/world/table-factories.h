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

#ifndef TABLE_FACTORY_H
#define TABLE_FACTORY_H

#include "common/database/table.h"

class Database;
class Reputation;
class ptString;

namespace World
{
  struct Factory;
} // namespace World

/**
 * Provides an interface to the database to handle storage of reputations.
 */
class FactoriesTable : public Table
{
private:
  /**
   * Creates a table in the database that will store eputations.
   */
  void CreateTable();

public:
  FactoriesTable(Database* db);

  /**
   * Insert a object into the database.
   * @param object
   */
  void Insert(const Factory& factory);

  Factory GetBB(const std::string& factoryFile, const std::string& factoryName);

  /**
   * Removes all objects from the database.
   */
  void DropTable();

  /**
   * This function will load all objects from the database.
   * @param reputations An array that will contain all objects.
   */
  void GetAll(Array<Factory>& factories);
};

#endif //TABLE_FACTORY_H
