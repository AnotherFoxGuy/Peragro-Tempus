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

#ifndef PT_DATASPAWNPOINT_H
#define PT_DATASPAWNPOINT_H

#include <string>
#include "common/util/ptvector3.h"

namespace PT
{
  namespace Data
  {
    /**
     * @ingroup data_handlers
     * Helper class that contains basic information about spawnpoint. Usually not
     * created directly, but accessed via SpawnPointDataManager instance. Use the
     * Set/Get methods to access the spawnpoint properties.
     * @see SpawnPointDataManager
     * @author Branko Majic <branko.majic@gmail.com>
     */
    class SpawnPoint
    {
    private:

      unsigned int item;

      unsigned int variation;

      ///Position of the spawnpoint in a sector (ie x='20', y='30', z='40').
      PtVector3 position;
      ///Name of the sector where the spawnpoint resides (ie 'room').
      std::string sectorName;

      unsigned int interval;

    public:
      SpawnPoint() {}

      void SetItem(unsigned int value) { item = value; }
      unsigned int GetItem() const { return item; }

      void SetVariation(unsigned int value) { variation = value; }
      unsigned int GetVariation() const { return variation; }

      void SetPosition(double x,double y,double z)
        { position = PtVector3(x,y,z); }
      void SetPosition(const PtVector3& value) { position = value;}
      const PtVector3& GetPosition() const { return position; }

      void SetSectorName(const std::string& value) { sectorName = value; }
      const std::string& GetSectorName() const { return sectorName; }

      void SetInterval(unsigned int value) { interval = value; }
      unsigned int GetInterval() const { return interval; }
    };
  } // Data namespace
} // PT namespace

#endif // PT_DATASPAWNPOINT_H
