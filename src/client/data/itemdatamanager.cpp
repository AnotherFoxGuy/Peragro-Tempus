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

#include <cssysdef.h>
#include <iutil/document.h>

#include "itemdatamanager.h"
#include "item.h"

namespace PT
{
  namespace Data
  {

    ItemDataManager::ItemDataManager()
    {
      file = "/peragro/xml/items/items.xml";
    }

    ItemDataManager::~ItemDataManager()
    {
      for (size_t i = 0; i<items.size(); i++) delete items[i];
    }

    ///@internal Currently the items vector is not preallocated, since there's
    ///no efficient way to get the count of items described in XML file. If need
    ///be, we could traverse the XML file once before adding the actual data in
    ///order to determine the number of items in file, and preallocate memory
    ///space.
    bool ItemDataManager::parseElement(iDocumentNode* node)
    {
      Item* item = new Item();

      item->SetId(node->GetNode("id")->GetContentsValueAsInt());

      item->SetName(node->GetNode("name")->GetContentsValue());

      item->SetIconName(node->GetNode("icon") ->GetContentsValue());

      item->SetDescription(node->GetNode("description")->GetContentsValue());

      item->SetMeshFile(node->GetNode("file")->GetContentsValue());

      item->SetMeshName(node->GetNode("mesh")->GetContentsValue());

      item->SetWeight(node->GetNode("weight")->GetContentsValueAsFloat());

      items.push_back(item);

      return true;
    }

    Item* ItemDataManager::GetItemById(unsigned int id) const
    {
      for (size_t i = 0; i<items.size(); i++)
        if (items[i]->GetId() == id) return items[i];

      return 0;
    }

    Item* ItemDataManager::GetItemByName(const std::string& name) const
    {
      for (size_t i = 0; i<items.size(); i++)
        if (items[i]->GetName() == name) return items[i];

      return 0;
    }
  } // Data namespace
} // PT namespace
