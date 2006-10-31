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

  return true;
}

bool BuyWindow::OnAccept(const CEGUI::EventArgs& args)
{
  return true;
}

bool BuyWindow::AddItem(unsigned int itemid, unsigned int slotid)
{
  if(slotid > 12) return false;
  if(slotid == -1) return false;

  Slot* slot = upperslots[slotid];

  if (!slot)
  {
    printf("TradeWindow: ERROR Couldn't add item %d in slot %d!\n", itemid, slotid);
    return false;
  }

  dragdrop->CreateItem(slot, itemid);
  
  return true;
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

  // Populate the Player1 bag with slots.
  CEGUI::Window* bag1 = winMgr->getWindow("BuyWindow/UpperSlots/UpperBag");
  for (int j=0; j<3; j++)
  {
    for (int i=0; i<4; i++)
    {
      Slot* slot = new Slot();
      slot->SetId((i+(j*4)));
      slot->SetType(DragDrop::Item);
      slot->SetParent(Slot::Buy);
      slot->SetWindow(dragdrop->createDragDropSlot(bag1, CEGUI::UVector2(CEGUI::UDim(0,4.0f+(28*i)), CEGUI::UDim(0,4.0f+(28*j)))));
      slot->GetWindow()->setUserData(slot);
      upperslots.Put(slot->GetId(), slot);
    }
  }

}


