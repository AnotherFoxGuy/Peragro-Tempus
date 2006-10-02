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

#ifndef OBJECT_H
#define OBJECT_H


#include "cssysdef.h"

#include "iutil/string.h"
#include "iutil/vfs.h"
#include "csutil/ref.h"
#include "csutil/csstring.h"

#include "CEGUI.h"
#include "ivaria/icegui.h"

class Object
{
private:
  uint objectid;
  uint amount;
  uint objecttype;
  bool stackable;
  CEGUI::Window* objectwindow;

public:
  Object();
  ~Object();
  uint GetId() { return objectid; }
  uint GetAmount() { return amount; }
  uint GetType() { return objecttype; }
  bool IsStackable() { return stackable; }
  CEGUI::Window* GetWindow() { return objectwindow; }

  void SetId(uint value) { this->objectid = value; }
  void SetAmount(uint value) { this->amount = value; }
  void SetType(uint value) { this->objecttype = value; }
  void SetStackable(bool value) { this->stackable = value; }
  void SetWindow(CEGUI::Window* value) { this->objectwindow = value; }
};

#endif // OBJECT_H
