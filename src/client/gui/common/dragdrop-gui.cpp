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

#include "client/gui/common/dragdrop-gui.h"

#include "client/item/itemmanager.h"

#include "CEGUI.h"
#include "CEGUIWindowManager.h" 
#include "CEGUILogger.h"

#include "client/network/network.h"
#include "client/gui/guimanager.h"


DragDrop::DragDrop (GUIManager* guimanager)
{
  this->guimanager = guimanager;
  winMgr = guimanager->GetCEGUI()->GetWindowManagerPtr ();
  Client* client = guimanager->GetClient();
  itemmanager = client->getItemmgr();
}

DragDrop::~DragDrop ()
{
}

bool DragDrop::handleDragEnter(const CEGUI::EventArgs& args)
{
  using namespace CEGUI;

  const DragDropEventArgs& ddea = static_cast<const DragDropEventArgs&>(args);
  ddea.window->setProperty("FrameColours", "tl:FF00FF00 tr:FF00FF00 bl:FF00FF00 br:FF00FF00");

  return true;
}

bool DragDrop::handleDragLeave(const CEGUI::EventArgs& args)
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

bool DragDrop::handleDragDropped(const CEGUI::EventArgs& args)
{
  using namespace CEGUI;

  const DragDropEventArgs& ddea = static_cast<const DragDropEventArgs&>(args);

  ddea.window->setProperty("FrameColours", "tl:FFFFFFFF tr:FFFFFFFF bl:FFFFFFFF br:FFFFFFFF");
  if (ddea.dragDropItem->isUserStringDefined("itemtype"))
  {
    ddea.window->addChildWindow(ddea.dragDropItem);
    ddea.window->setUserString("itemtype" , ddea.dragDropItem->getUserString("itemtype"));
    UpdateItemCounter(ddea.window);
  }
  return true;

}

bool DragDrop::handleDragDroppedRoot(const CEGUI::EventArgs& args)
{
  using namespace CEGUI;

  const DragDropEventArgs& ddea = static_cast<const DragDropEventArgs&>(args);
  int itemid = -1;

  Window* slot = ddea.dragDropItem->getParent();

  if (ddea.dragDropItem->isUserStringDefined("itemtype")) 
  { 
  itemid = atoi(ddea.dragDropItem->getUserString("itemtype").c_str()); 
  DropEntityRequestMessage msg;
  msg.setTargetId(itemid);
  network->send(&msg);
  ddea.dragDropItem->destroy();
  UpdateItemCounter(slot);
  printf("InventoryWindow: Dropped item of type %d to the world!\n", itemid);
  }
  else 
  {
    printf("InventoryWindow: Couldn't determine itemID, putting it back!\n");
  }

  return true;
}

bool DragDrop::handleDragDroppedStackable(const CEGUI::EventArgs& args)
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
      //printf("InventoryWindow: Slottype is %s and itemtype is %s \n", slottype, itemtype);

      if ( !strcmp(slottype, itemtype) ) 
      {
        ddea.window->getParent()->addChildWindow(ddea.dragDropItem);
        UpdateItemCounter(ddea.window->getParent());
      }
    }
  }

  return true;
}

