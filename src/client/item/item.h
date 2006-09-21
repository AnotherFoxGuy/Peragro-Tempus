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

#ifndef CLIENT_ITEM_H
#define CLIENT_ITEM_H


#include "cssysdef.h"

#include "iutil/string.h"
#include "iutil/vfs.h"
#include "csutil/ref.h"
#include "csutil/csstring.h"

class ClientItem
{
private:
  uint id;
  csString name;
  csString iconname;
  csString meshname;
  csString filename;
  csString description;
  uint stackable;

public:
  ClientItem();
  ~ClientItem();
  uint GetId() { return id; }
  csString GetName() { return name; }
  csString GetIconName() { return iconname;}
  csString GetMeshName() { return meshname;}
  csString GetFileName() { return filename;}
  csString GetDescription() { return description;} 
  uint GetStackable() { return stackable;} 

  void SetId(uint value) { this->id = value; }
  void SetName(csString value) { this->name = value; }
  void SetIconName(csString value) { this->iconname = value; }
  void SetMeshName(csString value) { this->meshname = value; }
  void SetFileName(csString value) { this->filename = value; }
  void SetDescription(csString value) { this->description = value; }
  void SetStackable(uint value) { this->stackable = value; }
};

#endif // CLIENT_ITEM_H
