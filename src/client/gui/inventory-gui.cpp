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

/*================//
// GUIInvWindow   //
//================*/
InventoryWindow::InventoryWindow(GUIManager* guimanager)
: GUIWindow (guimanager)
{
}

InventoryWindow::~InventoryWindow()
{
}

bool InventoryWindow::handleDragEnter(const CEGUI::EventArgs& args)
{
  using namespace CEGUI;

  const DragDropEventArgs& ddea = static_cast<const DragDropEventArgs&>(args);
  ddea.window->setProperty("FrameColours", "tl:FF00FF00 tr:FF00FF00 bl:FF00FF00 br:FF00FF00");

  return true;
}
bool InventoryWindow::handleDragLeave(const CEGUI::EventArgs& args)
{
  using namespace CEGUI;

  const DragDropEventArgs& ddea = static_cast<const DragDropEventArgs&>(args);
  ddea.window->setProperty("FrameColours", "tl:FFFFFFFF tr:FFFFFFFF bl:FFFFFFFF br:FFFFFFFF");
  CEGUI::Window* itemcounter = ddea.window->getChild(30);
  int nrofitems = itemcounter->getParent()->getChildCount()-2;
  char buffer[1024];
  sprintf(buffer, "%d", nrofitems);
  itemcounter->setText((CEGUI::String)buffer);
  if (nrofitems < 2) itemcounter->setVisible(false); else itemcounter->setVisible(true);

  return true;
}
bool InventoryWindow::handleDragDropped(const CEGUI::EventArgs& args)
{
  using namespace CEGUI;

  const DragDropEventArgs& ddea = static_cast<const DragDropEventArgs&>(args);

  ddea.window->setProperty("FrameColours", "tl:FFFFFFFF tr:FFFFFFFF bl:FFFFFFFF br:FFFFFFFF");
  ddea.window->addChildWindow(ddea.dragDropItem);
  ddea.window->setUserString("itemtype" , ddea.dragDropItem->getUserString("itemtype"));
  UpdateItemCounter(ddea.window);
  return true;

}
bool InventoryWindow::handleDragDroppedRoot(const CEGUI::EventArgs& args)
{
  using namespace CEGUI;

  const DragDropEventArgs& ddea = static_cast<const DragDropEventArgs&>(args);
  int itemid = -1;

  Window* slot = ddea.dragDropItem->getParent();

  if (ddea.dragDropItem->isUserStringDefined("itemtype")) 
  { 
  itemid = atoi(ddea.dragDropItem->getUserString("itemtype").c_str()); 
  printf("About to drop item of type: %d!\n", itemid);
  DropEntityRequestMessage msg;
  msg.setTargetId(itemid);
  network->send(&msg);
  ddea.dragDropItem->destroy();
  printf("Item icon destroyed!\n");
  UpdateItemCounter(slot);
  }
  else 
  {
    //slot->addChildWindow(ddea.dragDropItem);
    printf("Couldn't determine itemID, putting it back!");
  }
  printf("Dropped item to the world!\n");

  return true;
}
bool InventoryWindow::handleDragDroppedStackable(const CEGUI::EventArgs& args)
{
  using namespace CEGUI;

  const DragDropEventArgs& ddea = static_cast<const DragDropEventArgs&>(args);

  bool stackable = false;
  // Check if the slot is occupied by a item with the same id.
  if (ddea.dragDropItem->isUserStringDefined("stackable")) {if(ddea.dragDropItem->getUserString("stackable") == "true") stackable = true;}

  if (stackable)
  {
    if (ddea.window->isUserStringDefined("itemtype")) 
    {
      const char* slottype = ddea.window->getUserString("itemtype").c_str();
      const char* itemtype = ddea.dragDropItem->getUserString("itemtype").c_str();
      //printf("Slottype is %s and itemtype is %s \n", slottype, itemtype);

      if ( !strcmp(slottype, itemtype) ) 
      {
        ddea.window->getParent()->addChildWindow(ddea.dragDropItem);
        UpdateItemCounter(ddea.window->getParent());
      }
    }
  }

  return true;
}
bool InventoryWindow::handleCloseButton(const CEGUI::EventArgs& args)
{
  winMgr->getWindow("Inventory/Frame")->setVisible(false);
  winMgr->getWindow("Chatlog/Frame")->activate();
  return true;
}
CEGUI::Window* InventoryWindow::createDragDropSlot(CEGUI::Window* parent, const CEGUI::Point& position)
{
  // Create the slot
  CEGUI::Window* slot = winMgr->createWindow("TaharezLook/StaticImage");
  parent->addChildWindow(slot);
  //slot->setWindowPosition(position);
  slot->setPosition(CEGUI::Absolute, position);
  slot->setSize(CEGUI::Absolute, CEGUI::Size(35.0f, 35.0f));
  //slot->setWindowSize(CEGUI::UVector2(CEGUI::cegui_reldim(0.2f), CEGUI::cegui_reldim(0.2f)));
  slot->subscribeEvent(CEGUI::Window::EventDragDropItemEnters, CEGUI::Event::Subscriber(&InventoryWindow::handleDragEnter, this));
  slot->subscribeEvent(CEGUI::Window::EventDragDropItemLeaves, CEGUI::Event::Subscriber(&InventoryWindow::handleDragLeave, this));
  slot->subscribeEvent(CEGUI::Window::EventDragDropItemDropped, CEGUI::Event::Subscriber(&InventoryWindow::handleDragDropped, this));

  // Create the itemcounter
  CEGUI::Window* itemcounter = winMgr->createWindow("TaharezLook/Editbox");
  slot->addChildWindow(itemcounter);
  itemcounter->setWindowPosition(CEGUI::UVector2(CEGUI::cegui_reldim(0.5f), CEGUI::cegui_reldim( 0.5f)));
  itemcounter->setWindowSize(CEGUI::UVector2(CEGUI::cegui_reldim(0.65f), CEGUI::cegui_reldim(0.65f)));
  itemcounter->setVisible(false);
  itemcounter->disable();
  itemcounter->setText("0");
  itemcounter->setAlwaysOnTop(true);
  itemcounter->setAlpha(0.5);
  itemcounter->setID(30);
  itemcounter->setProperty("Font", "Commonwealth-8");

  return slot;
}
CEGUI::Window* InventoryWindow::createItemIcon(CEGUI::String itemname, int itemtype, bool stackable)
{
  char uniquename[1024];
  counter += 1;
  sprintf(uniquename, "%d_%d_icon", itemtype, counter);

  // create a drag/drop item
  CEGUI::DragContainer* item = static_cast<CEGUI::DragContainer*>(
    winMgr->createWindow("DragContainer", uniquename));
  item->setWindowPosition(CEGUI::UVector2(CEGUI::cegui_reldim(0.05f), CEGUI::cegui_reldim(0.05f)));
  item->setWindowSize(CEGUI::UVector2(CEGUI::cegui_reldim(0.9f), CEGUI::cegui_reldim(0.9f)));
  item->setTooltipText(itemname);
  // Set the itemID.
  char itemtypestr[1024];
  sprintf(itemtypestr, "%d", itemtype);
  item->setUserString("itemtype" , itemtypestr);
  // Set wether or not the item is stackable
  if (stackable)
  item->setUserString("stackable" , "true");
  else item->setUserString("stackable" , "false");

  // set a static image as drag container's contents
  CEGUI::Window* itemIcon = winMgr->createWindow("TaharezLook/StaticImage");
  item->addChildWindow(itemIcon);
  itemIcon->setWindowPosition(CEGUI::UVector2(CEGUI::cegui_reldim(0), CEGUI::cegui_reldim(0)));
  itemIcon->setWindowSize(CEGUI::UVector2(CEGUI::cegui_reldim(1), CEGUI::cegui_reldim(1)));
  //itemIcon->setProperty("Image", "set:TaharezLook image:CloseButtonNormal");
  char inventoryimage[1024];
  sprintf(inventoryimage, "set:Inventory image:%d", itemtype);
  itemIcon->setProperty("Image", inventoryimage);
  // disable to allow inputs to pass through.
  itemIcon->disable();

  //if (stackable)
  item->subscribeEvent(CEGUI::Window::EventDragDropItemDropped, CEGUI::Event::Subscriber(&InventoryWindow::handleDragDroppedStackable, this));

  return item;
}
void InventoryWindow::UpdateItemCounter(CEGUI::Window* parent)
{
  CEGUI::Window* itemcounter = parent->getChild(30);
  int nrofitems = itemcounter->getParent()->getChildCount()-1;
  char buffer[1024];
  sprintf(buffer, "%d", nrofitems);
  itemcounter->setText((CEGUI::String)buffer);
  if (nrofitems < 2) itemcounter->setVisible(false); else itemcounter->setVisible(true);
}
bool InventoryWindow::AddItem(CEGUI::String itemname, int itemtype, bool stackable)
{
  CEGUI::Window* inventoryframe = winMgr->getWindow("Inventory/Bag");

  CEGUI::Window* freeslot = 0;
  unsigned int i = 0;

  while ((freeslot == 0) && (i < inventoryframe->getChildCount()))
  {
    CEGUI::Window* slot = inventoryframe->getChildAtIdx(i);
    i += 1;
    //printf("Checking slot %d \n", i);

    char itemtypestr[1024];
    sprintf(itemtypestr, "%d", itemtype);
    bool sameid = false;
    // Check if the slot is occupied by a item with the same id.
    if (slot->isUserStringDefined("itemtype")) 
      if (slot->getUserString("itemtype") == itemtypestr) 
      {
        sameid = true;
      }

      if ((slot->getChildCount() < 2 ) || sameid)
      {
        //printf("slot %s is empty: Item added to slot\n", slot->getName().c_str());
        freeslot = slot;

        freeslot->addChildWindow(createItemIcon(itemname, itemtype, stackable));
        freeslot->setUserString("itemtype" , itemtypestr);

        UpdateItemCounter(slot);

        return true;
      }
  }
  printf("Inventory is full!\n");
  return false;
}

