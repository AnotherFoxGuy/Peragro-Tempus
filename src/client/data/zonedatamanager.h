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

#ifndef PT_DATAZONEDATAMANAGER_H
#define PT_DATAZONEDATAMANAGER_H

#include <vector>

#include "datamanager.h"
#include "zone.h"

namespace PT
{
  namespace Data
  {
    //Forward declarations
    class Zone;

    /**
     * @ingroup data_handlers
     * Handles the loading of zone data from XML definition file. The data
     * resides in $DATA/xml/zones/zones.xml file, where $DATA is the parent
     * directory where the Peragro Tempus data resides.
     */
    class ZoneDataManager : public DataManager
    {
    private:
      ///Zone descriptions. Instances are owned by ZoneDataManager.
      ///@internal We're using a std::vector here since it's being populated
      ///only during application startup. It also offers speed when accessing
      ///elements.
      std::vector<Zone*> zones;

    public:
      /**
       * Convenience constructor allowing for immediate setting of data
       * directory path.
       * @param path Path to the data directory.
       */
      ZoneDataManager(PointerLibrary* ptrlib);
      ~ZoneDataManager();

      void GetAllZones(std::vector<Zone*>& list) { list = zones; }

      /// Implements the DataManager superclass
      bool parseElement(iDocumentNode* node);

      /// Root Node of the xml: &lt;zones&gt;
      const char* getRootName() { return "zones"; }

      /// Element Node of the xml: &lt;zone&gt;
      const char* getElementName() { return "zone"; }
    };
  } // Data namespace
} // PT namespace

#endif // PT_DATAZONEDATAMANAGER_H
