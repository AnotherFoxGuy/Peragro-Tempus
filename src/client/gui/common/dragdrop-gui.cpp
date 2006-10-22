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

  if(!newslot->IsEmpty()) return true;

  if(oldslot->GetParent() == Slot::Trade)
  {
    Slot* oldinvslot = guimanager->GetTradeWindow()->GetOldSlot(oldslot);
    MoveObject(oldslot, oldinvslot);
    if(oldinvslot) oldslot = oldinvslot;
    guimanager->GetTradeWindow()->UpdateOffer();
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

  // If it's a equiped item unequip it.
  if(slot->GetId() < 10)
  {
    int ownid = PointerLibrary::getInstance()->getEntityManager()->GetOwnId();
    PointerLibrary::getInstance()->getEntityManager()->unequip(ownid, slot->GetId());
  }

  ddea.dragDropItem->destroy();
  Object* object = slot->GetObject();
  delete object;
  slot->Clear();

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
  {
    icon = winMgr->createWindow("DragContainer", uniquename);
    icon->disable();
  }

  icon->setPosition(CEGUI::UVector2(CEGUI::UDim(0.0f,0.0f), CEGUI::UDim(0.0f,0.0f)));
  icon->setSize(CEGUI::UVector2(CEGUI::UDim(0.0f,21.6f), CEGUI::UDim(0.0f,21.6f)));
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

CEGUI::String DragDrop::IntToStr(int number)
{
  char buffer[1024];
  sprintf(buffer, "%d", number);
  CEGUI::String value = (CEGUI::String)buffer;

  return value;
}

void DragDrop::CreateItem(Slot* slot, uint itemid, bool interactable)
{
  ClientItem* clientitem = itemmanager->GetItemById(itemid);

  if (!clientitem)
  {
    printf("DragDrop: ERROR Failed to create item %d!\n", itemid);
    return;
  }

  Object* object = new Object();
  object->SetId(itemid);
  object->SetWindow(createIcon(DragDrop::Item, itemid, interactable));
  slot->SetObject(object);
  slot->GetWindow()->addChildWindow(object->GetWindow());
}

void DragDrop::MoveObject(Slot* oldslot, Slot* newslot)
{
  Object* object = oldslot->GetObject();
  oldslot->MoveObjectTo(newslot);
  newslot->GetWindow()->addChildWindow(object->GetWindow());
}