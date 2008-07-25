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

#ifndef EQUIPMENT_H
#define EQUIPMENT_H

#include "equipeditem.h"

#include <csutil/parray.h>

#include "client/pointer/pointer.h"

namespace PT
{
  namespace Entity
  {
    class Equipment
    {
    private:
      csPDelArray<EquipedItem> equipment;
      Entity* entity;
      /*
      Checks if an item with the slotname exists,
      if so delete it and add the new item.
      */
      void AddItem(EquipedItem* Item);
      void RemoveItem(unsigned int slotid);

    public:
      Equipment(Entity* entity);
      ~Equipment() {}
      void Equip(unsigned int slotId, unsigned int itemId, const char* meshname, const char* meshfile);
      void UnEquip(unsigned int slotId);

      Entity* GetEntity() { return entity; }
      void ClearAll(); // Destructs all meshes and wipes the array.
      void ConstructMeshes(); // Constructs meshes for all equiped items.
      void DestructMeshes(); // Handy for LOD purposes.
    };
  }
}

#endif // EQUIPMENT_H
