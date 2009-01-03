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

namespace Common
{
  namespace World
  {
    struct Factory;
  } // namespace World
} // namespace Common

namespace Geom
{
  class Box;
  class Vector3;
} // namespace Geom

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

  std::string GetFactoryFile(ResultSet* rs, size_t row);
  std::string GetFactoryName(ResultSet* rs, size_t row);
  Geom::Vector3 GetPosition(ResultSet* rs, size_t row, size_t offset = 2);
  Geom::Box GetBoundingBox(ResultSet* rs, size_t row);
  size_t GetDetailLevel(ResultSet* rs, size_t row);
  std::string GetMD5(ResultSet* rs, size_t row);

public:
  FactoriesTable(Database* db);

  /**
   * Insert a object into the database.
   * @param object
   */
  void Insert(const Common::World::Factory& factory);

  Geom::Box GetBB(const std::string& factoryFile, const std::string& factoryName);

  std::string GetMD5(const std::string& factoryFile, const std::string& factoryName);

  /**
   * Removes all objects from the database.
   */
  void DropTable();

  /**
   * This function will load all objects from the database.
   * @param reputations An array that will contain all objects.
   */
  void GetAll(Array<Common::World::Factory>& factories);
};

#endif //TABLE_FACTORY_H
