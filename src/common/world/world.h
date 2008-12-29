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

#include "common/geom/octree.h"
#include "common/geom/box.h"
#include "common/geom/vector2.h"

class dbSQLite;

namespace World
{
  struct Factory
  {
    std::string factoryFile;
    std::string factoryName;
    Geom::Box boundingBox;
  };

  struct Object
  {
    size_t id;
    std::string name;
    std::string factoryFile;
    std::string factoryName;
    Geom::Vector3 position;
    Geom::Box worldBB;
  };

  class WorldManager
  {
  public:
    typedef Geom::OcTree<Geom::Box, Object> Octree;

  private:
    dbSQLite db;
    Octree objects;

  public:
    WorldManager();
    ~WorldManager();

    bool Add(const Object& object);
    bool AddLookUp(const Object& object);
    bool Remove(const Object& object);

    bool Add(const Factory& factory);
    bool Remove(const Factory& factory);

    Octree::QueryResult Query(const Geom::Sphere& s);
  };

} // namespace World

typedef Geom::Vector2 PtVector2;

#endif // COMMON_WORLD_H
