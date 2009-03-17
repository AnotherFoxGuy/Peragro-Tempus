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

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include "ext/wfmath/octree.h"
#include "ext/wfmath/point.h"

namespace Common
{
  namespace Entity
  {
    class Entity;
  } // namespace Entity
} // namespace Common

typedef WFMath::OcTree<Common::Entity::Entity, WFMath::Point<3>, true>::Type Octree;

namespace Common
{
  namespace Entity
  {
    typedef boost::shared_ptr<Entity> Entityp;
    typedef boost::weak_ptr<Entity> WeakEntityp;

    class EntityManager
    {
    protected:
      std::map<size_t, Entityp> entities;
      typedef std::map<size_t, Entityp>::iterator Iterator;
      typedef std::map<size_t, Entityp>::const_iterator ConstIterator;
      size_t primaryId; // for example, player ID

      Octree octree;

    public:
      EntityManager();
      virtual ~EntityManager();

      virtual bool Add(Entityp entity);
      virtual void Remove(const Entityp entity);
      virtual void Remove(size_t entityId);
      virtual void RemoveAll() { entities.clear(); }

      size_t GetCount() { entities.size(); }
      Entityp FindById(size_t id);

      Entityp FindByName(const std::string& name);

      /// the following two commands are synonymous
      size_t GetPrimaryId() { return primaryId; }
      size_t GetPlayerId() { return primaryId; }

      virtual void Reset();

      virtual Octree::QueryResult Query(const WFMath::Ball<3>& s);

      // Helpers
      Entityp Getp(Entity*);
      virtual std::list<Entityp> Queryp(const WFMath::Ball<3>& s);
    };

  } // namespace Entity
} // namespace Common

#endif // COMMON_ENTITYMANAGER_H
