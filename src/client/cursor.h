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

#ifndef CURSOR_H
#define CURSOR_H

#include <cssysdef.h>
#include <csutil/ref.h>
#include <csutil/weakref.h>
#include <cstool/collider.h>

#include "CEGUI.h"

namespace PT
{
  class Client;
} // PT namespace 

struct iCelEntity;
struct iCelPlLayer;
struct iCamera;
struct iPcTooltip;
struct iCollideSystem;

class Cursor
{
private:
  PT::Client* client;
  csWeakRef<iCelEntity> selent;
  int mousex;
  int mousey;
  CEGUI::Window* nametag;

public:
  Cursor(PT::Client* client);
  ~Cursor();

  void Draw();
  void MouseMove(iCelPlLayer* pl, iCamera* camera, int x, int y);
  iCelEntity* GetSelectedEntity();
  iMeshWrapper* Get3DPointFrom2D(iCamera* camera, csVector3* worldCoord, csVector3* untransfCoord, iSector** sector = 0);
  iMeshWrapper* Get3DPointFrom2D(int x, int y, iCamera* camera, csVector3* worldCoord, csVector3* untransfCoord, iSector** sector = 0);

  int GetMouseX(){ return mousex; }
  int GetMouseY(){ return mousey; }
};

#endif // __Selector_h
