/*
    Copyright (C) 2007-2008 Development Team of Peragro Tempus

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
/**
 * @file itemdatamanager.h
 *
 * @basic Handles the loading of items data from XML definition file.
 */

#ifndef PT_ITEMDATAMANAGER_H
#define PT_ITEMDATAMANAGER_H

#include <vector>
#include <string>

#include "src/client/data/datamanager.h"

namespace PT
{
  namespace Data
  {
    // Forward declarations
    class Item;

    /**
     * @ingroup data_handlers
     * Handles the loading of items data from XML definition file. The data
     * resides in $DATA/xml/items/items.xml file, where $DATA is the parent
     * directory where the Peragro Tempus data resides.
     */
    class ItemDataManager : public DataManager
    {
    private:
      /// Items descriptions. Instances are owned by ItemDataManager.
      /// @internal We're using a std::vector here since it's being populated.
      /// only during application startup. It also offers speed when accessing.
      /// elements.
      std::vector<Item*> items;

    public:
      /**
       * Base constructor
       */
      ItemDataManager(PointerLibrary* ptrlib);
      /**
       * Destructor.
       */
      ~ItemDataManager();

      /**
       * Get an item by ID.
       * @param id Unique ID of the wanted item.
       * @return Pointer to item with given ID, or 0 if none was found.
       */
      Item* GetItemById(unsigned int id) const;

      /**
       * Get an item by name.
       * @param name Name of the wanted item.
       * @return Pointer to item with given Name, or 0 if none was found.
       */
      Item* GetItemByName(const std::string& name) const;

      /**
       * Get a vector of all items.
       * @param list The list passed to populate with items.
       */
      void GetAllItems(std::vector<Item*>& list) { list = items; }

      /// Implements the DataManager superclass.
      bool parseElement(iDocumentNode* node);

      /// Parse the item xml files.
      bool parseItems();

      /// Root Node of the xml: &lt;items&gt;.
      const char* getRootName() { return 0; }

      /// Element Node of the xml: &lt;item&gt;.
      const char* getElementName() { return "item"; }
    };

  } // Data namespace
} // PT namespace

#endif // PT_ITEMDATAMANAGER_H
