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

#ifndef PT_DATASPAWNPOINTDATAMANAGER_H
#define PT_DATASPAWNPOINTDATAMANAGER_H

#include <vector>
#include <string>

#include "src/client/data/datamanager.h"

namespace PT
{
  namespace Data
  {
    //Forward declarations
    class SpawnPoint;

    /**
     * @ingroup data_handlers
     * Handles the loading of spawnpoint data from XML definition file. The data
     * resides in $DATA/xml/spawnpoints/spawnpoints.xml file, where $DATA is the parent
     * directory where the Peragro Tempus data resides.
     * @author Branko Majic <branko.majic@gmail.com>
     */
    class SpawnPointDataManager : public DataManager
    {
    private:
      ///SpawnPoint descriptions. Instances are owned by SpawnPointDataManager.
      ///@internal We're using a std::vector here since it's being populated
      ///only during application startup. It also offers speed when accessing
      ///elements.
      std::vector<SpawnPoint*> spawnpoints;

    public:
      /**
       * Base constructor
       */
      SpawnPointDataManager(PointerLibrary* ptrlib);
      ~SpawnPointDataManager();

      void GetAllSpawnPoints(std::vector<SpawnPoint*>& list) { list = spawnpoints; }

      /// Implements the DataManager superclass
      bool parseElement(iDocumentNode* node);

      /// Root Node of the xml: &lt;spawnpoints&gt;
      const char* getRootName() { return "spawnpoints"; }

      /// Element Node of the xml: &lt;spawnpoint&gt;
      const char* getElementName() { return "spawnpoint"; }
    };
  } // Data namespace
} // PT namespace

#endif // PT_DATASPAWNPOINTDATAMANAGER_H
