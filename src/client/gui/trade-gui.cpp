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

TradeWindow::TradeWindow(GUIManager* guimanager)
: GUIWindow (guimanager)
{
  accept1 = false;
  accept2 = false;
}

TradeWindow::~TradeWindow()
{
  trade1.DeleteAll();
  trade2.DeleteAll();
  inventory.DeleteAll();
}

bool TradeWindow::OnYesRequest(const CEGUI::EventArgs& args)
{
  winMgr->getWindow("TradeWindow/Frame")->setVisible(true);

  TradeResponseMessage msg;
  network->send(&msg);

  return true;
}

bool TradeWindow::OnNoRequest(const CEGUI::EventArgs& args)
{
  TradeResponseMessage msg;
  msg.setError(ptString("The other player declined the trade.", strlen("The other player declined the trade.")));
  network->send(&msg);

  return true;
}

bool TradeWindow::OnYesConfirm(const CEGUI::EventArgs& args)
{
  TradeConfirmRequestMessage msg;
  network->send(&msg);

  return true;
}

bool TradeWindow::OnNoConfirm(const CEGUI::EventArgs& args)
{
  TradeCancelMessage msg;
  network->send(&msg);

  return true;
}

bool TradeWindow::OnCloseButton(const CEGUI::EventArgs& args)
{
  TradeWindow::CancelTrade();
  
  TradeCancelMessage msg;
  network->send(&msg);

  return true;
}

bool TradeWindow::OnAcceptPlayer1(const CEGUI::EventArgs& args)
{
  SetAccept(1, true);
  TradeOfferAcceptMessage msg;
  network->send(&msg);
  return true;
}

void TradeWindow::ClearItems()
{
  dragdrop->ClearSlotsDelete(trade2);
}

bool TradeWindow::AddItem(unsigned int player, unsigned int itemid, unsigned int slotid)
{
  if(!(player == 2)) return false;
  if(slotid > numberOfSlots) return false;
  if(slotid == -1) return false;

  Slot* slot = trade2[slotid];

  if (!slot)
  {
    printf("TradeWindow: ERROR Couldn't add item %d in slot %d!\n", itemid, slotid);
    return false;
  }

  // Create a new non-interactable item.
  slot->SetObject(dragdrop->CreateItem(itemid, false));

  printf("TradeWindow: Creating item %d!\n", itemid);
  
  return true;
}

void TradeWindow::SetAccept(unsigned int player, bool value)
{
  if(player == 1)
  {
    btn = winMgr->getWindow("TradeWindow/Player1/Accept");
    value ? btn->disable() : btn->enable();
    accept1 = value;
  }
  else if(player == 2)
  {
    btn = winMgr->getWindow("TradeWindow/Player2/Accept");
    value ? btn->disable() : btn->enable();
    accept2 = value;
  }

  // Check if both are in accept state.
  if(accept1 && accept2)
  {
    ConfirmDialogWindow* dialog = guimanager->CreateConfirmWindow();
    dialog->SetText("You sure you want to trade?");
    dialog->SetYesEvent(CEGUI::Event::Subscriber(&TradeWindow::OnYesConfirm, this));
    dialog->SetNoEvent(CEGUI::Event::Subscriber(&TradeWindow::OnNoConfirm, this)); 
  }
}

bool TradeWindow::AddItem(Slot* oldslot, Slot* newslot)
{
  if (!oldslot || !newslot)
  {
    printf("TradeWindow: ERROR Couldn't move item from slot to slot!\n");
    return false;
  }

  // If we drag the icon around to the same 
  // slot in the trade window, return.
  if((oldslot->GetParent() == Slot::TradeLeft)
    && oldslot->GetId() == newslot->GetId())
    return true;

  if(oldslot->GetParent() == Slot::TradeLeft)
  {
    dragdrop->MoveObject(oldslot, newslot);
    inventory.Put(newslot->GetId(), inventory[oldslot->GetId()]);
    inventory.Put(oldslot->GetId(), 0);
  }
  else
  {
    inventory.Put(newslot->GetId(), oldslot);
    // Disable the icon in the inventory.
    Object* object = oldslot->GetObject();
    object->GetWindow()->disable();
    // Create a new item in the trade inventory.
    newslot->SetObject(dragdrop->CreateItem(object->GetId()));
  }

  // Send an updated state of the trade inventory.
  TradeWindow::UpdateOffer();

  return true;
}

