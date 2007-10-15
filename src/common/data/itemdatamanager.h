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

namespace PT
{
  namespace Data
  {
    //Forward declarations
    class Item;

    /**
      @ingroup data_handlers
      Handles the loading of items data from XML definition file. The data
      resides in $DATA/xml/items/items.xml file, where $DATA is the parent
      directory where the Peragro Tempus data resides.
      @author Branko Majic <branko.majic@gmail.com>
    */
    class ItemDataManager
    {
    private:
      ///Items descriptions. Instances are owned by ItemDataManager.
      ///@internal We're using a std::vector here since it's being populated
      ///only during application startup. It also offers speed when accessing
      ///elements.
      std::vector<Item*> items;
      ///Represents the parent directory of the Peragro Tempus data.
      std::string dataPath;

    public:
      /**
        Base constructor that doesn't do anything at all.
       */
      ItemDataManager() {}
      /**
        Convenience constructor allowing for immediate setting of data
        directory path.
        @param path Path to the data directory.
       */
      ItemDataManager(std::string path) : dataPath(path) {}
      ~ItemDataManager();

      /**
        Set the data directory path used for loading 'items.xml' file.
       */
      void SetDataPath(const std::string& value) { dataPath = value; }
      /**
        @return Data directory path.
       */
      std::string GetDataPath() const { return dataPath; }

      /**
        Loads all the items information located in items definition file.
        @return True if successful, false if an error occured.
       */
      bool LoadItemData();

      /**
        @param id Unique ID of the wanted item.
        @return Pointer to item with given ID, or 0 if none was found.
       */
      Item* GetItemById(uint id) const;
      /**
        @param name Name of the wanted item.
        @return Pointer to item with given ID, or 0 if none was found.
       */
      Item* GetItemByName(const std::string& name) const;
    };
  }
}

#endif