CEGUI::Window* DragDrop::createDragDropSlot(CEGUI::Window* parent, const CEGUI::UVector2& position)
{
  // Create the slot
  CEGUI::Window* slot = winMgr->createWindow("Peragro/StaticImage");
  parent->addChildWindow(slot);
  slot->setWindowPosition(position);
  slot->setWindowSize(CEGUI::UVector2(CEGUI::UDim(0,24.0f), CEGUI::UDim(0,24.0f)));
  slot->subscribeEvent(CEGUI::Window::EventDragDropItemEnters, CEGUI::Event::Subscriber(&DragDrop::handleDragEnter, this));
  slot->subscribeEvent(CEGUI::Window::EventDragDropItemLeaves, CEGUI::Event::Subscriber(&DragDrop::handleDragLeave, this));
  slot->subscribeEvent(CEGUI::Window::EventDragDropItemDropped, CEGUI::Event::Subscriber(&DragDrop::handleDragDropped, this));

  // Create the itemcounter
  CEGUI::Window* itemcounter = winMgr->createWindow("Peragro/StaticText");
  slot->addChildWindow(itemcounter);
  itemcounter->setWindowPosition(CEGUI::UVector2(CEGUI::UDim(0.60f,0), CEGUI::UDim(0.15f,0)));
  itemcounter->setWindowSize(CEGUI::UVector2(CEGUI::UDim(0.85f,0), CEGUI::UDim(0.80f,0)));
  itemcounter->setVisible(false);
  itemcounter->disable();
  itemcounter->setText("0");
  itemcounter->setAlwaysOnTop(true);
  itemcounter->setInheritsAlpha(false);
  itemcounter->setAlpha(1.0);
  itemcounter->setID(30);
  itemcounter->setProperty("Font", "Commonwealth-8");
  itemcounter->setProperty("BackgroundEnabled", "False");
  itemcounter->setProperty("FrameEnabled", "False");

  return slot;
}

CEGUI::Window* DragDrop::createIcon(int type, int itemid, bool stackable)
{
  char uniquename[1024];
  counter += 1;
  sprintf(uniquename, "%d_%d_icon", itemid, counter);

  ClientItem* clientitem = itemmanager->GetItemById(itemid);
  printf("=================%d", clientitem->GetId());
  printf("=================%s", clientitem->GetName().GetData());

  // create a drag/drop item
  CEGUI::DragContainer* item = static_cast<CEGUI::DragContainer*>(
    winMgr->createWindow("DragContainer", uniquename));
  item->setWindowPosition(CEGUI::UVector2(CEGUI::UDim(0.0f,0.0f), CEGUI::UDim(0.0f,0.0f)));
  item->setWindowSize(CEGUI::UVector2(CEGUI::UDim(0.9f,0.0f), CEGUI::UDim(0.9f,0.0f)));
  item->setHorizontalAlignment(CEGUI::HA_CENTRE);
  item->setVerticalAlignment(CEGUI::VA_CENTRE);
  //item->setTooltipText(clientitem->GetDescription().GetData());
  // Set the itemID.
  char itemtypestr[1024];
  sprintf(itemtypestr, "%d", itemid);
  item->setUserString("itemtype" , itemtypestr);
  // Set wether or not the item is stackable
  if (stackable)
    item->setUserString("stackable" , "true");
  else item->setUserString("stackable" , "false");

  // set a static image as drag container's contents
  CEGUI::Window* itemIcon = winMgr->createWindow("Peragro/StaticImage");
  item->addChildWindow(itemIcon);
  itemIcon->setWindowPosition(CEGUI::UVector2(CEGUI::UDim(0.0f,0.0f), CEGUI::UDim(0.0f,0.0f)));
  itemIcon->setWindowSize(CEGUI::UVector2(CEGUI::UDim(1.0f,0.0f), CEGUI::UDim(1.0f,0.0f)));
  itemIcon->setHorizontalAlignment(CEGUI::HA_CENTRE);
  itemIcon->setVerticalAlignment(CEGUI::VA_CENTRE);
  itemIcon->setProperty("FrameEnabled", "False");
  //itemIcon->setProperty("Image", clientitem->GetIconName().GetData());

  // disable to allow inputs to pass through.
  itemIcon->disable();

  //if (stackable)
  item->subscribeEvent(CEGUI::Window::EventDragDropItemDropped, CEGUI::Event::Subscriber(&DragDrop::handleDragDroppedStackable, this));

  return item;
}

void DragDrop::UpdateItemCounter(CEGUI::Window* parent)
{
  if (!parent->isChild(30)) return;
  CEGUI::Window* itemcounter = parent->getChild(30);
  int nrofitems = itemcounter->getParent()->getChildCount()-1;
  char buffer[1024];
  sprintf(buffer, "%d", nrofitems);
  itemcounter->setText((CEGUI::String)buffer);
  if (nrofitems < 2) itemcounter->setVisible(false); 
  else itemcounter->setVisible(true);
}