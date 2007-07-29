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

#ifndef EQUIPEDITEM_H
#define EQUIPEDITEM_H

#include <cssysdef.h>

#include <string>

#include "client/entity/ptentity.h"

class EquipedItem
{
 private:
   unsigned int id;
   unsigned int slotId;
   iCelEntity* itementity;
   PtEntity* entity;
   std::string GetSocketName(unsigned int slotid);

 public:
   EquipedItem(PtEntity* entity, unsigned int slotId, unsigned int itemId);
   ~EquipedItem(){}
   unsigned int GetId(){ return id; }
   unsigned int GetSlotId() { return slotId; }
   iCelEntity* GetItemEntity() { return itementity; }
   void ConstructMesh(); // Creates the mesh and adds it to the socket.
   void DestructMesh();
}; 

#endif // EQUIPEDITEM_H
