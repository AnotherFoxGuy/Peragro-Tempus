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
#include "entitycallback.h"

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
      entities.clear();
    }

    bool EntityManager::Add(Entityp entity)
    {
      {
        LockType lock(mutex);
        // If object is already present return false.
        if (entities.count(entity->GetId()) > 0)
          return false;
      }

      bool succes = octree.Add(entity);
      if (succes)
      {
        {
          LockType lock(mutex);
          entities[entity->GetId()] = entity;
        }

        {
          LockType lock(cbMutex);
          std::list<Common::Entity::EntityCallback*>::iterator it;
          for ( it=callback_list.begin() ; it != callback_list.end(); it++ )
          {
            (*it)->OnEntityAdd(entity);
          }
        }
      }

      return succes;
    }

    void EntityManager::Remove(const Entityp entity)
    {
      {
        LockType lock(cbMutex);
        std::list<Common::Entity::EntityCallback*>::iterator it;
        for ( it=callback_list.begin() ; it != callback_list.end(); it++ )
        {
          (*it)->OnEntityRemove(entity);
        }
      }

      LockType lock(mutex);
      // TODO: throw when the entity hasn't been found?
      entities.erase(entity->GetId());
      // Entity will automatically be removed from the octree.
    }

    void EntityManager::Remove(size_t entityId)
    {
      Remove(FindById(entityId));
    }

    Entityp EntityManager::FindById(size_t id)
    {
      LockType lock(mutex);
      Iterator it;
      it = entities.find(id);
      if (it != entities.end())
      {
//         printf("found common Ent(%i)%s\n", it->second->GetId(), it->second->GetName().c_str());
         return it->second;
      }
      else
      {
        return Entityp();
      }
    }

    Entityp EntityManager::FindByName(const std::string& name)
    {
      LockType lock(mutex);
      //TODO
      return Entityp();
    }

    void EntityManager::Reset()
    {
      primaryId = 0;
      RemoveAll();
    }

    std::list<Entityp> EntityManager::Query(const WFMath::Ball<3>& s)
    {
      LockType lock(mutex);
      std::list<Entityp> r = octree.Query<Entity>(s);
      return r;
    }

    void EntityManager::AddEntityCallback(Common::Entity::EntityCallback* cb)
    {
      LockType lock(cbMutex);
      callback_list.remove(cb);
      callback_list.push_back(cb);
    }

    void EntityManager::RemoveEntityCallback(Common::Entity::EntityCallback* cb)
    {
      LockType lock(cbMutex);
      callback_list.remove(cb);
    }

    Entityp EntityManager::Getp(Entity* e)
    {
      return FindById(e->GetId());
    }
  } // namespace World
} // namespace Common
