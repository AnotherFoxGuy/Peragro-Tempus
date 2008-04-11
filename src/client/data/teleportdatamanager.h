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

#ifndef PT_DATATELEPORTDATAMANAGER_H
#define PT_DATATELEPORTDATAMANAGER_H

#include <string>
#include <vector>

#include "datamanager.h"

namespace PT
{
  namespace Data
  {
    //Forward declarations
    class Teleport;

    /**
     * @ingroup data_handlers
     * Handles the loading of teleport data from XML definition file. The data
     * resides in $DATA/xml/teleports/teleports.xml file, where $DATA is the
     * parent directory where the Peragro Tempus data resides.
     * @author Branko Majic <branko.majic@gmail.com>
     */
    class TeleportDataManager : public DataManager
    {
    private:
      ///Teleport descriptions. Instances are owned by TeleportDataManager.
      ///@internal We're using a std::vector here since it's being populated
      ///only during application startup. It also offers speed when accessing
      ///elements.
      std::vector<Teleport*> teleports;

    public:
      /**
       * Convenience constructor allowing for immediate setting of data
       * directory path.
       * @param path Path to the data directory.
       */
      TeleportDataManager(PointerLibrary* ptrlib);
      ~TeleportDataManager();

      /**
       * @param id Unique ID of the wanted teleport.
       * @return Pointer to teleport with given ID, or 0 if none was found.
       */
      Teleport* GetTeleportById(unsigned int id) const;
      /**
       * @param name Name of the wanted teleport.
       * @return Pointer to teleport with given Name, or 0 if none was found.
       */
      Teleport* GetTeleportByName(const std::string& name) const;

      /**
       * @return Number of defined teleports.
       */
      size_t GetTeleportCount() { return teleports.size(); }

      /**
       * @param i Index of teleport within the teleports vector.
       * @return Teleport within vector given index.
       */
      Teleport* operator[](size_t i) { return teleports[i]; }

      /// Implements the DataManager superclass
      bool parseElement(iDocumentNode* node);

      /// Root Node of the xml: &lt;doors&gt;
      const char* getRootName() { return "teleports"; }

      /// Element Node of the xml: &lt;door&gt;
      const char* getElementName() { return "teleport"; }
    };
  }
}

#endif
