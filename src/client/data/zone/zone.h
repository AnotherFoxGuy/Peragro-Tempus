/*
    Copyright (C) 2007 Development Team of Peragro Tempus

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

#ifndef PT_DATAZONE_H
#define PT_DATAZONE_H

#include <vector>
#include <wfmath/point.h>
#include "common/util/ptstring.h"

namespace PT
{
  namespace Data
  {
    /**
     * @ingroup data_handlers
     * Helper class that contains basic information about zone. Usually not
     * created directly, but accessed via ZoneDataManager instance. Use the
     * Set/Get methods to access the zone properties.
     * @see ZoneDataManager
     */
    class Zone
    {
    private:
      ///ID of the zone
      unsigned int id;

      ///Zone type
      ptString type;

      ///Nodes forming the zone.
      std::vector<WFMath::Point<2> > nodes;

    public:
      ///Set the ID of the zone.
      void SetId(unsigned int x){ id = x; }

      ///Get the ID of the zone.
      unsigned int GetId(){ return id; }

      ///Set the zone type.
      void SetType(ptString x){ type = x; }

      ///Get the zone type.
      ptString GetType(){ return type; }

      ///Add a node to the zone.
      void AddNode(WFMath::Point<2> node)
      {
        nodes.push_back(node);
      }

      ///Get the number of nodes in this zone.
      size_t GetSize(){ return nodes.size(); }

      ///Get a node from the zone.
      WFMath::Point<2> GetNode(size_t x){ return nodes[x]; }
    };
  } // Data namespace
} // PT namespace

#endif // PT_DATAZONE_H
