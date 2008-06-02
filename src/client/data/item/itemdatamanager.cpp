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
#include <iutil/stringarray.h>

#include "itemdatamanager.h"
#include "item.h"
#include "client/pointer/pointer.h"
#include "client/reporter/reporter.h"

namespace PT
{
  namespace Data
  {

    ItemDataManager::ItemDataManager(PointerLibrary* ptrlib)
      : DataManager(ptrlib)
    {
    } // end ItemDataManager()

    ItemDataManager::~ItemDataManager()
    {
      for (size_t i = 0; i<items.size(); i++)
      {
        delete items[i];
      } // end for
    } // end ~ItemDataManager()

    bool ItemDataManager::parseItems()
    {
      csRef<iVFS> vfs = csQueryRegistry<iVFS>
        (PointerLibrary::getInstance()->getObjectRegistry());
      csRef<iStringArray> items = vfs->FindFiles("/peragro/xml/items/*.xml");

      while (items->GetSize() > 0)
      {
        file = static_cast<std::string>(items->Pop());
        Report(PT::Debug, "Parsing %s", file.c_str());
        parse();
      } // end while
      return true;
    } // end parseItems()

    /// @internal Currently the items vector is not preallocated, since there's
    /// no efficient way to get the count of items described in XML file. If need
    /// be, we could traverse the XML file once before adding the actual data in
    /// order to determine the number of items in file, and preallocate memory
    /// space.
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
      // Nodes below here are not always specified.
      if (node->GetNode("equiptype"))
      {
        item->SetEquiptype(node->GetNode("equiptype")->GetContentsValue());
      }
      if (node->GetNode("reach"))
      {
        item->SetReach(node->GetNode("reach")->GetContentsValueAsFloat());
      }
      if (node->GetNode("damage"))
      {
        item->SetDamage(node->GetNode("damage")->GetContentsValueAsFloat());
      }
      if (node->GetNode("quality"))
      {
        item->SetQuality(node->GetNode("quality")->GetContentsValueAsFloat());
      }
      if (node->GetNode("heft"))
      {
        item->SetHeft(node->GetNode("heft")->GetContentsValueAsFloat());
      }
      if (node->GetNode("durability"))
      {
        item->SetDurability(node->GetNode("durability")->
          GetContentsValueAsFloat());
      }
      if (node->GetNode("itemtype"))
      {
        item->SetItemtype(node->GetNode("itemtype")->GetContentsValue());
      }
      if (node->GetNode("pull"))
      {
        item->SetPull(node->GetNode("pull")->GetContentsValueAsFloat());
      }
      if (node->GetNode("range"))
      {
        item->SetRange(node->GetNode("range")->GetContentsValueAsFloat());
      }
      if (node->GetNode("amount"))
      {
        item->SetAmount(node->GetNode("amount")->GetContentsValueAsFloat());
      }
      if (node->GetNode("mobility"))
      {
        item->SetMobility(node->GetNode("mobility")->
          GetContentsValueAsFloat());
      }
      if (node->GetNode("hardness"))
      {
        item->SetHardness(node->GetNode("hardness")->
          GetContentsValueAsFloat());
      }
      if (node->GetNode("bulk"))
      {
        item->SetBulk(node->GetNode("bulk")->GetContentsValueAsFloat());
      }
      if (node->GetNode("toughness"))
      {
        item->SetToughness(node->GetNode("toughness")->
          GetContentsValueAsFloat());
      }

      items.push_back(item);

      return true;
    } // end parseElement()

    Item* ItemDataManager::GetItemById(unsigned int id) const
    {
      for (size_t i = 0; i<items.size(); i++)
      {
        if (items[i]->GetId() == id)
          return items[i];
      } // end for

      return 0;
    } // end GetItemById()

    Item* ItemDataManager::GetItemByName(const std::string& name) const
    {
      for (size_t i = 0; i<items.size(); i++)
      {
        if (items[i]->GetName() == name)
          return items[i];
      } // end for

      return 0;
    } // end GetItemByName()

  } // Data namespace
} // PT namespace
