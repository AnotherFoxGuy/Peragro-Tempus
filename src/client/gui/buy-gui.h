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

#ifndef BUY_GUI_H
#define BUY_GUI_H

#include "base-gui.h"

#include "client/gui/common/inventory.h"

class BuyWindow : public GUIWindow
{
private:
  DragDrop* dragdrop;
  ItemMGR* itemmanager;

  Inventory* upperslots;
  Inventory* lowerslots;

  csArray<unsigned int> items;

private:
  bool OnCloseButton(const CEGUI::EventArgs& args); 
  bool OnAccept(const CEGUI::EventArgs& args);
  bool OnScroll(const CEGUI::EventArgs& args);
  void Update(int linenr);
  void UpdateOffer();

public:
  BuyWindow(GUIManager* guimanager);
  virtual ~BuyWindow();
  void CreateGUIWindow();    // load the chat guilayout and register button events.

public:
  bool AddItem(unsigned int itemid); 
  void CancelTrade();
  void AcceptTrade();

  void MoveItem(Slot* oldslot, Slot* newslot);

};


#endif // BUY_GUI_H
