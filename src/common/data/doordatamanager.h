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

#ifndef PT_DATADOORDATAMANAGER_H
#define PT_DATADOORDATAMANAGER_H

#include <vector>
#include <string>

namespace PT
{
  namespace Data
  {
    //Forward declarations
    class Door;

    /**
      @ingroup data_handlers
      Handles the loading of door data from XML definition file. The data
      resides in $DATA/xml/doors/doors.xml file, where $DATA is the parent
      directory where the Peragro Tempus data resides.
      @author Branko Majic <branko.majic@gmail.com>
    */
    class DoorDataManager
    {
    private:
      ///Door descriptions. Instances are owned by DoorDataManager.
      ///@internal We're using a std::vector here since it's being populated
      ///only during application startup. It also offers speed when accessing
      ///elements.
      std::vector<Door*> doors;
      ///Represents the parent directory of the Peragro Tempus data.
      std::string dataPath;

    public:
      /**
        Base constructor that doesn't do anything at all.
       */
      DoorDataManager() {}
      /**
        Convenience constructor allowing for immediate setting of data
        directory path.
        @param path Path to the data directory.
       */
      DoorDataManager(const std::string& path) : dataPath(path) {}
      ~DoorDataManager();

      /**
        Set the data directory path used for loading 'doors.xml' file.
       */
      void SetDataPath(const std::string& value) { dataPath = value; }
      /**
        @return Data directory path.
       */
      const std::string& GetDataPath() const { return dataPath; }

      /**
        Loads all the doors information located in doors definition file.
        @return True if successful, false if an error occured.
       */
      bool LoadDoorData();

      /**
        @param id Unique ID of the wanted door.
        @return Pointer to door with given ID, or 0 if none was found.
       */
      Door* GetDoorById(unsigned int id) const;
      /**
        @param name Name of the wanted door.
        @return Pointer to door with given ID, or 0 if none was found.
       */
      Door* GetDoorByName(const std::string& name) const;
    };
  } // Data namespace
} // PT namespace

#endif // PT_DATADOORDATAMANAGER_H
