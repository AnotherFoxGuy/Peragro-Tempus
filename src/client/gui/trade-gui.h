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

class TradeWindow : public GUIWindow
{
private:
  DragDrop* dragdrop;
  ItemMGR* itemmanager;
  int numberOfSlots;
  bool accept1;
  bool accept2;
  csArray<Slot*> trade1;
  csArray<Slot*> trade2;
  csArray<Slot*> inventory;
  void Clear(csArray<Slot*> arr);

private:
  bool OnCloseButton(const CEGUI::EventArgs& args); 
  bool OnAcceptPlayer1(const CEGUI::EventArgs& args);
  bool OnAcceptPlayer2(const CEGUI::EventArgs& args);

public:
  TradeWindow(GUIManager* guimanager);
  virtual ~TradeWindow();
  void CreateGUIWindow();    // load the chat guilayout and register button events.
  Slot* GetOldSlot(Slot* slot);
  void UpdateOffer();

public:
  bool OnYesRequest(const CEGUI::EventArgs& args);
  bool OnNoRequest(const CEGUI::EventArgs& args);
  bool OnYesConfirm(const CEGUI::EventArgs& args);
  bool OnNoConfirm(const CEGUI::EventArgs& args);

public:
  void SetName(unsigned int player, csString name);
  bool AddItem(unsigned int player, unsigned int itemid, unsigned int amount, unsigned int slotid);
  bool AddItem(Slot* oldslot, Slot* newslot);
  void SetMoney(unsigned int player, unsigned int amount); 
  void SetAccept(unsigned int player, bool value);
  void SetNotificationMsg(csString errormsg);
  void CancelTrade();
  void AcceptTrade();
  void ClearItems();

};


#endif // TRADE_GUI_H
