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

#include "cssysdef.h"
#include "csutil/ref.h"
#include "iutil/vfs.h"
#include "iutil/virtclk.h"
#include "iengine/engine.h"
#include "csgeom/path.h"

struct iObjectRegistry;
struct iLoader;

class ItemManager
{
private:
  csArray<Item*> items;

public:
  ItemManager(iObjectRegistry* obj_reg);
  ~ItemManager();
  void Initialize();
  Item* GetItemById(uint id);
  Item* GetItemByName(csString name);  
};

#endif // ITEMMANAGER_H
