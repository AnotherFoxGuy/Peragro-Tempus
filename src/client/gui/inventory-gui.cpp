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

InventoryWindow::InventoryWindow(GUIManager* guimanager)
: GUIWindow (guimanager)
{
}

InventoryWindow::~InventoryWindow()
{
}

bool InventoryWindow::handleCloseButton(const CEGUI::EventArgs& args)
{
  winMgr->getWindow("Inventory/Frame")->setVisible(false);
  return true;
}

bool InventoryWindow::AddItem(int itemid,int slotid)
{
  if(slotid > numberOfSlots) return false;

  Slot* slot = inventory[slotid];

  if (slot->GetObjectId() && !slot->GetStackable())
  {
    printf("InventoryWindow: ERROR Slot %d already occupied!\n", slotid);
    return false;
  }

  ClientItem* clientitem = itemmanager->GetItemById(itemid);

  // If the slot already has an item and it's stackable
  // increase the amount.
  if (slot->GetObjectId() && slot->GetStackable())
  {
    slot->SetAmount(slot->GetAmount()+1);
    dragdrop->UpdateItemCounter(slot->GetSlotWindow(), slot->GetAmount());
  }
  else
  {
    slot->SetObjectId(itemid);
    slot->SetAmount(1);
    slot->SetObjectWindow(dragdrop->createIcon(DragDrop::Item, itemid));
    slot->GetSlotWindow()->addChildWindow(slot->GetObjectWindow());
    // If stackable is bigger then 1 the item is stackable by that amount.
    // If stackable equals 0 its infinitly stackable.
    if (clientitem->GetStackable() > 1 || clientitem->GetStackable() == 0)
      slot->SetStackable(true);
  }

  return false;
}

bool InventoryWindow::AddItem(int itemid)
{
  ClientItem* clientitem = itemmanager->GetItemById(itemid);

  // Lets try and look for a slot with the same item.
  for (int i=0; i<numberOfSlots; i++)
  {
    Slot* slot = inventory[i+1];
    if((slot->GetObjectId() == itemid) 
      && slot->GetStackable()
      && ( (clientitem->GetStackable() > slot->GetAmount()) || (clientitem->GetStackable() == 0) ) )
    {
      slot->SetAmount(slot->GetAmount()+1);
      dragdrop->UpdateItemCounter(slot->GetSlotWindow(), slot->GetAmount());
      return true;
    }
  }
  // Look for an empty slot.
  for (int i=0; i<numberOfSlots; i++)
  {
    Slot* slot = inventory[i+1];
    if(slot->IsEmpty())
    {
      slot->SetObjectId(itemid);
      slot->SetAmount(1);
      slot->SetObjectWindow(dragdrop->createIcon(DragDrop::Item, itemid));
      slot->GetSlotWindow()->addChildWindow(slot->GetObjectWindow());
      // If stackable is bigger then 1 the item is stackable by that amount.
      // If stackable equals 0 its infinitly stackable.
      if (clientitem->GetStackable() > 1 || clientitem->GetStackable() == 0)
        slot->SetStackable(true);
      return true;
    }
  }

  printf("InventoryWindow: ERROR Inventory is full!\n");

  return false;
}

void InventoryWindow::CreateGUIWindow()
{
  numberOfSlots = 20;

  GUIWindow::CreateGUIWindow ("inventory.xml");
  winMgr = cegui->GetWindowManagerPtr ();

  dragdrop = guimanager->GetDragDrop();
  itemmanager = guimanager->GetClient ()->getItemmgr();

  //Load the inventory icon imageset
  vfs = guimanager->GetClient()->getVFS ();
  vfs->ChDir ("/peragro/skin/");
  cegui->GetImagesetManagerPtr()->createImageset("/peragro/skin/inventory.imageset", "Inventory");

  // Get the frame window
  CEGUI::FrameWindow* frame = static_cast<CEGUI::FrameWindow*>(winMgr->getWindow("Inventory/Frame"));
  frame->subscribeEvent(CEGUI::FrameWindow::EventCloseClicked, CEGUI::Event::Subscriber(&InventoryWindow::handleCloseButton, this));

  CEGUI::Window* root = winMgr->getWindow("Root");
  root->subscribeEvent(CEGUI::Window::EventDragDropItemDropped, CEGUI::Event::Subscriber(&DragDrop::handleDragDroppedRoot, dragdrop));

  // Populate the bag with slots.
  CEGUI::Window* bag = winMgr->getWindow("Inventory/Bag");
  for (int j=0; j<4; j++)
  {
    for (int i=0; i<5; i++)
    {
      Slot* slot = new Slot();
      slot->SetSlotId((i+1)+j*5);
      slot->SetSlotType(DragDrop::Item);
      slot->SetSlotWindow(dragdrop->createDragDropSlot(bag, CEGUI::UVector2(CEGUI::UDim(0,4.0f+(28*i)), CEGUI::UDim(0,4.0f+(28*j)))));
      slot->GetSlotWindow()->setUserData(slot);
      inventory.Put(slot->GetSlotId(), slot);
    }
  }

 // Get the root window
  rootwindow = winMgr->getWindow("Inventory/Frame");

}
