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

#include <boost/shared_ptr.hpp>

#include <wfmath/octree.h>
#include <wfmath/axisbox.h>
#include <wfmath/point.h>
#include <wfmath/ball.h>

//class dbSQLite;
#include "common/database/sqlite/sqlite.h"

#include "table-objects.h"
#include "table-factories.h"

namespace Common
{
  namespace World
  {
    struct Object;
  }
}

typedef WFMath::OcTree<WFMath::AxisBox<3>, true>::Type Octree;

namespace Common
{
  namespace World
  {
    typedef FactoriesTableVO Factory;

    struct Object : WFMath::Shape<WFMath::AxisBox<3> >, ObjectsTableVO
    {
    private:
      WFMath::AxisBox<3> boundingBox;
    public:
      Object() {}
      Object(const ObjectsTableVO& vo)
      {
        id = vo.id; 
        name = vo.name;
        factoryFile = vo.factoryFile;
        factoryName = vo.factoryName;
        position = vo.position;
        rotation = vo.rotation;
        sector = vo.sector;
        //boundingBox = vo.boundingBox;
        this->SetShape(vo.boundingBox);
        detailLevel = vo.detailLevel;
      }

      virtual void SetShape(const WFMath::AxisBox<3>& v)
      {
        WFMath::Shape<WFMath::AxisBox<3> >::SetShape(v);
        boundingBox = v;
      }
    };

    typedef boost::shared_ptr<Object> Objectp;

    class WorldManager
    {
    private:
      dbSQLite db;
      ObjectsTable objectsTable;
      FactoriesTable factoryTable;

      Octree octree;
      std::list<Objectp> objects;

      void Insert(const Objectp object, bool unique);

    public:
      WorldManager();
      ~WorldManager();

      bool HasData();

      bool Add(const Objectp object, bool unique = true);
      bool AddLookUp(Objectp object, bool unique = true);
      bool Update(const Objectp object);
      bool Remove(const Objectp object);

      bool Add(const Factory& factory);
      bool Remove(const Factory& factory);

      std::string GetMD5(const std::string& factoryFile, const std::string& factoryName)
      {
        try
        {return factoryTable.GetSingle(factoryFile, factoryName)->hash;}
        catch (char*)
        {return "Invalid MD5";}
      }

      std::list<Objectp> Query(const WFMath::Ball<3>& s);
    };

  } // namespace World
} // namespace Common

#endif // COMMON_WORLD_H
