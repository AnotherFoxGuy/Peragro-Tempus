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

#ifndef PTCELENTITY_H
#define PTCELENTITY_H


#include "cssysdef.h"

#include "iutil/string.h"
#include "iutil/vfs.h"
#include "csutil/ref.h"
#include "csutil/csstring.h"

class Item
{
private:
  uint id;
  csString name;
  csString iconname;
  csString meshname;
  csString description;

public:
  Item();
  ~Item();
  uint GetId() { return id; }
  csString GetName() { return name; }
  csString GetIconName() { return iconname;}
  csString GetMeshName() { return meshname;}
  csString GetDescription() { return description;} 

  void SetId(uint value) { id = value; }
  void SetName(csString value) { name = value; }
  void SetIconName(csString value) { iconname = value; }
  void SetMeshName(csString value) { meshname = value; }
  void SetDescription(csString value) { description = value; }
};

#endif // PTCELENTITY_H
