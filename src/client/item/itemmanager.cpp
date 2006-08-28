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

#include "client/item/item.h"
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


ItemManager::ItemManager (iObjectRegistry* obj_reg)
{
  engine = CS_QUERY_REGISTRY(obj_reg, iEngine);
  //if (!engine) return ReportError("Failed to locate 3D engine!");

  vfs = CS_QUERY_REGISTRY(obj_reg, iVFS);
  //if (!vfs) return ReportError("Failed to locate VFS!");

  stringset = CS_QUERY_REGISTRY_TAG_INTERFACE (obj_reg,
    "crystalspace.shared.stringset", iStringSet);
}

ItemManager::~ItemManager ()
{
}

bool ItemManager::Initialize ()
{
  //items->Clear();

  csRef<iFile> buf = VFS->Open ("/peragro/data/meshes/items.xml", VFS_FILE_READ);

  if (!buf)
  {
    printf("ItemManager: ERROR Couldn't open item file!\n");
    return false;
  }

  csRef<iDocument> doc;
  csRef<iDocumentSystem> docsys (CS_QUERY_REGISTRY (object_reg, iDocumentSystem));
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
    csRef<iDocumentNode> node = doc->GetRoot()->GetNode ("items");
    if (!node)
    {
      printf("ItemManager: ERROR Couldn't open item file!\n");
      return false;
    }
  }

  csRef<iDocumentNodeIterator> it = node->GetNodes ();
  while (it->HasNext ())
  {
    csRef<iDocumentNode> child = it->Next ();
    //if (child->GetType () != CS_NODE_ELEMENT) continue;
    csRef<iDocumentNode> id          = child->GetNode ("id");
    csRef<iDocumentNode> name        = child->GetNode ("name");
    csRef<iDocumentNode> icon        = child->GetNode ("icon");
    csRef<iDocumentNode> description = child->GetNode ("description");
    csRef<iDocumentNode> mesh        = child->GetNode ("mesh");

    Item item = new Item();

    item->SetId(id->GetContentsValueAsInt()); 
    item->SetName((csString)name->GetContentsValue ()); 
    item->SetIconName((csString)icon->GetContentsValue ()); 
    item->SetMeshName((csString)description->GetContentsValue ()); 
    item->SetDescription((csString)mesh->GetContentsValue ()); 
  }

  return true;
}

Item* ItemMGR::GetItemById(unit id)
{
  if(id > items.Lenght())
    { 
      printf("ItemMGR: ERROR: Recieved ID %d is bigger then Number of items!\n", id)
      return 0;
    }
  else
    return items->GetIndex(id);
}

Item* ItemMGR::GetItemByName(csString name)
{
  for (int i = 0; i < items.Length(); i++)
   {
     Item* item = items.GetIndex(i);
     if(item->GetName().Compare(name))
       return item;
   }
  printf("ItemMGR: ERROR Couldn't find item %s!\n", name.GetData());
  return 0;
}