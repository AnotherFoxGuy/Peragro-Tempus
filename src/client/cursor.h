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

struct iCelEntity;
struct iCelPlLayer;
struct iCamera;
struct iPcTooltip;
class Client;

class Cursor
{
private:
  Client* client;
  csWeakRef<iCelEntity> selent;
  csWeakRef<iCelEntity> prevselent;
  int mousex;
  int mousey;
  csRef<iPcTooltip> nametag;

public:
  Cursor(Client* client);
  ~Cursor();

  void Draw();
  void MouseMove(iCelPlLayer* pl, iCamera* camera, int x, int y);
  iCelEntity* getSelectedEntity();
};

#endif // __Selector_h
