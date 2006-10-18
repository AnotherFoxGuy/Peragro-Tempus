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
#include "client/gui/gui.h"


DragDrop::DragDrop (GUIManager* guimanager)
{
  this->guimanager = guimanager;
  winMgr = guimanager->GetCEGUI()->GetWindowManagerPtr ();
  itemmanager = PointerLibrary::getInstance()->getItemManager();
  network = PointerLibrary::getInstance()->getNetwork();
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

  return true;
}

bool DragDrop::handleDragDropped(const CEGUI::EventArgs& args)
{
  using namespace CEGUI;

  const DragDropEventArgs& ddea = static_cast<const DragDropEventArgs&>(args);
  ddea.window->setProperty("FrameColours", "tl:FFFFFFFF tr:FFFFFFFF bl:FFFFFFFF br:FFFFFFFF");

  Slot* oldslot = static_cast<Slot*>(ddea.dragDropItem->getParent()->getUserData());
  Slot* newslot = static_cast<Slot*>(ddea.window->getUserData());

  if(oldslot->GetParent() == Slot::Trade)
  {
    Slot* oldinvslot = guimanager->GetTradeWindow()->GetOldSlot(oldslot);
    MoveObject(oldslot, oldinvslot);
    if(oldinvslot) oldslot = oldinvslot;
  }

  EquipRequestMessage slotchangemsg;
  slotchangemsg.setNewSlotId(newslot->GetId());
  slotchangemsg.setOldSlotId(oldslot->GetId());
  if (network) network->send(&slotchangemsg);

  return true;

}

bool DragDrop::handleDragDroppedRoot(const CEGUI::EventArgs& args)
{
  using namespace CEGUI;

  const DragDropEventArgs& ddea = static_cast<const DragDropEventArgs&>(args);
  int objectid = -1;

  Slot* slot = static_cast<Slot*>(ddea.dragDropItem->getParent()->getUserData());
  objectid = slot->GetObject()->GetId();

  if(objectid < 1) return true;

  DropRequestMessage msg;
  msg.setTarget(objectid);
  msg.setSlotId(slot->GetId());
  if (network) network->send(&msg);

  if( slot->GetObject()->GetAmount() > 1)
  {
    slot->GetObject()->SetAmount(slot->GetObject()->GetAmount()-1);
    UpdateItemCounter(slot->GetWindow(), slot->GetObject()->GetAmount());
  }
  else 
  {
    ddea.dragDropItem->destroy();
    Object* object = slot->GetObject();
    delete object;
    slot->Clear();
  }

  printf("InventoryWindow: Dropped item of type %d of slot %d to the world!\n", objectid, slot->GetId());

  return true;
}

bool DragDrop::handleDragDroppedTrade(const CEGUI::EventArgs& args)
{
  using namespace CEGUI;

  const DragDropEventArgs& ddea = static_cast<const DragDropEventArgs&>(args);
  ddea.window->setProperty("FrameColours", "tl:FFFFFFFF tr:FFFFFFFF bl:FFFFFFFF br:FFFFFFFF");

  Slot* oldslot = static_cast<Slot*>(ddea.dragDropItem->getParent()->getUserData());
  Slot* newslot = static_cast<Slot*>(ddea.window->getUserData());

  guimanager->GetTradeWindow()->AddItem(oldslot, newslot);

  return true;

}

