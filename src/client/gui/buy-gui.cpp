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

BuyWindow::BuyWindow(GUIManager* guimanager)
: GUIWindow (guimanager)
{
}

BuyWindow::~BuyWindow()
{
}

bool BuyWindow::OnCloseButton(const CEGUI::EventArgs& args)
{
  winMgr->getWindow("BuyWindow/Frame")->setVisible(false);

  dragdrop->ClearSlotsDelete(upperslots);
  dragdrop->ClearSlotsDelete(lowerslots);
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

  Update(page);

  printf("Scrolling: page %f\n", page);

  return true;
}

void BuyWindow::MoveItem(Slot* oldslot, Slot* newslot)
{
  int itemid = oldslot->GetObject()->GetId();

  if(oldslot->GetParent() == Slot::BuyUpper)
  {
    for (size_t i = 0; i < items.GetSize(); i++)
    {
      if(itemid == items.Get(i))
      {
        items.DeleteIndex(i);
        printf("BuyWindow:: Deleted index for itemid %d\n", itemid);
        break;
      }
    }
  }
  else if(oldslot->GetParent() == Slot::BuyLower)
  {
    items.Push(oldslot->GetObject()->GetId());
  }

  dragdrop->MoveObject(oldslot, newslot);
}

bool BuyWindow::AddItem(unsigned int itemid)
{
  items.Push(itemid);
  return true;
}

void BuyWindow::Update(int linenr)
{
  int nrInventorySlots = 12;
  linenr = linenr*nrInventorySlots;

  CEGUI::Scrollbar* scrollbar = static_cast<CEGUI::Scrollbar*>(winMgr->getWindow("BuyWindow/UpperSlots/UpperBag/scrollbar"));
  scrollbar->setDocumentSize(items.GetSize());

  // Clearing the old items.
  dragdrop->ClearSlotsDelete(upperslots);

  // Putting the items in.
  int counter = 0; 
  for (int i=linenr; i<items.GetSize(); i++)
  {
    if(counter > nrInventorySlots-1) break;
    Slot* slot = upperslots[counter];
    unsigned int itemid = items.Get(i);
    slot->SetObject(dragdrop->CreateItem(itemid));
    counter += 1;
  }
}

void BuyWindow::UpdateOffer()
{
  // Make a list of items and send it to the network.
  // Get actual items.
  //TradeOffersListPvpMessage msg;
  csArray<Object*> objects;
  for (size_t i=0; i<lowerslots.GetSize(); i++)
  {
    Slot* slot = lowerslots[i];
    if (!slot) continue;
    Object* object = slot->GetObject();
    if(object)
    {
      objects.Push(object);
    }
  }

  // Make the offer list.
  //msg.setOffersCount(objects.GetSize());
  printf("------------------------------------------\n");
  printf("BuyWindow: Creating Trade Offer List Pvp\n");
  for (size_t i=0; i<objects.GetSize(); i++)
  {
    Object* object = objects.Get(i);
    printf("item %d\n", object->GetId());
    //msg.setItemId(i, object->GetId());
  }

  //network->send(&msg);
  printf("SEND\n");

  printf("------------------------------------------\n");
}

void BuyWindow::AcceptTrade()
{
  winMgr->getWindow("BuyWindow/Frame")->setVisible(false);

  int nrInventorySlots = 30;

  // Putting the new items in the inventory.
  int counter = 10;
  for (int i=0; i<lowerslots.GetSize(); i++)
  {
    Slot* slot = lowerslots[i];
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

  dragdrop->ClearSlotsDelete(upperslots);
  dragdrop->ClearSlotsDelete(lowerslots);
  items.DeleteAll();
}

void BuyWindow::CreateGUIWindow()
{

  GUIWindow::CreateGUIWindow ("buy.xml");
  winMgr = cegui->GetWindowManagerPtr ();

  dragdrop = guimanager->GetDragDrop();

/*===============Delete====================*/
  //Load the inventory icon imageset
  vfs->ChDir ("/peragro/skin/");
  cegui->GetImagesetManagerPtr()->createImageset("/peragro/skin/inventory.imageset", "Inventory");
/*==========================================*/

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
  dragdrop->CreateBag(bag1, &upperslots, Slot::BuyUpper, DragDrop::Item, 3, 4);

  // Populate the lower bag with slots.
  CEGUI::Window* bag2 = winMgr->getWindow("BuyWindow/LowerSlots/LowerBag");
  dragdrop->CreateBag(bag2, &lowerslots, Slot::BuyLower, DragDrop::Item, 2, 4);

  AddItem(1);
  AddItem(2);
  AddItem(3);
  AddItem(4);
  AddItem(1);
  AddItem(1);
  AddItem(1);
  AddItem(2);
  AddItem(3);
  AddItem(4);
  AddItem(1);
  AddItem(1);
  AddItem(1);
  AddItem(4);

  Update(0);

}


