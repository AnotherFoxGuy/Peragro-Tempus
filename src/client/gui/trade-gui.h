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

#ifndef TRADE_GUI_H
#define TRADE_GUI_H

#include "base-gui.h"

#include "client/gui/common/dragdrop-gui.h"
#include "client/gui/common/slot.h"

/*================//
// GUIInvWindow  //
//================*/
class TradeWindow : public GUIWindow
{
private:
  DragDrop* dragdrop;
  ItemMGR* itemmanager;
  int numberOfSlots;

  csArray<Slot*> inventory1;
  csArray<Slot*> inventory2;

private:
  bool OnCloseButton(const CEGUI::EventArgs& args); 
  bool OnAcceptPlayer1(const CEGUI::EventArgs& args);
  bool OnAcceptPlayer2(const CEGUI::EventArgs& args);

public:
  TradeWindow(GUIManager* guimanager);
  virtual ~TradeWindow();
  void CreateGUIWindow();    // load the chat guilayout and register button events.

public:
  void SetName(uint player, csString name);
  void AddItem(uint player, uint item, uint slot);
  void RemoveItem(uint player, uint slot);
  void SetMoney(uint player, uint amount); 
  void SetAccept(uint player, bool value);
  void SetNotificationMsg(csString errormsg);

};


#endif // TRADE_GUI_H
