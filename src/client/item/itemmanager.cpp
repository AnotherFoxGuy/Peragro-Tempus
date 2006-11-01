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

#include "client/item/itemmanager.h"

#include "cstool/initapp.h"
#include "csutil/cmdline.h"
#include "csutil/csstring.h"
#include "csutil/csshlib.h"
#include "csutil/event.h"
#include "csutil/sysfunc.h"
#include "csutil/syspath.h"

#include "csgeom/math3d.h"

#include "iengine/camera.h"
#include "iengine/campos.h"
#include "iengine/mesh.h"
#include "iengine/sector.h"
#include "iengine/texture.h"
#include "iengine/material.h"
#include "iengine/scenenode.h"
#include "iengine/movable.h"
#include "imesh/object.h"
#include "imesh/spritecal3d.h"
#include "imesh/sprite2d.h"
#include "imesh/genmesh.h"
#include "iutil/databuff.h"
#include "iutil/event.h"
#include "iutil/eventq.h"
#include "iutil/object.h"
#include "iutil/vfs.h"
#include "ivaria/collider.h"
#include "ivideo/graph2d.h"
#include "ivideo/natwin.h"
#include "ivideo/txtmgr.h"
#include "ivideo/material.h"

#include "iutil/objreg.h"
#include "imap/loader.h"

#include "csutil/xmltiny.h"



ItemMGR::ItemMGR (iObjectRegistry* obj_reg)
{
  engine = CS_QUERY_REGISTRY(obj_reg, iEngine);
  //if (!engine) return ReportError("Failed to locate 3D engine!");

  vfs = CS_QUERY_REGISTRY(obj_reg, iVFS);
  //if (!vfs) return ReportError("Failed to locate VFS!");

  stringset = CS_QUERY_REGISTRY_TAG_INTERFACE (obj_reg,
    "crystalspace.shared.stringset", iStringSet);

  docsys = CS_QUERY_REGISTRY(obj_reg, iDocumentSystem);
}

ItemMGR::~ItemMGR ()
{
  items.DeleteAll();
}

bool ItemMGR::Initialize ()
{
  //items->Clear();

  csRef<iFile> buf = vfs->Open("/peragro/xml/items/items.xml", VFS_FILE_READ);

  if (!buf)
  {
    printf("ItemManager: ERROR Couldn't open item file!\n");
    return false;
  }

  csRef<iDocument> doc;
  csRef<iDocumentNode> node;
  
  if (!docsys) docsys = csPtr<iDocumentSystem> (new csTinyDocumentSystem ());
  doc = docsys->CreateDocument ();
  const char* error = doc->Parse (buf, true);
  if (error != 0)
  {
    printf("ItemManager: ERROR Couldn't open item file!\n");
    return false;
  }

  if (doc)
  {
    node = doc->GetRoot()->GetNode("items");
    if (!node)
    {
      printf("ItemManager: ERROR Couldn't open item file!\n");
      return false;
    }
  }

  printf("\n==Loading items==========================\n");
  csRef<iDocumentNodeIterator> it = node->GetNodes ();
  while (it->HasNext ())
  {
    csRef<iDocumentNode> child = it->Next ();
    //if (child->GetType () != CS_NODE_ELEMENT) continue;
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

      printf("ItemManager: ERROR Missing %s token(s) for %s!\n\n", error.GetData(), good.GetData());
      //return false;
      continue;
    }

    ClientItem* item = new ClientItem();

    item->SetId(id->GetContentsValueAsInt()); 
    item->SetName((csString)name->GetContentsValue ()); 
    item->SetIconName((csString)icon->GetContentsValue ());
    item->SetDescription((csString)description->GetContentsValue ()); 
    item->SetFileName((csString)file->GetContentsValue ()); 
    item->SetMeshName((csString)mesh->GetContentsValue ()); 

    if (item->GetId() >= items.Capacity())
      items.SetCapacity(item->GetId()+1);

    items.Put(item->GetId(), item);
    printf("%d : %s\n", item->GetId(), item->GetName().GetData());
  }
  printf("================================= %d item(s)\n\n", items.Length()-1);

  return true;
}

ClientItem* ItemMGR::GetItemById(uint id)
{
  if (id < items.Length())
  {
    return items[id];
  }
  printf("ItemMGR: ERROR Couldn't find item %d!\n", id);
  return 0;
}

ClientItem* ItemMGR::GetItemByName(csString name)
{
  for (size_t i = 0; i < items.Length(); i++)
   {
     ClientItem* item = items[i];
     if(item)
       if(item->GetName().Compare(name))
         return item;
   }
  printf("ItemMGR: ERROR Couldn't find item %s!\n", name.GetData());
  return 0;
}