void InventoryWindow::AddSkil(const char* skillname, unsigned int skillvalue)
{
  btn = winMgr->getWindow("Inventory/StatsFrame");
  char charskillvalue[10];
  sprintf(charskillvalue, "%d", skillvalue);
  CEGUI::ListboxItem* skillvalueItem = new CEGUI::ListboxTextItem(charskillvalue);
  CEGUI::ListboxItem* skillnameItem = new CEGUI::ListboxTextItem(skillname);

  //skillvalueItem->setSelectionBrushImage((CEGUI::utf8*)"TaharezLook", (CEGUI::utf8*)"TextSelectionBrush");
  //skillnameItem->setSelectionBrushImage((CEGUI::utf8*)"TaharezLook", (CEGUI::utf8*)"TextSelectionBrush");

  unsigned int row = ((CEGUI::MultiColumnList*)btn)->addRow();
  ((CEGUI::MultiColumnList*)btn)->setItem(skillnameItem, 0, row);
  ((CEGUI::MultiColumnList*)btn)->setItem(skillvalueItem, 1, row);
  //((CEGUI::MultiColumnList*)btn)->setSelectionMode(CEGUI::MultiColumnList::RowSingle);
}
void InventoryWindow::CreateGUIWindow()
{
  GUIWindow::CreateGUIWindow ("inventory.xml");
  winMgr = cegui->GetWindowManagerPtr ();

  //Load the inventory icon imageset
  vfs = guimanager->GetClient()->getVFS ();
  vfs->ChDir ("/peragro/skin/");
  cegui->GetImagesetManagerPtr()->createImageset("/peragro/skin/inventory.imageset", "Inventory");

  // Get the frame window
  CEGUI::FrameWindow* frame = static_cast<CEGUI::FrameWindow*>(winMgr->getWindow("Inventory/Frame"));
  frame->subscribeEvent(CEGUI::FrameWindow::EventCloseClicked, CEGUI::Event::Subscriber(&InventoryWindow::handleCloseButton, this));

  CEGUI::Window* root = winMgr->getWindow("Root");
  root->subscribeEvent(CEGUI::Window::EventDragDropItemDropped, CEGUI::Event::Subscriber(&InventoryWindow::handleDragDroppedRoot, this));


  CEGUI::Window* bag = winMgr->getWindow("Inventory/Bag");

  for (int j=0; j<8; j++)
  {
    for (int i=0; i<4; i++)
    {
      createDragDropSlot(bag, CEGUI::Point(4.0f+(39*i), 4.0f+(39*j)));
    }
  }

  // Setup the skill list
  btn = winMgr->getWindow("Inventory/StatsFrame");
  CEGUI::String str_skill("Skill");
  CEGUI::String str_value("Value");
  ((CEGUI::MultiColumnList*)btn)->addColumn(str_skill,0,0.6f);
  ((CEGUI::MultiColumnList*)btn)->addColumn(str_value,1,0.4f);
  ((CEGUI::MultiColumnList*)btn)->setSelectionMode(CEGUI::MultiColumnList::RowSingle);
  ((CEGUI::MultiColumnList*)btn)->setUserColumnSizingEnabled(false);


 // Get the root window
  rootwindow = winMgr->getWindow("Inventory/Frame");


}
