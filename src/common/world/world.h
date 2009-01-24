/*
    Copyright (C) 2008 Development Team of Peragro Tempus

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

#ifndef COMMON_WORLD_H
#define COMMON_WORLD_H

#include <string>

#include <boost/shared_ptr.hpp>

#include "common/geom/octree.h"
#include "common/geom/box.h"
#include "common/geom/vector3.h"

//class dbSQLite;
#include "common/database/sqlite/sqlite.h"

#include "table-objects.h"
#include "table-factories.h"

namespace Common
{
  namespace World
  {
    struct Object;
  }
}

typedef Geom::OcTree<Common::World::Object, Geom::Box> Octree;

namespace Common
{
  namespace World
  {
    struct Factory
    {
      std::string factoryFile;
      std::string factoryName;
      Geom::Box boundingBox;
      size_t detailLevel;
      std::string hash;
    };

    struct Object
    {
      Object() : worldBB(this) {}

      Object(const Object& o) : worldBB(this)
      {
        id = o.id;
        name = o.name;
        factoryFile = o.factoryFile;
        factoryName = o.factoryName;
        position = o.position;
        sector = o.sector;
        worldBB = o.worldBB.Get();
        worldBB.Set(o.worldBB.Get());
        detailLevel = o.detailLevel;
      }

      size_t id;
      std::string name;
      std::string factoryFile;
      std::string factoryName;
      Geom::Vector3 position;
      std::string sector;
      Octree::Shape worldBB;
      size_t detailLevel;
    };

    class WorldManager
    {
    private:
      dbSQLite db;
      ObjectsTable objectsTable;
      FactoriesTable factoryTable;

      typedef boost::shared_ptr<Object> Objectp;

      Octree octree;
      std::list<Objectp> objects;

    public:
      WorldManager();
      ~WorldManager();

      bool Add(const Objectp object, bool unique = true);
      bool AddLookUp(Objectp object, bool unique = true);
      bool Update(const Object* object);
      bool Remove(const Objectp object);

      bool Add(const Factory& factory);
      bool Remove(const Factory& factory);

      std::string GetMD5(const std::string& factoryFile, const std::string& factoryName)
      {
        return factoryTable.GetMD5(factoryFile, factoryName);
      }

      Octree::QueryResult Query(const Geom::Sphere& s);
    };

  } // namespace World
} // namespace Common

#endif // COMMON_WORLD_H