void TradeWindow::UpdateOffer()
{
  // Make a list of items and send it to the network.
  // Get actual items.
  TradeOffersListPvpMessage msg;
  csArray<Object*> objects;
  csArray<unsigned int> slotids;
  for (size_t i=0; i<trade1.GetSize(); i++)
  {
    Slot* slot = trade1[i];
    if (!slot) continue;
    Object* object = slot->GetObject();
    if(object)
    {
      objects.Push(object);
      slotids.Push(slot->GetId());
    }
  }

  // Make the offer list.
  msg.setOffersCount(objects.GetSize());
  printf("------------------------------------------\n");
  printf("TradeWindow: Creating Offer List Pvp for %d items\n", objects.GetSize());
  for (size_t i=0; i<objects.GetSize(); i++)
  {
    Object* object = objects.Get(i);
    unsigned int slotid = slotids.Get(i);
    printf("item %d in slot %d!\n", object->GetId(), slotid);
    msg.setItemId(i, object->GetId());
    msg.setSlotId(i, slotid);
  }

  network->send(&msg);
  printf("SEND\n");

  printf("------------------------------------------\n");
}

Slot* TradeWindow::GetOldSlot(Slot* slot)
{
  if (!slot)
  {
    printf("TradeWindow: ERROR Couldn't get old slot, invalid slot!\n");
    return 0;
  }

  Slot* oldslot = inventory[slot->GetId()];

  if (!oldslot)
  {
    printf("TradeWindow: ERROR Couldn't get old slot!\n");
    return 0;
  }

  return oldslot;
}

void TradeWindow::CancelTrade()
{
  winMgr->getWindow("TradeWindow/Frame")->setVisible(false);

  // Putting the items back in the inventory.
  for (int i=0; i<numberOfSlots; i++)
  {
    Slot* slot = trade1[i];
    if(!slot->IsEmpty())
    {
      Slot* oldslot = inventory[i];
      oldslot->GetObject()->GetWindow()->enable();
    }
  }

  dragdrop->ClearSlotsDelete(trade1);
  dragdrop->ClearSlotsDelete(trade2);
  inventory.DeleteAll();
  SetAccept(1, false);
  SetAccept(2, false);
  
}

void TradeWindow::AcceptTrade()
{
  winMgr->getWindow("TradeWindow/Frame")->setVisible(false);

  int nrInventorySlots = 30;

  // Putting the new items in the inventory.
  int counter = 10;
  for (int i=0; i<numberOfSlots; i++)
  {
    Slot* slot = trade2[i];
    if(!slot->IsEmpty())
    {
      Object* object = slot->GetObject();
      while(!guimanager->GetInventoryWindow()->AddItem(object->GetId(), counter)
        && counter < nrInventorySlots)
      {
        counter += 1;
      }
    }
  }

  // Deleting the traded items in the inventory.
  for (int i=0; i<numberOfSlots; i++)
  {
    Slot* slot = trade1[i];
    if(!slot->IsEmpty())
    {
      Slot* oldslot = inventory[i];
      guimanager->GetInventoryWindow()->RemoveItem(oldslot->GetId());
    }
  }

  dragdrop->ClearSlotsDelete(trade1);
  dragdrop->ClearSlotsDelete(trade2);
  inventory.DeleteAll();
  SetAccept(1, false);
  SetAccept(2, false);
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

  // Get the frame window
  CEGUI::PushButton* accept1 = static_cast<CEGUI::PushButton*>(winMgr->getWindow("TradeWindow/Player1/Accept"));
  accept1->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TradeWindow::OnAcceptPlayer1, this));

  //btn = winMgr->getWindow("TradeWindow/Player2/Accept");
  //btn->disable();

  // Populate the Player1 bag with slots.
  CEGUI::Window* bag1 = winMgr->getWindow("TradeWindow/Player1/Bag");
  dragdrop->CreateBag(bag1, &trade1, Slot::TradeLeft, DragDrop::Item, 4, 4);

  // Populate the Player2 bag with slots.
  CEGUI::Window* bag2 = winMgr->getWindow("TradeWindow/Player2/Bag");
  dragdrop->CreateBag(bag2, &trade2, Slot::TradeRight, DragDrop::Item, 4, 4);

}


