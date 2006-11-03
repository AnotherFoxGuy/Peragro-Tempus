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

SellWindow::SellWindow(GUIManager* guimanager)
: GUIWindow (guimanager)
{
}

SellWindow::~SellWindow()
{
}

bool SellWindow::OnCloseButton(const CEGUI::EventArgs& args)
{

  return true;
}

bool SellWindow::OnAccept(const CEGUI::EventArgs& args)
{
  return true;
}

bool SellWindow::AddItem(unsigned int itemid, unsigned int slotid)
{
  if(slotid > 12) return false;
  if(slotid == -1) return false;

  Slot* slot = upperslots[slotid];

  if (!slot)
  {
    printf("SellWindow: ERROR Couldn't add item %d in slot %d!\n", itemid, slotid);
    return false;
  }

  slot->SetObject(dragdrop->CreateItem(itemid));

  return true;
}

void SellWindow::AcceptTrade()
{
  winMgr->getWindow("SellWindow/Frame")->setVisible(false);

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
}

void SellWindow::CreateGUIWindow()
{

  GUIWindow::CreateGUIWindow ("buy.xml");
  winMgr = cegui->GetWindowManagerPtr ();

  dragdrop = guimanager->GetDragDrop();

  // Get the root window
  rootwindow = winMgr->getWindow("SellWindow/Frame");

  // Get the frame window
  CEGUI::FrameWindow* frame = static_cast<CEGUI::FrameWindow*>(winMgr->getWindow("BuyWindow/Frame"));
  frame->subscribeEvent(CEGUI::FrameWindow::EventCloseClicked, CEGUI::Event::Subscriber(&SellWindow::OnCloseButton, this));

  // Get the frame window
  CEGUI::PushButton* accept1 = static_cast<CEGUI::PushButton*>(winMgr->getWindow("BuyWindow/Accept"));
  accept1->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&SellWindow::OnAccept, this));

  // Populate the Player1 bag with slots.
  CEGUI::Window* bag1 = winMgr->getWindow("SellWindow/UpperSlots/UpperBag");
  //dragdrop->CreateBag(bag1, &upperslots, Inventory::SellUpper, DragDrop::Item, 3, 4);

  // Populate the lower bag with slots.
  CEGUI::Window* bag2 = winMgr->getWindow("SellWindow/LowerSlots/LowerBag");
  //dragdrop->CreateBag(bag2, &lowerslots, Inventory::SellLower, DragDrop::Item, 2, 4);


}


