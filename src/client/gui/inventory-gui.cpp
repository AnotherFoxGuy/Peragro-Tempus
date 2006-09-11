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

bool InventoryWindow::AddItem(int itemid, bool stackable)
{
  CEGUI::Window* inventoryframe = winMgr->getWindow("Inventory/Bag");

  CEGUI::Window* freeslot = 0;
  unsigned int i = 0;
  bool sameid = false;

  while ((freeslot == 0) && (i < inventoryframe->getChildCount()))
  {
    CEGUI::Window* slot = inventoryframe->getChildAtIdx(i);
    i += 1;
    //printf("InventoryWindow: Checking slot %d \n", i);

    char itemtypestr[1024];
    sprintf(itemtypestr, "%d", itemid);
    // Check if the slot is occupied by a item with the same id.
    if (slot->isUserStringDefined("itemtype")) 
      if (slot->getUserString("itemtype") == itemtypestr) 
        sameid = true;

      if ((slot->getChildCount() < 2 ) || sameid)
      {
        //printf("slot %s is empty: Item added to slot\n", slot->getName().c_str());
        freeslot = slot;

        freeslot->addChildWindow(dragdrop->createIcon(DragDrop::Item, itemid, stackable));
        freeslot->setUserString("itemtype" , itemtypestr);

        dragdrop->UpdateItemCounter(slot);

        return true;
      }
  }
  printf("InventoryWindow: Inventory is full!\n");
  return false;
}

void InventoryWindow::CreateGUIWindow()
{
  GUIWindow::CreateGUIWindow ("inventory.xml");
  winMgr = cegui->GetWindowManagerPtr ();

  dragdrop = guimanager->GetDragDrop();

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
      dragdrop->createDragDropSlot(bag, CEGUI::UVector2(CEGUI::UDim(0,4.0f+(28*i)), CEGUI::UDim(0,4.0f+(28*j))));
    }
  }

 // Get the root window
  rootwindow = winMgr->getWindow("Inventory/Frame");

}
