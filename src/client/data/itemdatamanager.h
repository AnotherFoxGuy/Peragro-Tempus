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

#ifndef PT_ITEMDATAMANAGER_H
#define PT_ITEMDATAMANAGER_H

#include <vector>
#include <string>

#include "datamanager.h"

namespace PT
{
  namespace Data
  {
    //Forward declarations
    class Item;

    /**
     * @ingroup data_handlers
     * Handles the loading of items data from XML definition file. The data
     * resides in $DATA/xml/items/items.xml file, where $DATA is the parent
     * directory where the Peragro Tempus data resides.
     * @author Branko Majic <branko.majic@gmail.com>
     */
    class ItemDataManager : public DataManager
    {
    private:
      ///Items descriptions. Instances are owned by ItemDataManager.
      ///@internal We're using a std::vector here since it's being populated
      ///only during application startup. It also offers speed when accessing
      ///elements.
      std::vector<Item*> items;

    public:
      /**
       * Convenience constructor allowing for immediate setting of data
       * directory path.
       * @param path Path to the data directory.
       */
      ItemDataManager();
      ~ItemDataManager();

      /**
       * @param id Unique ID of the wanted item.
       * @return Pointer to item with given ID, or 0 if none was found.
       */
      Item* GetItemById(unsigned int id) const;
      /**
       * @param name Name of the wanted item.
       * @return Pointer to item with given Name, or 0 if none was found.
       */
      Item* GetItemByName(const std::string& name) const;

      /// Implements the DataManager superclass
      bool parseElement(iDocumentNode* node);

      /// Root Node of the xml: &lt;doors&gt;
      const char* getRootName() { return "items"; }

      /// Element Node of the xml: &lt;door&gt;
      const char* getElementName() { return "item"; }
    };
  } // Data namespace
} // PT namespace

#endif // PT_ITEMDATAMANAGER_H
