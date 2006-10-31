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

  if(!newslot->IsEmpty()) 
  {
    Object* obj = newslot->GetObject();
    if(obj->GetWindow()->isDisabled())
      return true;
  }

  if(oldslot->GetParent() == Slot::TradeLeft)
  {
    // Enable the inventory icon again.
    Slot* oldinvslot = guimanager->GetTradeWindow()->GetOldSlot(oldslot);
    oldinvslot->GetObject()->GetWindow()->enable();
    // Destroy the trade icon.
    Object* tradeobj = oldslot->GetObject();
    tradeobj->GetWindow()->destroy();
    delete tradeobj;
    oldslot->Clear();

    if(oldinvslot) oldslot = oldinvslot;
    guimanager->GetTradeWindow()->UpdateOffer();
  }

  InventoryMoveItemRequestMessage slotchangemsg;
  slotchangemsg.setNewSlot(newslot->GetId());
  slotchangemsg.setOldSlot(oldslot->GetId());
  if (network) network->send(&slotchangemsg);

  return true;
}

bool DragDrop::handleDragDroppedIcon(const CEGUI::EventArgs& args)
{
  using namespace CEGUI;

  const DragDropEventArgs& ddea = static_cast<const DragDropEventArgs&>(args);

  DragDropEventArgs& ddea1 = const_cast<DragDropEventArgs&>(ddea);

  // An icon has been dropped on an icon,
  // relay the event to the parent, the slot.
  ddea1.window = ddea.window->getParent();

  DragDrop::handleDragDropped(ddea1);

  printf("DragDrop::handleDragDroppedIcon.\n");

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
  msg.setItemId(objectid);
  msg.setSlot(slot->GetId());
  if (network) network->send(&msg);

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

bool DragDrop::handleDragDroppedBuy(const CEGUI::EventArgs& args)
{
  using namespace CEGUI;

  const DragDropEventArgs& ddea = static_cast<const DragDropEventArgs&>(args);
  ddea.window->setProperty("FrameColours", "tl:FFFFFFFF tr:FFFFFFFF bl:FFFFFFFF br:FFFFFFFF");

  Slot* oldslot = static_cast<Slot*>(ddea.dragDropItem->getParent()->getUserData());
  Slot* newslot = static_cast<Slot*>(ddea.window->getUserData());

  if(oldslot->GetParent() == Slot::BuyLower || oldslot->GetParent() == Slot::BuyUpper) 
    guimanager->GetBuyWindow()->MoveItem(oldslot, newslot);

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

  // Setup event for swapping.
  icon->subscribeEvent(CEGUI::Window::EventDragDropItemDropped, CEGUI::Event::Subscriber(&DragDrop::handleDragDroppedIcon, this));

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

Object* DragDrop::CreateItem(uint itemid, bool interactable)
{
  ClientItem* clientitem = itemmanager->GetItemById(itemid);

  if (!clientitem)
  {
    printf("DragDrop: ERROR Failed to create item %d!\n", itemid);
    return 0;
  }

  Object* object = new Object();
  object->SetId(itemid);
  object->SetWindow(createIcon(DragDrop::Item, itemid, interactable));

  return object;
}

void DragDrop::MoveObject(Slot* oldslot, Slot* newslot)
{
  if(!newslot->IsEmpty())
  {
    // If slot is occupied: swap.
    printf("DragDrop: Swapping objects!\n");
    Object* object1 = oldslot->GetObject();
    Object* object2 = newslot->GetObject();
    CEGUI::Window* icon1 = oldslot->GetObject()->GetWindow();
    CEGUI::Window* icon2 = newslot->GetObject()->GetWindow();
    oldslot->SetObject(object2);
    newslot->SetObject(object1);
    oldslot->GetWindow()->addChildWindow(icon2);  
    newslot->GetWindow()->addChildWindow(icon1);
    // Forces CEGUI to update.
    icon1->notifyScreenAreaChanged();
    icon2->notifyScreenAreaChanged();
  }
  else
  {
    Object* object = oldslot->GetObject();
    oldslot->MoveObjectTo(newslot);
    newslot->GetWindow()->addChildWindow(object->GetWindow());
  }
}

void DragDrop::ClearSlotsDelete(csArray<Slot*> arr)
{
  // Clears the inventory and deletes the objects. 
  for (size_t i=0; i<arr.GetSize(); i++)
  {
    Slot* slot = arr[i];
    if (!slot) continue;
    Object* object = slot->GetObject();
    if(object)
    {
      object->GetWindow()->destroy();
      delete object;
      slot->Clear();
    }
  }
}

void DragDrop::CreateBag(CEGUI::Window* bag, csArray<Slot*>* slotarray, Slot::ParentType parent, DragDrop::Type type , int rows, int columns)
{
  for (int j=0; j<rows; j++)
  {
    for (int i=0; i<columns; i++)
    {
      Slot* slot = new Slot();
      slot->SetId((i+(j*columns)));
      slot->SetType(type);
      slot->SetParent(parent);
      slot->SetWindow(createDragDropSlot(bag, CEGUI::UVector2(CEGUI::UDim(0,4.0f+(28*i)), CEGUI::UDim(0,4.0f+(28*j)))));
      slot->GetWindow()->setUserData(slot);

      switch(parent)
      {
      case Slot::BuyUpper:
        slot->GetWindow()->removeEvent(CEGUI::Window::EventDragDropItemDropped);
        slot->GetWindow()->subscribeEvent(CEGUI::Window::EventDragDropItemDropped, 
          CEGUI::Event::Subscriber(&DragDrop::handleDragDroppedBuy, this));
        break;
      case Slot::BuyLower:
        slot->GetWindow()->removeEvent(CEGUI::Window::EventDragDropItemDropped);
        slot->GetWindow()->subscribeEvent(CEGUI::Window::EventDragDropItemDropped, 
          CEGUI::Event::Subscriber(&DragDrop::handleDragDroppedBuy, this));
        break;
      case Slot::Inventory:
        // Nothing to do.
        break;
      case Slot::TradeLeft:
        slot->GetWindow()->removeEvent(CEGUI::Window::EventDragDropItemDropped);
        slot->GetWindow()->subscribeEvent(CEGUI::Window::EventDragDropItemDropped, 
          CEGUI::Event::Subscriber(&DragDrop::handleDragDroppedTrade, this));
        break;
      case Slot::TradeRight:
        slot->GetWindow()->removeAllEvents();
        break;

      default: printf("DragDrop: Unknown ParentType %d !\n", parent);
      }

      slotarray->Put(slot->GetId(), slot);
    }
  }
}