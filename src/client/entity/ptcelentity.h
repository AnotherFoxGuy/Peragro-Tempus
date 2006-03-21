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

#include "iengine/mesh.h"

#include "physicallayer/entity.h"
#include "propclass/linmove.h"


class ptCelEntity 
{
private:
  int id;
  int type;
  csString name;
  iCelEntity* celentity;
  //iPcLinearMovement* pclinmove;
  //Stats* stats;
  //EquipedItems* eqitems;

public:

  ptCelEntity (int id, int type, csString name, iCelEntity* celentity);
  ~ptCelEntity();

  int GetId () const { return id; }

  csString GetName () const { return name; }

  iCelEntity* GetCelEntity () const { return celentity; }

  //iPcLinearMovement* GetPcLinMove () const { return pclinmove; }
};

#endif // PTCELENTITY_H
