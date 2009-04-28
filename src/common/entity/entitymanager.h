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

#ifndef COMMON_ENTITYMANAGER_H
#define COMMON_ENTITYMANAGER_H

#include <string>
#include <map>
#include <list>

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include "ext/wfmath/octree.h"
#include "ext/wfmath/point.h"

#include "common/util/mutex.h"

namespace Common
{
  namespace Entity
  {
    class Entity;
    struct EntityCallback;
  } // namespace Entity
} // namespace Common

namespace Common
{
  namespace Entity
  {
    typedef boost::shared_ptr<Entity> Entityp;
    typedef boost::weak_ptr<Entity> WeakEntityp;

    typedef std::map<size_t, Entityp> Entities;
    typedef std::pair<ptScopedMutexCopyAble, Entities> EntitiesSafe;

    typedef WFMath::OcTree<WFMath::Point<3>, true>::Type Octree;

    class EntityManager
    {
    protected:
      Mutex cbMutex;
      std::list<EntityCallback*> callback_list;

    protected:
      Mutex mutex;
      Entities entities;
      typedef Entities::iterator Iterator;
      typedef Entities::const_iterator ConstIterator;
      size_t primaryId; // for example, player ID

      Octree octree;

    public:
      EntityManager();
      virtual ~EntityManager();

      virtual bool Add(Entityp entity);
      virtual void Remove(const Entityp entity);
      virtual void Remove(size_t entityId);
      virtual void RemoveAll() { entities.clear(); }

      /// This isn't threadsafe.
      const Entities& GetEntities() { return entities; }

      /// This is threadsafe, but do NOT keep a reference!
      /// It will keep a lock as long as the EntitiesSafe exists.
      EntitiesSafe GetEntitiesSafe() { return EntitiesSafe(ptScopedMutexCopyAble(mutex), entities); }

      size_t GetCount() { ptScopedMutex p(mutex); return entities.size(); }
      Entityp FindById(size_t id);

      Entityp FindByName(const std::string& name);

      /// the following two commands are synonymous
      size_t GetPrimaryId() { return primaryId; }
      size_t GetPlayerId() { return primaryId; }

      virtual void Reset();

      virtual std::list<Entityp> Query(const WFMath::Ball<3>& s);

      virtual void AddEntityCallback(EntityCallback* cb);
      virtual void RemoveEntityCallback(EntityCallback* cb);

      // Helpers
      Entityp Getp(Entity*);
    };

  } // namespace Entity
} // namespace Common

#endif // COMMON_ENTITYMANAGER_H
