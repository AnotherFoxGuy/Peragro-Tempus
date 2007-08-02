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

#include "client/reporter/reporter.h"

BuyWindow::BuyWindow(GUIManager* guimanager)
: GUIWindow (guimanager)
{
  totalmoney = 0;
}

BuyWindow::~BuyWindow()
{
  delete lowerslots;
  delete upperslots;
}

bool BuyWindow::OnCloseButton(const CEGUI::EventArgs& args)
{
  winMgr->getWindow("BuyWindow/Frame")->setVisible(false);

  upperslots->ClearSlotsDelete();
  lowerslots->ClearSlotsDelete();
  items.DeleteAll();

  //TradeCancelMessage msg;
  //network->send(&msg);

  return true;
}

bool BuyWindow::OnAccept(const CEGUI::EventArgs& args)
{
  UpdateOffer();
  return true;
}

bool BuyWindow::OnScroll(const CEGUI::EventArgs& args)
{
  using namespace CEGUI;

  const WindowEventArgs& ddea = static_cast<const WindowEventArgs&>(args);
  
  CEGUI::Scrollbar* scrollbar = static_cast<CEGUI::Scrollbar*>(ddea.window);
  float page = scrollbar->getScrollPosition();

  Update((int)page);

	Report(PT::Debug, "Scrolling: page %f", page);

  return true;
}

void BuyWindow::MoveItem(Slot* oldslot, Slot* newslot)
{
  int itemid = oldslot->GetObject()->GetId();

  // Item has been moved to be bought. Add to totalmoney.
  if(oldslot->GetParent() == Inventory::BuyUpper)
  {
    for (size_t i = 0; i < items.GetSize(); i++)
    {
      if(itemid == items.Get(i).itemid)
      {
				Report(PT::Debug, "BuyWindow:: Deleted index for itemid %d\n", itemid);
        totalmoney += items.Get(i).price;
        items.DeleteIndex(i);
        break;
      }
    }
  }
  // Item has been moved back, substract from totalmoney.
  else if(oldslot->GetParent() == Inventory::BuyLower)
  {
    Item item;
    item.itemid = oldslot->GetObject()->GetId();
    item.price = oldslot->GetObject()->GetPrice();
    items.Push(item);
    totalmoney -= item.price;
  }

  dragdrop->MoveObject(oldslot, newslot);
  SetTotalMoney(totalmoney);
}

void BuyWindow::SetTotalMoney(unsigned int price)
{
  btn = winMgr->getWindow("BuyWindow/Money/TotalMoney");
  btn->setText(dragdrop->IntToStr(price));
}

void BuyWindow::SetYourMoney(unsigned int price)
{
  btn = winMgr->getWindow("BuyWindow/Money/YourMoney");
  btn->setText(dragdrop->IntToStr(price));
}

bool BuyWindow::AddItem(unsigned int itemid, unsigned int price)
{
  winMgr->getWindow("BuyWindow/Frame")->setVisible(true);
  Item item;
  item.itemid = itemid;
  item.price = price;
  items.Push(item);
  Update(0);
  return true;
}

void BuyWindow::Update(int linenr)
{
  int nrInventorySlots = 12;
  linenr = linenr*nrInventorySlots;

  CEGUI::Scrollbar* scrollbar = static_cast<CEGUI::Scrollbar*>(winMgr->getWindow("BuyWindow/UpperSlots/UpperBag/scrollbar"));
  scrollbar->setDocumentSize(items.GetSize());

  // Clearing the old items.
  upperslots->ClearSlotsDelete();

  // Putting the items in.
  int counter = 0; 
  for (size_t i=linenr; i<items.GetSize(); i++)
  {
    if(counter > nrInventorySlots-1) break;
    Slot* slot = upperslots->GetSlot(counter);
    Item item = items.Get(i);
    slot->SetObject(dragdrop->CreateItem(item.itemid));
    slot->GetObject()->SetPrice(item.price);
    counter += 1;
  }
}

void BuyWindow::UpdateOffer()
{
  // Make a list of items and send it to the network.
  // Get actual items.
  TradeOrderListNpcMessage msg;
  csArray<Inventory::ObjectAndSlot> objandslot = lowerslots->GetAllObjects();

  msg.setIsBuy(1);

  // Make the offer list.
  msg.setOrdersCount(objandslot.GetSize());
	Report(PT::Debug, "------------------------------------------");
  Report(PT::Debug, "BuyWindow: Creating Trade Offer List Pvp");
  for (size_t i=0; i<objandslot.GetSize(); i++)
  {
    Inventory::ObjectAndSlot objslot = objandslot.Get(i);
    Report(PT::Debug, "item %d", objslot.object->GetId());
    msg.setItemId(i, objslot.object->GetId());
  }

  network->send(&msg);
  Report(PT::Debug, "SEND");

  Report(PT::Debug, "------------------------------------------");
}

void BuyWindow::AcceptTrade()
{
  winMgr->getWindow("BuyWindow/Frame")->setVisible(false);

  int nrInventorySlots = 30;

  // Get all the items.
  csArray<Inventory::ObjectAndSlot> objandslot = lowerslots->GetAllObjects();

  // Putting the new items in the inventory.
  int counter = 10;
  for (size_t i=0; i<objandslot.GetSize(); i++)
  {
    unsigned int objid = objandslot.Get(i).object->GetId();
    while(!guimanager->GetInventoryWindow()->AddItem(objid, counter)
      && counter < nrInventorySlots)
    {
      counter += 1;
    }
  }

  upperslots->ClearSlotsDelete();
  lowerslots->ClearSlotsDelete();
  items.DeleteAll();
}

void BuyWindow::CreateGUIWindow()
{

  GUIWindow::CreateGUIWindow ("buy.xml");
  winMgr = cegui->GetWindowManagerPtr ();

  dragdrop = guimanager->GetDragDrop();

  // Get the root window
  rootwindow = winMgr->getWindow("BuyWindow/Frame");

  // Get the frame window
  CEGUI::FrameWindow* frame = static_cast<CEGUI::FrameWindow*>(winMgr->getWindow("BuyWindow/Frame"));
  frame->subscribeEvent(CEGUI::FrameWindow::EventCloseClicked, CEGUI::Event::Subscriber(&BuyWindow::OnCloseButton, this));

  // Get the frame window
  CEGUI::PushButton* accept1 = static_cast<CEGUI::PushButton*>(winMgr->getWindow("BuyWindow/Accept"));
  accept1->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&BuyWindow::OnAccept, this));

    // Get the frame window
  CEGUI::Scrollbar* scrollbar = static_cast<CEGUI::Scrollbar*>(winMgr->getWindow("BuyWindow/UpperSlots/UpperBag/scrollbar"));
  scrollbar->subscribeEvent(CEGUI::Scrollbar::EventScrollPositionChanged, CEGUI::Event::Subscriber(&BuyWindow::OnScroll, this));
  scrollbar->setStepSize(1.0);
  scrollbar->setOverlapSize(0.0);
  scrollbar->setPageSize(12);

  // Populate the upper bag with slots.
  CEGUI::Window* bag1 = winMgr->getWindow("BuyWindow/UpperSlots/UpperBag");
  upperslots = new Inventory(guimanager);
  upperslots->Create(bag1, Inventory::BuyUpper, DragDrop::Item, 3, 4);

  // Populate the lower bag with slots.
  CEGUI::Window* bag2 = winMgr->getWindow("BuyWindow/LowerSlots/LowerBag");
  lowerslots = new Inventory(guimanager);
  lowerslots->Create(bag2, Inventory::BuyLower, DragDrop::Item, 2, 4);

  Update(0);

}


