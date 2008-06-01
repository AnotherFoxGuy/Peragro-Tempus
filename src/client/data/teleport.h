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

#ifndef PT_DATATELEPORT_H
#define PT_DATATELEPORT_H

#include <string>
#include "common/util/ptvector3.h"

namespace PT
{
  namespace Data
  {
    /**
     * @ingroup data_handlers
     * Helper class that contains basic information about teleport. Usually not
     * created directly, but accessed via TeleportDataManager instance. Use the
     * Set/Get methods to access the teleport properties.
     * @see TeleportDataManager
    */
    class Teleport
    {
    private:
      ///Unique ID of the teleport.
      unsigned int id;
      ///Name of the teleport.
      std::string name;
      ///Description of the teleport
      std::string description;
      ///Name of the mesh to be used for the teleport.
      std::string meshName;
      ///Position of a teleport in the sector.
      PtVector3 position;
      ///Name of the sector where teleport resides.
      std::string sectorName;
      ///Exit position of a teleport.
      PtVector3 destinationPosition;
      ///Exit sector of a teleport.
      std::string destinationSector;

    public:
      Teleport() : id(0) {}

      void SetId(unsigned int value) { id = value; }
      unsigned int GetId() const { return id; }

      void SetName(const std::string& value) { name = value; }
      const std::string& GetName() const { return name; }

      void SetDescription(const std::string& value) { description = value; }
      const std::string& GetDescription() const { return description; }

      void SetMeshName(const std::string& value) { meshName = value; }
      const std::string& GetMeshName() const { return meshName; }

      void SetPosition(const PtVector3& value) { position = value; }
      const PtVector3& GetPosition() const  { return position; }

      void SetSectorName(const std::string& value) { sectorName = value; }
      const std::string& GetSectorName() const { return sectorName; }

      void SetDestinationPosition(const PtVector3& value)
        { destinationPosition = value; }
      const PtVector3& GetDestinationPosition() const
        { return destinationPosition; }

      void SetDestinationSector(const std::string& value)
        { destinationSector = value; }
      const std::string& GetDestinationSector() const
        { return destinationSector; }
    };
  }
}

#endif
