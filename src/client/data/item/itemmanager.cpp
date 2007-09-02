/*
    Copyright (C) 2005 Development Team of Peragro Tempus

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

#include "itemmanager.h"

#include <iutil/vfs.h>
#include <iutil/objreg.h>
#include <imap/loader.h>

#include <csutil/xmltiny.h>

#include "client/reporter/reporter.h"

namespace PT
{
  namespace Data
  {
    ItemManager::ItemManager (iObjectRegistry* obj_reg)
    {
      engine = csQueryRegistry<iEngine> (obj_reg);
      //if (!engine) return ReportError("Failed to locate 3D engine!");

      vfs = csQueryRegistry<iVFS> (obj_reg);
      //if (!vfs) return ReportError("Failed to locate VFS!");

      stringset = csQueryRegistryTagInterface<iStringSet> (obj_reg,
        "crystalspace.shared.stringset");

      docsys = csQueryRegistry<iDocumentSystem> (obj_reg);
    }

    ItemManager::~ItemManager ()
    {
      items.DeleteAll();
    }

    bool ItemManager::Initialize ()
    {
      // Clear the array.
      items.DeleteAll();

      csRef<iFile> buf = vfs->Open("/peragro/xml/items/items.xml", VFS_FILE_READ);

      if (!buf)
        return Report(PT::Error, "ItemManager: Couldn't open item file!");

      csRef<iDocument> doc;
      csRef<iDocumentNode> node;

      if (!docsys) docsys = csPtr<iDocumentSystem> (new csTinyDocumentSystem ());
      doc = docsys->CreateDocument ();
      const char* error = doc->Parse (buf, true);
      if (error != 0)
        return Report(PT::Error, "ItemManager: Couldn't open item file!");

      if (doc)
      {
        node = doc->GetRoot()->GetNode("items");
        if (!node)
          return Report(PT::Error, "ItemManager: Couldn't open item file!");
      }

      Report(PT::Debug, "==Loading items==========================");
      csRef<iDocumentNodeIterator> it = node->GetNodes ();
      while (it->HasNext ())
      {
        csRef<iDocumentNode> child = it->Next ();
        csRef<iDocumentNode> id          = child->GetNode ("id");
        csRef<iDocumentNode> name        = child->GetNode ("name");
        csRef<iDocumentNode> icon        = child->GetNode ("icon");
        csRef<iDocumentNode> description = child->GetNode ("description");
        csRef<iDocumentNode> file        = child->GetNode ("file");
        csRef<iDocumentNode> mesh        = child->GetNode ("mesh");

        if (!id || !name || !icon || !description || !file || !mesh)
        {
          csString error;
          csString good;
          if(!id) error = "<id>"; else good = id->GetContentsValue ();
          if(!name) error += "<name>"; else good = name->GetContentsValue ();
          if(!icon) error += "<icon>";
          if(!description) error += "<description>";
          if(!file) error += "<file>";
          if(!mesh) error += "<mesh>";

          Report(PT::Error, "ItemManager: Missing %s token(s) for %s!", error.GetData(), good.GetData());
          continue;
        }

        Item* item = new Item();

        item->SetId(id->GetContentsValueAsInt()); 
        item->SetName((csString)name->GetContentsValue ()); 
        item->SetIconName((csString)icon->GetContentsValue ());
        item->SetDescription((csString)description->GetContentsValue ()); 
        item->SetFileName((csString)file->GetContentsValue ()); 
        item->SetMeshName((csString)mesh->GetContentsValue ()); 

        if (item->GetId() >= items.Capacity())
          items.SetCapacity(item->GetId()+1);

        items.Put(item->GetId(), item);
        Report(PT::Debug, "%d : %s", item->GetId(), item->GetName().GetData());
      }
      Report(PT::Debug, "================================= %d item(s)\n", items.GetSize()-1);

      return true;
    }

    Item* ItemManager::GetItemById(uint id)
    {
      if (id < items.GetSize())
      {
        return items[id];
      }
      Report(PT::Error, "ItemManager: Couldn't find item %d!", id);
      return 0;
    }

    Item* ItemManager::GetItemByName(csString name)
    {
      for (size_t i = 0; i < items.GetSize(); i++)
      {
        Item* item = items[i];
        if(item)
          if(item->GetName().Compare(name))
            return item;
      }
      Report(PT::Error, "ItemManager: Couldn't find item %s!", name.GetData());
      return 0;
    }

  } // Data namespace 
} // PT namespace 
