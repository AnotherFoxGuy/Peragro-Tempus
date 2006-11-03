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

#include "client/gui/common/object.h"

class Slot
{
private:
  uint slotid;
  Object* object;
  uint slottype;
  uint parent;
  CEGUI::Window* slotwindow;

public:
  Slot();
  ~Slot();
  uint GetId() { return slotid; }
  Object* GetObject() { return object; }
  uint GetType() { return slottype; }
  uint GetParent() { return parent; }
  CEGUI::Window* GetWindow() { return slotwindow; }

  void SetId(uint value) { this->slotid = value; }
  void SetObject(Object* value) 
  { 
    this->object = value; 
    slotwindow->addChildWindow(value->GetWindow());
    value->GetWindow()->notifyScreenAreaChanged();
  }
  void SetType(uint value) { this->slottype = value; }
  void SetParent(uint value) { this->parent = value; }
  void SetWindow(CEGUI::Window* value) { this->slotwindow = value; }

  bool IsEmpty();
  void Clear();
  void MoveObjectTo(Slot* slot);

};

#endif // SLOT_H
