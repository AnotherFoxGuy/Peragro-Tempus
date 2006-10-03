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

#include "client/gui/gui.h"

#include "CEGUI.h"
#include "CEGUIWindowManager.h" 
#include "CEGUILogger.h"

#include "client/network/network.h"
#include "client/gui/guimanager.h"

/*=============//
// TradeWindow //
//=============*/
TradeWindow::TradeWindow(GUIManager* guimanager)
: GUIWindow (guimanager)
{
}

TradeWindow::~TradeWindow()
{
}

bool TradeWindow::OnCloseButton(const CEGUI::EventArgs& args)
{
  winMgr->getWindow("TradeWindow/Frame")->setVisible(false);

  // Putting the items back in the inventory.
  for (int i=0; i<numberOfSlots; i++)
  {
    Slot* slot = inventory1[i];
    if(!slot->IsEmpty())
    {
      Slot* oldslot = oldslots[i];
      guimanager->GetInventoryWindow()->MoveItem(slot, oldslot);
    }
  }

  inventory1.DeleteAll();
  oldslots.DeleteAll();
  
  // TODO: Send TradeCancelRequestMessage.

  return true;
}

bool TradeWindow::AddItem(uint player, uint itemid, uint amount, uint slotid)
{
  if(!(player == 2)) return false;
  if(slotid > numberOfSlots) return false;
  if(slotid == -1) return false;

  Slot* slot = inventory2[slotid];

  if (!slot)
  {
    printf("TradeWindow: ERROR Couldn't add item %d in slot %d!\n", itemid, slotid);
    return false;
  }

  if (!slot->IsEmpty())
  {
    printf("InventoryWindow: ERROR Slot %d already occupied!\n", slotid);
    return false;
  }

  // Create a new item.
  dragdrop->CreateItem(slot, itemid, amount);
  
  return true;
}

bool TradeWindow::AddItem(Slot* oldslot, Slot* newslot)
{
  if (!oldslot || !newslot)
  {
    printf("InventoryWindow: ERROR Couldn't move item from slot to slot!\n");
    return false;
  }

  dragdrop->MoveObject(oldslot, newslot);

  if(oldslot->GetParent() == Slot::Trade)
  {
    oldslots.Put(newslot->GetId(), oldslots[oldslot->GetId()]);
    oldslots.Put(oldslot->GetId(), 0);
  }
  else
    oldslots.Put(newslot->GetId(), oldslot);

  // TODO: Make a list of items and send it to the network.

  return true;
}

void TradeWindow::CreateGUIWindow()
{
  numberOfSlots = 16;

  GUIWindow::CreateGUIWindow ("trade-pc.xml");
  winMgr = cegui->GetWindowManagerPtr ();

  dragdrop = guimanager->GetDragDrop();

  // Get the root window
  rootwindow = winMgr->getWindow("TradeWindow/Frame");

  // Get the frame window
  CEGUI::FrameWindow* frame = static_cast<CEGUI::FrameWindow*>(winMgr->getWindow("TradeWindow/Frame"));
  frame->subscribeEvent(CEGUI::FrameWindow::EventCloseClicked, CEGUI::Event::Subscriber(&TradeWindow::OnCloseButton, this));

  // Populate the Player1 bag with slots.
  CEGUI::Window* bag1 = winMgr->getWindow("TradeWindow/Player1/Bag");
  for (int j=0; j<4; j++)
  {
    for (int i=0; i<4; i++)
    {
      Slot* slot = new Slot();
      slot->SetId((i+(j*4)));
      slot->SetType(DragDrop::Item);
      slot->SetParent(Slot::Trade);
      slot->SetWindow(dragdrop->createDragDropSlot(bag1, CEGUI::UVector2(CEGUI::UDim(0,4.0f+(28*i)), CEGUI::UDim(0,4.0f+(28*j)))));
      slot->GetWindow()->setUserData(slot);
      slot->GetWindow()->removeEvent(CEGUI::Window::EventDragDropItemDropped);
      slot->GetWindow()->subscribeEvent(CEGUI::Window::EventDragDropItemDropped, 
        CEGUI::Event::Subscriber(&DragDrop::handleDragDroppedTrade, dragdrop));
      inventory1.Put(slot->GetId(), slot);
    }
  }

  // Populate the Player2 bag with slots.
  // TODO: Need to make special dragdrop target which is not interactable.
  CEGUI::Window* bag2 = winMgr->getWindow("TradeWindow/Player2/Bag");
  for (int j=0; j<4; j++)
  {
    for (int i=0; i<4; i++)
    {
      Slot* slot = new Slot();
      slot->SetId((i+(j*4)));
      slot->SetType(DragDrop::Item);
      slot->SetWindow(dragdrop->createDragDropSlot(bag2, CEGUI::UVector2(CEGUI::UDim(0,4.0f+(28*i)), CEGUI::UDim(0,4.0f+(28*j)))));
      slot->GetWindow()->setUserData(slot);
      slot->GetWindow()->removeAllEvents();
      inventory2.Put(slot->GetId(), slot);
    }
  }

}


