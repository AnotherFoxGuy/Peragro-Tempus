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

#ifndef INV_GUI_H
#define INV_GUI_H

#include "base-gui.h"

#include "client/gui/common/dragdrop-gui.h"

class InventoryWindow : public GUIWindow
{
private:
  bool handleCloseButton(const CEGUI::EventArgs& args);
  DragDrop* dragdrop;

public:
  InventoryWindow(GUIManager* guimanager);
  virtual ~InventoryWindow();
  void CreateGUIWindow();    // load the chat guilayout and register button events.
  bool AddItem(int itemid, bool stackable);
};


#endif // INV_GUI_H
