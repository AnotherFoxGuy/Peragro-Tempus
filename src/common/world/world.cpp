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

#include "world.h"

#include "common/database/sqlite/sqlite.h"

#include "table-objects.h"
#include "table-factories.h"

namespace Common
{
  namespace World
  {
    /// For std::set
    bool operator<(const Common::World::Object& obj1, const Common::World::Object& obj2)
    {
      return obj1.id < obj2.id;
    }

    WorldManager::WorldManager() : db("world.sqlite")
    {
      ObjectsTable objectsTable(&db);
      Array<Object> objs;
      objectsTable.GetObjects(objs);
      for (size_t i = 0; i < objs.getCount(); i++)
      {
        printf("WorldManager: %s\n", objs[i].name.c_str());
        objects.Add(objs[i].worldBB, objs[i]);
      }
    }

    WorldManager::~WorldManager()
    {
    }

    bool WorldManager::Add(const Object& object)
    {
      ObjectsTable objectsTable(&db);
      objectsTable.Insert(object);
      return objects.Add(object.worldBB, object);
    }

    bool WorldManager::AddLookUp(Object& object)
    {
      FactoriesTable factoryTable(&db);
      object.worldBB = factoryTable.GetBB(object.factoryFile, object.factoryName);
      // TODDO: do proper transform.
      //object.worldBB = object.worldBB + object.position;

      return Add(object);
    }

    bool WorldManager::Remove(const Object& object)
    {
      return false;
    }

    bool WorldManager::Add(const Factory& factory)
    {
      FactoriesTable factoryTable(&db);
      factoryTable.Insert(factory);
      return true;
    }

    bool WorldManager::Remove(const Factory& factory)
    {
      return false;
    }

    Octree::QueryResult WorldManager::Query(const Geom::Sphere& s)
    {
      return objects.Query(s);
    }

  } // namespace World
} // namespace Common