CEGUI::Window* DragDrop::createDragDropSlot(CEGUI::Window* parent, const CEGUI::UVector2& position)
{
  // Create the slot
  CEGUI::Window* slot = winMgr->createWindow("Peragro/StaticImage");
  parent->addChildWindow(slot);
  slot->setPosition(position);
  slot->setSize(CEGUI::UVector2(CEGUI::UDim(0,24.0f), CEGUI::UDim(0,24.0f)));
  slot->subscribeEvent(CEGUI::Window::EventDragDropItemEnters, CEGUI::Event::Subscriber(&DragDrop::handleDragEnter, this));
  slot->subscribeEvent(CEGUI::Window::EventDragDropItemLeaves, CEGUI::Event::Subscriber(&DragDrop::handleDragLeave, this));
  slot->subscribeEvent(CEGUI::Window::EventDragDropItemDropped, CEGUI::Event::Subscriber(&DragDrop::handleDragDropped, this));

  // Create the itemcounter
  CEGUI::Window* itemcounter = winMgr->createWindow("Peragro/StaticText");
  slot->addChildWindow(itemcounter);
  itemcounter->setPosition(CEGUI::UVector2(CEGUI::UDim(0.60f,0), CEGUI::UDim(0.15f,0)));
  itemcounter->setSize(CEGUI::UVector2(CEGUI::UDim(0.85f,0), CEGUI::UDim(0.80f,0)));
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

CEGUI::Window* DragDrop::createIcon(int icontype, int objectid, bool interactable)
{
  char uniquename[1024];
  counter += 1;
  sprintf(uniquename, "%d_%d_icon", objectid, counter);

  // Create a drag/drop Icon
  CEGUI::Window* icon;
  if(interactable)
    icon = winMgr->createWindow("DragContainer", uniquename);
  else
    icon = winMgr->createWindow("Peragro/StaticImage", uniquename);

  icon->setPosition(CEGUI::UVector2(CEGUI::UDim(0.0f,0.0f), CEGUI::UDim(0.0f,0.0f)));
  icon->setSize(CEGUI::UVector2(CEGUI::UDim(0.9f,0.0f), CEGUI::UDim(0.9f,0.0f)));
  icon->setHorizontalAlignment(CEGUI::HA_CENTRE);
  icon->setVerticalAlignment(CEGUI::VA_CENTRE);

  // Set a static image as drag container's contents
  CEGUI::Window* iconImage = winMgr->createWindow("Peragro/StaticImage");
  icon->addChildWindow(iconImage);
  iconImage->setPosition(CEGUI::UVector2(CEGUI::UDim(0.0f,0.0f), CEGUI::UDim(0.0f,0.0f)));
  iconImage->setSize(CEGUI::UVector2(CEGUI::UDim(1.0f,0.0f), CEGUI::UDim(1.0f,0.0f)));
  iconImage->setHorizontalAlignment(CEGUI::HA_CENTRE);
  iconImage->setVerticalAlignment(CEGUI::VA_CENTRE);
  iconImage->setProperty("FrameEnabled", "False");
  // Disable to allow inputs to pass through.
  iconImage->disable();

  // Lets decide what to make of the icon: Item or Skill.
  if(icontype == DragDrop::Item)
  {
    ClientItem* clientitem = itemmanager->GetItemById(objectid);

    // Set some variables.
    icon->setUserString("itemid" , IntToStr(objectid));
    icon->setUserString("icontype" , IntToStr(icontype));
    icon->setTooltipText(clientitem->GetName().GetData());

    icon->setUserData(clientitem);
    //ClientItem* clientitem2 = static_cast<ClientItem*>(icon->getUserData());
    //printf("================%s\n", clientitem2->GetName().GetData() );

    iconImage->setProperty("Image", clientitem->GetIconName().GetData());
  }
  else if(icontype == DragDrop::Skill)
  {

  }

  return icon;
}

void DragDrop::UpdateItemCounter(CEGUI::Window* parent, uint amount)
{
  if (!parent->isChild(30)) return;
  CEGUI::Window* itemcounter = parent->getChild(30);
  itemcounter->setText(IntToStr(amount));
  if (amount < 2) itemcounter->setVisible(false); 
  else itemcounter->setVisible(true);
}

CEGUI::String DragDrop::IntToStr(int number)
{
  char buffer[1024];
  sprintf(buffer, "%d", number);
  CEGUI::String value = (CEGUI::String)buffer;

  return value;
}

void DragDrop::CreateItem(Slot* slot, uint itemid, uint amount)
{
  ClientItem* clientitem = itemmanager->GetItemById(itemid);

  Object* object = new Object();
  object->SetId(itemid);
  object->SetAmount(amount);
  object->SetWindow(createIcon(DragDrop::Item, itemid));
  // If stackable is bigger then 1 the item is stackable by that amount.
  // If stackable equals 0 its infinitly stackable.
  if (clientitem->GetStackable() > 1 || clientitem->GetStackable() == 0)
    object->SetStackable(true);
  slot->SetObject(object);
  slot->GetWindow()->addChildWindow(object->GetWindow());
}

void DragDrop::MoveObject(Slot* oldslot, Slot* newslot)
{
  Object* object = oldslot->GetObject();

  oldslot->MoveObjectTo(newslot);
  UpdateItemCounter(oldslot->GetWindow(), 0);
  newslot->GetWindow()->addChildWindow(object->GetWindow());
  UpdateItemCounter(newslot->GetWindow(), newslot->GetObject()->GetAmount());
}