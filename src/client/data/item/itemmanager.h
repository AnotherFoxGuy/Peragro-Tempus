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

#ifndef ITEMMANAGER_H
#define ITEMMANAGER_H

#include <cssysdef.h>
#include <csutil/ref.h>
#include <iutil/vfs.h>
#include <iutil/virtclk.h>
#include <iengine/engine.h>
#include <csgeom/path.h>
#include <csutil/parray.h>

#include <iutil/strset.h>
#include <iutil/document.h>

#include "client/data/item/item.h"

struct iObjectRegistry;
struct iLoader;
struct iDocument;

namespace PT
{
  namespace Data
  {
    class ItemManager
    {
    private:
      csPDelArray<Item> items;

      csRef<iEngine> engine;
      csRef<iVFS> vfs;
      csRef<iStringSet> stringset;
      csRef<iDocumentSystem> docsys;

    public:
      ItemManager(iObjectRegistry* obj_reg);
      ~ItemManager();
      bool Initialize();
      Item* GetItemById(uint id);
      Item* GetItemByName(csString name);  
    };

  } // Data namespace 
} // PT namespace 

#endif // ITEMMANAGER_H
