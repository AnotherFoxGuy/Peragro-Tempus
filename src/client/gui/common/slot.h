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

#ifndef SLOT_H
#define SLOT_H


#include "cssysdef.h"

#include "iutil/string.h"
#include "iutil/vfs.h"
#include "csutil/ref.h"
#include "csutil/csstring.h"

#include "CEGUI.h"
#include "ivaria/icegui.h"

class Slot
{
private:
  uint slotid;
  uint objectid;
  uint amount;
  uint slottype;
  bool stackable;
  CEGUI::Window* objectwindow;
  CEGUI::Window* slotwindow;


public:
  Slot();
  ~Slot();
  uint GetSlotId() { return slotid; }
  uint GetObjectId() { return objectid; }
  uint GetAmount() { return amount; }
  uint GetSlotType() { return slottype; }
  bool GetStackable() { return stackable; }
  CEGUI::Window* GetObjectWindow() { return objectwindow; }
  CEGUI::Window* GetSlotWindow() { return slotwindow; }

  void SetSlotId(uint value) { this->slotid = value; }
  void SetObjectId(uint value) { this->objectid = value; }
  void SetAmount(uint value) { this->amount = value; }
  void SetSlotType(uint value) { this->slottype = value; }
  void SetStackable(bool value) { this->stackable = value; }
  void SetObjectWindow(CEGUI::Window* value) { this->objectwindow = value; }
  void SetSlotWindow(CEGUI::Window* value) { this->slotwindow = value; }

  bool IsEmpty();
  void Clear();
  Slot* operator = (Slot* slot);

};

#endif // SLOT_H
