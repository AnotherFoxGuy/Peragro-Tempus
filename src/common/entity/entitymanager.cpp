/*
    Copyright (C) 2009 Development Team of Peragro Tempus

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

#include "entitymanager.h"

#include "entity.h"

namespace Common
{
  namespace Entity
  {
    EntityManager::EntityManager()
      : entities(), world_loaded(false), primaryId(0)
    {
    }

    EntityManager::~EntityManager()
    {
    }

/*
    bool EntityManager::Add(const Entityp entity)
    {
      // If object is already present return false.
      if (std::find(entities.begin(), entities.end(), entity) != entities.end())
        return false;

      entities.push_back(entity);
      return octree.Add(&entity->position);
    }
*/

    void EntityManager::Remove(const Entityp entity)
    {
      std::vector<Entityp>::iterator i;
      for (i = entities.begin();  i < entities.end();  i++)
      {
        if (*i == entity) 
        {
          entities.erase(i);
          break;
        }
      }
    }

    Entityp EntityManager::findEntById(unsigned int id)
    {
      //TODO if (id == 0) return 0;
      std::vector<Entityp>::iterator i;
      for (i = entities.begin();  i < entities.end();  i++)
      {
        if ((*i) && (*i)->GetId() == id) return *i;
      }
      return Entityp();
    }

    void EntityManager::Reset()
    {
      world_loaded = false;
      primaryId = 0;
      RemoveAll();
    }

/*
    Octree::QueryResult EntityManager::Query(const WFMath::Ball<3>& s)
    {
      return octree.Query(s);
    }
*/

  } // namespace World
} // namespace Common
