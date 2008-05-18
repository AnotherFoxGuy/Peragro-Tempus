/*
    Copyright (C) 2005 Development Team of Peragro Tempus

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General public License for more details.

    You should have received a copy of the GNU General public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef INVENTORY_H
#define INVENTORY_H

#include "cssysdef.h"

#include "iutil/string.h"
#include "iutil/vfs.h"
#include "csutil/ref.h"
#include "csutil/csstring.h"

#include "CEGUI.h"
#include "ivaria/icegui.h"

#include "client/gui/guimanager.h"

#include "client/gui/common/dragdrop-gui.h"

namespace PT
{
  namespace GUI
  {
    namespace Windows
    {
      class DragDrop;
    } // Windows namespace
  } // GUI namespace
} // PT namespace

class Inventory
{
public:
  enum ParentType
  {
    InventoryUpper=0,
    InventoryLower=1,
    TradeLeft=2,
    TradeRight=3,
    BuyUpper=4,
    BuyLower=5,
    SellUpper=6,
    SellLower=7
  };

  struct ObjectAndSlot
  {
    Object* object;
    Slot*  slot;
  };

private:
  ParentType parent;
  csArray<Slot*> slotarray;
  CEGUI::Window* bag;
  PT::GUI::Windows::DragDrop* dragdrop;
  PT::GUI::GUIManager* guimanager;
  CEGUI::WindowManager* winMgr;

private:
  CEGUI::Window* createDragDropSlot(CEGUI::Window* parent,
                                    const CEGUI::UVector2& position);

public:
  Inventory(PT::GUI::GUIManager* guimanager);
  ~Inventory();
  void Create(CEGUI::Window* bag, Inventory::ParentType parent,
              PT::GUI::Windows::DragDrop::Type type , int rows,
              int columns, int offset = 0);

  void ClearSlotsDelete();
  unsigned int FindFreeSlot();
  unsigned int FindObject(unsigned int objectid);
  bool RemoveObject(unsigned int slotid);

  csArray<ObjectAndSlot> GetAllObjects();

  Slot* GetSlot(unsigned int slotid)
  {
    if (slotid >= slotarray.GetSize()) return 0;
    return slotarray[slotid];
  }

  csArray<Slot*>* GetSlotArray() { return &slotarray; }

};

#endif // INVENTORY_H
