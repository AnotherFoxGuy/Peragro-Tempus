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


namespace Common
{
  namespace Entity
  {
    EntityManager::EntityManager()
    {
    }

    EntityManager::~EntityManager()
    {
    }

    bool EntityManager::Add(const Entityp entity)
    {
      // If object is already present return false.
      if (std::find(entities.begin(), entities.end(), entity) != entities.end())
        return false;

      entity.push_back(object);
      return octree.Add(&entity->position);
    }

    bool EntityManager::Remove(const Entityp entity)
    {
      return false;
    }

    Octree::QueryResult EntityManager::Query(const Geom::Sphere& s)
    {
      return octree.Query(s);
    }

  } // namespace World
} // namespace Common
