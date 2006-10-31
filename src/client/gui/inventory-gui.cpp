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
  iObjectRegistry* obj_reg = PointerLibrary::getInstance()->getObjectRegistry();
  vfs = CS_QUERY_REGISTRY(obj_reg, iVFS);
}

InventoryWindow::~InventoryWindow()
{
  inventory.DeleteAll();
}

bool InventoryWindow::handleCloseButton(const CEGUI::EventArgs& args)
{
  winMgr->getWindow("Inventory/Frame")->setVisible(false);
  return true;
}

bool InventoryWindow::AddItem(unsigned int itemid, unsigned int slotid)
{
  if(slotid > numberOfSlots) return false;
  if(slotid == -1) return false;

  Slot* slot = inventory[slotid];

  if (!slot)
  {
    printf("InventoryWindow: ERROR Couldn't add item %d in slot %d!\n", itemid, slotid);
    return false;
  }

  if (!slot->IsEmpty())
  {
    printf("InventoryWindow: ERROR Slot %d already occupied!\n", slotid);
    return false;
  }

  // Create a new item.
  if(slot->IsEmpty())
    slot->SetObject(dragdrop->CreateItem(itemid));

  return true;
}

bool InventoryWindow::MoveItem(unsigned int oldslotid, unsigned int newslotid)
{
  if(oldslotid == newslotid) return true;
  Slot* oldslot = inventory[oldslotid];
  Slot* newslot = inventory[newslotid];

  return MoveItem(oldslot, newslot);
}

bool InventoryWindow::MoveItem(Slot* oldslot, Slot* newslot)
{
  if (!oldslot || !newslot)
  {
    printf("InventoryWindow: ERROR Couldn't move item from slot to slot!\n");
    return false;
  }

  dragdrop->MoveObject(oldslot, newslot);

  return true;
}

bool InventoryWindow::RemoveItem(unsigned int slotid)
{
  if(slotid > numberOfSlots) return false;
  if(slotid == -1) return false;

  Slot* slot = inventory[slotid];

  Object* object = slot->GetObject();
  object->GetWindow()->destroy();
  delete object;
  slot->Clear();
  return true;
}

unsigned int InventoryWindow::FindItem(unsigned int itemid)
{
  for (size_t i = 0; i < inventory.GetSize(); i++)
  {
    Slot* slot = inventory.Get(i);
    
    if (!slot) 
      continue;
    
    if (!slot->GetObject())
      continue;

    if (slot->GetObject()->GetId() == itemid)
    {
      return slot->GetId();
    }
  }
  return ~0;
}

unsigned int InventoryWindow::FindFreeSlot()
{
  for (size_t i = 10; i < inventory.GetSize(); i++)
  {
    Slot* slot = inventory.Get(i);
    
    if (!slot) 
      continue;
    
    if (slot->IsEmpty())
      return slot->GetId();
  }
  printf("ERROR InventoryWindow: Inventory full!\n");
  return ~0;
}

void InventoryWindow::SetupEquipSlot(unsigned int id, const char* window)
{
  Slot* slot = new Slot();
  slot->SetId(id);
  slot->SetType(DragDrop::Item);
  CEGUI::Window* slotwin = winMgr->getWindow(window);
  slotwin->subscribeEvent(CEGUI::Window::EventDragDropItemEnters, CEGUI::Event::Subscriber(&DragDrop::handleDragEnter, dragdrop));
  slotwin->subscribeEvent(CEGUI::Window::EventDragDropItemLeaves, CEGUI::Event::Subscriber(&DragDrop::handleDragLeave, dragdrop));
  slotwin->subscribeEvent(CEGUI::Window::EventDragDropItemDropped, CEGUI::Event::Subscriber(&DragDrop::handleDragDropped, dragdrop));
  slot->SetWindow(slotwin);
  slot->GetWindow()->setUserData(slot);
  inventory.Put(slot->GetId(), slot);
}

void InventoryWindow::CreateGUIWindow()
{
  // First 10 slots(0-9) are equipment,
  // other 20 are inventory.
  numberOfSlots = 30;

  //Load the inventory icon imageset
  vfs->ChDir ("/peragro/skin/");
  cegui->GetImagesetManagerPtr()->createImageset("/peragro/skin/inventory.imageset", "Inventory");

  GUIWindow::CreateGUIWindow ("inventory.xml");
  winMgr = cegui->GetWindowManagerPtr ();

  dragdrop = guimanager->GetDragDrop();
  itemmanager = PointerLibrary::getInstance()->getItemManager();

  // Get the frame window
  CEGUI::FrameWindow* frame = static_cast<CEGUI::FrameWindow*>(winMgr->getWindow("Inventory/Frame"));
  frame->subscribeEvent(CEGUI::FrameWindow::EventCloseClicked, CEGUI::Event::Subscriber(&InventoryWindow::handleCloseButton, this));

  CEGUI::Window* root = winMgr->getWindow("Root");
  root->subscribeEvent(CEGUI::Window::EventDragDropItemDropped, CEGUI::Event::Subscriber(&DragDrop::handleDragDroppedRoot, dragdrop));

  // Populate the bag with slots.
  CEGUI::Window* bag = winMgr->getWindow("Inventory/Bag");
  dragdrop->CreateBag(bag, &inventory, Slot::Inventory, DragDrop::Item, 4, 5);

 // Get the root window
  rootwindow = winMgr->getWindow("Inventory/Frame");

  // Setup the equipslots.
  SetupEquipSlot(0, "Inventory/EquipFrame/WeaponRight");

}
