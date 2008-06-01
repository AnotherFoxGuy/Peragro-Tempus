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

#ifndef PT_DATASECTORDATAMANAGER_H
#define PT_DATASECTORDATAMANAGER_H

#include <vector>
#include <string>

#include "src/client/data/datamanager.h"

namespace PT
{
  namespace Data
  {
    //Forward declarations
    class Sector;

    /**
     * @ingroup data_handlers
     * Handles the loading of sector data from XML definition file. The data
     * resides in $DATA/xml/world/sectors.xml file, where $DATA is the parent
     * directory where the Peragro Tempus data resides.
     */
    class SectorDataManager : public DataManager
    {
    private:
      ///Sector descriptions. Instances are owned by SectorDataManager.
      ///@internal We're using a std::vector here since it's being populated
      ///only during application startup. It also offers speed when accessing
      ///elements.
      std::vector<Sector*> sectors;

    public:
      /**
       * Base constructor
       */
      SectorDataManager(PointerLibrary* ptrlib);
      ~SectorDataManager();

      /**
       * @param id Unique ID of the wanted sector.
       * @return Pointer to item with given ID, or 0 if none was found.
       */
      Sector* GetSectorById(unsigned int id) const;
      /**
       * @param name Name of the wanted sector.
       * @return Pointer to item with given Name, or 0 if none was found.
       */
      Sector* GetSectorByName(const std::string& name) const;

      void GetAllSectors(std::vector<Sector*>& list) { list = sectors; }

      /// Implements the DataManager superclass
      bool parseElement(iDocumentNode* node);

      /// Root Node of the xml: &lt;doors&gt;
      const char* getRootName() { return "sectors"; }

      /// Element Node of the xml: &lt;door&gt;
      const char* getElementName() { return "sector"; }
    };
  } // Data namespace
} // PT namespace

#endif // PT_DATASECTORDATAMANAGER_H
