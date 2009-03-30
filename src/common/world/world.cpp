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
#include "common/util/printhelper.h"

namespace Common
{
  namespace World
  {
    WorldManager::WorldManager() : db("world.sqlite"), objectsTable(&db), factoryTable(&db)
    {
      ObjectsTableVOArray objs = objectsTable.GetAll();
      ObjectsTableVOArray::iterator it;
      for (it = objs.begin(); it != objs.end(); it++)
      {
        printf("WorldManager: %"SIZET" %s\n", (*it)->id, (*it)->name.c_str());
        boost::shared_ptr<Object> object(new Object( *(*it).get() ));
        objects.push_back(object);
        octree.Add(object);
      }
    }

    WorldManager::~WorldManager()
    {
    }

    bool WorldManager::HasData()
    {
      FactoriesTableVOArray facts = factoryTable.GetAll();
      ObjectsTableVOArray objs = objectsTable.GetAll();
      return facts.size() && objs.size();
    }

    class SameId
    {
      Object* t;
    public:
      SameId(boost::shared_ptr<Object> _t) : t(_t.get()) {}
      bool operator() (boost::shared_ptr<Object> o) { return t->id == o->id; }
    };

     void WorldManager::Insert(const Objectp object, bool unique)
     {
       if (unique)
          object->id = objectsTable.GetMaxId()+1;

       objectsTable.Insert(object->id,
                           object->name,
                           object->factoryFile,
                           object->factoryName,
                           object->position,
                           object->rotation,
                           object->sector,
                           object->GetShape(),
                           object->detailLevel);
     }

    bool WorldManager::Add(const Objectp object, bool unique)
    {
      // If object is already present, just update the DB instead.
      if (std::find(objects.begin(), objects.end(), object) != objects.end())
        return Update(object);

      // Remove any objects with the same ID.
      objects.remove_if(SameId(object));

      Insert(object, unique);
      objects.push_back(object);
      return octree.Add(object);
    }

    bool WorldManager::AddLookUp(Objectp object, bool unique)
    {
      WFMath::AxisBox<3> box;
      try 
      {box = factoryTable.GetSingle(object->factoryFile, object->factoryName)->boundingBox;}
      catch (char*){}
      // TODO: do proper transform.
      box += object->position;
      object->SetShape(box);

      object->detailLevel = 0;
      try 
      {factoryTable.GetSingle(object->factoryFile, object->factoryName)->detailLevel;}
      catch (char*){}

      return Add(object, unique);
    }

    bool WorldManager::Update(const Objectp object)
    {
      Insert(object, false);
      return true;
    }

    bool WorldManager::Remove(const Objectp object)
    {
      return false;
    }

    bool WorldManager::Add(const Factory& factory)
    {
      factoryTable.Insert(factory.factoryFile, 
                          factory.factoryName, 
                          factory.boundingBox,
                          factory.detailLevel,
                          factory.hash);
      return true;
    }

    bool WorldManager::Remove(const Factory& factory)
    {
      return false;
    }

    std::list<Objectp> WorldManager::Query(const WFMath::Ball<3>& s)
    {
      return octree.Query<Object>(s);
    }

  } // namespace World
} // namespace Common
