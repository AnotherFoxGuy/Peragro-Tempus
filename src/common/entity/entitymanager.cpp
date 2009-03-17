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
      : entities(), primaryId(0)
    {
    }

    EntityManager::~EntityManager()
    {
    }

    bool EntityManager::Add(Entityp entity)
    {
      // If object is already present return false.
      if (entities.count(entity->GetId()) > 0) 
        return false;

      bool succes = octree.Add(&entity->position);
      if (succes) entities[entity->GetId()] = entity;
      return succes;
    }

    void EntityManager::Remove(const Entityp entity)
    {
      // TODO: throw when the entity hasn't been found?
      entities.erase(entity->GetId()); 
      // Entity will automatically be removed from the octree.
    }

    void EntityManager::Remove(size_t entityId)
    {
      // TODO: throw when the entity hasn't been found?
      entities.erase(entityId);
      // Entity will automatically be removed from the octree.
    }

    Entityp EntityManager::FindById(size_t id)
    {
      Iterator it;
      it = entities.find(id);
      if (it != entities.end()) 
         return it->second;
      else
        return Entityp();
    }

    Entityp EntityManager::FindByName(const std::string& name)
    {
      //TODO
      return Entityp();
    }

    void EntityManager::Reset()
    {
      primaryId = 0;
      RemoveAll();
    }

    Octree::QueryResult EntityManager::Query(const WFMath::Ball<3>& s)
    {
      return octree.Query(s);
    }

    Entityp EntityManager::Getp(Entity* e)
    {
      return FindById(e->GetId());
    }

    std::list<Entityp> EntityManager::Queryp(const WFMath::Ball<3>& s)
    {
      std::list<Entityp> result;
      Octree::QueryResult query = Query(s);
      Octree::QueryResult::const_iterator it;
      for ( it=query.begin() ; it != query.end(); it++ )
      {
        result.push_back(Getp(*it));
      }
      return result;
    }

  } // namespace World
} // namespace Common
