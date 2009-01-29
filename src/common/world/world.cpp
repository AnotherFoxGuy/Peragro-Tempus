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


namespace Common
{
  namespace World
  {
    WorldManager::WorldManager() : db("world.sqlite"), objectsTable(&db), factoryTable(&db)
    {
      Array<Object> objs;
      objectsTable.GetAll(objs);
      for (size_t i = 0; i < objs.getCount(); i++)
      {
        printf("WorldManager: %s\n", objs[i].name.c_str());
        boost::shared_ptr<Object> object(new Object(objs[i]));
        objects.push_back(object);
        octree.Add(&object->worldBB);
      }
    }

    WorldManager::~WorldManager()
    {
    }

    bool WorldManager::HasData()
    {
      Array<Factory> facts;
      factoryTable.GetAll(facts);
      Array<Object> objs;
      objectsTable.GetAll(objs);

      return facts.getCount() && objs.getCount();
    }

    class SameId
    {
      Object* t;
    public:
      SameId(boost::shared_ptr<Object> _t) : t(_t.get()) {}
      bool operator() (boost::shared_ptr<Object> o) { return t->id == o->id; }
    };

    bool WorldManager::Add(const Objectp object, bool unique)
    {
      // If object is already present, just update the DB instead.
      if (std::find(objects.begin(), objects.end(), object) != objects.end())
        return Update(object.get());

      // Remove any objects with the same ID.
      objects.remove_if(SameId(object));

      objectsTable.Insert(*object.get(), unique);
      objects.push_back(object);
      return octree.Add(&object->worldBB);
    }

    bool WorldManager::AddLookUp(Objectp object, bool unique)
    {
      object->worldBB = factoryTable.GetBB(object->factoryFile, object->factoryName);
      // TODO: do proper transform.
      object->worldBB += object->position;

      object->detailLevel = factoryTable.GetDetailLevel(object->factoryFile, object->factoryName);

      return Add(object, unique);
    }

    bool WorldManager::Update(const Object* object)
    {
      objectsTable.Insert(*object, false);
      return true;
    }

    bool WorldManager::Remove(const Objectp object)
    {
      return false;
    }

    bool WorldManager::Add(const Factory& factory)
    {
      factoryTable.Insert(factory);
      return true;
    }

    bool WorldManager::Remove(const Factory& factory)
    {
      return false;
    }

    Octree::QueryResult WorldManager::Query(const WFMath::Ball<3>& s)
    {
      return octree.Query(s);
    }

  } // namespace World
} // namespace Common
