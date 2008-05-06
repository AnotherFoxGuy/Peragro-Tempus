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

#include "client/serversetup/serversetupmanager.h" //remove

#include "client/data/itemdatamanager.h"
#include "client/data/item.h"

#include "CEGUI.h"
#include "CEGUIWindowManager.h"
#include "CEGUILogger.h"

#include "client/network/network.h"
#include "client/gui/guimanager.h"
#include "client/gui/gui.h"

#include "client/event/interfaceevent.h"

#include "client/reporter/reporter.h"

#include "src/common/items/itemsid.h"

using namespace PT;

DragDrop::DragDrop (GUIManager* guimanager)
{
  this->guimanager = guimanager;
  winMgr = guimanager->GetCEGUI()->GetWindowManagerPtr ();
  itemDataManager = PointerLibrary::getInstance()->getServerSetupManager()->GetItemDataManager(); //TODO remove
  network = PointerLibrary::getInstance()->getNetwork();

  counter = 0;
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

  if(oldslot->GetParent() == Inventory::TradeLeft)
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

  Report(PT::Debug, "DragDrop::handleDragDroppedIcon.");

  return true;
}

bool DragDrop::handleDragDroppedRoot(const CEGUI::EventArgs& args)
{
  using namespace CEGUI;

  const DragDropEventArgs& ddea = static_cast<const DragDropEventArgs&>(args);
  int objectid = -1;

  Slot* slot = static_cast<Slot*>(ddea.dragDropItem->getParent()->getUserData());
  if (!slot) return false;
  objectid = slot->GetObject()->GetId();

  if (objectid < 1) return false;

  DropRequestMessage msg;
  //msg.setItemId(objectid);
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

  if(oldslot->GetParent() == Inventory::BuyLower || oldslot->GetParent() == Inventory::BuyUpper)
    guimanager->GetBuyWindow()->MoveItem(oldslot, newslot);

  return true;
}

bool DragDrop::handleRightClickedIcon(const CEGUI::EventArgs& args)
{
  using namespace CEGUI;

  const MouseEventArgs& mouseArgs = static_cast<const MouseEventArgs&>(args);

  // If input is different from RightButton,
  // swallow the event and don't do anything.
  if (mouseArgs.button != RightButton)
  {
    return true;
  }

  Slot* slot = static_cast<Slot*>(mouseArgs.window->getParent()->getUserData());
  if (!slot) return false;
  Object* object = slot->GetObject();
  if (!object) return false;

  PT::Events::EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
  csRef<iEvent> interfaceEvent = evmgr->CreateEvent("interface.interact");
  interfaceEvent->Add("entityId", slot->GetId());
  interfaceEvent->Add("objectId", object->GetId());
  interfaceEvent->Add("variationId", object->GetVariationId());

  // @TODO: Use equiptype in items.xml and add itemtype instead of hardcoding here.
  std::string actions = "Drop";

  if (object->GetId() == APPLEID)
  {
    actions += ", Eat";
  }
  if (object->GetId() == 6) // Book
  {
    actions += ", Read";
    if (object->GetVariationId() == 0)
    {
      // empty, so you can also write in it.
      actions += ", Write";
    }
  }

  interfaceEvent->Add("actions", actions.c_str());
  evmgr->AddEvent(interfaceEvent);

  return true;
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

  // Setup event for interaction
  icon->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&DragDrop::handleRightClickedIcon, this));

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
    PT::Data::Item* clientitem = itemDataManager->GetItemById(objectid);

    // Set some variables.
    icon->setUserString("itemid" , IntToStr(objectid));
    icon->setUserString("icontype" , IntToStr(icontype));
    icon->setTooltipText(clientitem->GetName());

    icon->setUserData(clientitem);
    //ClientItem* clientitem2 = static_cast<ClientItem*>(icon->getUserData());
    //printf("================%s\n", clientitem2->GetName().GetData() );

    iconImage->setProperty("Image", clientitem->GetIconName());
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

Object* DragDrop::CreateItem(uint itemid, unsigned int variationid, bool interactable)
{
  PT::Data::Item* clientitem = itemDataManager->GetItemById(itemid);

  if (!clientitem)
  {
    Report(PT::Error, "DragDrop: Failed to create item %d!", itemid);
    return 0;
  }

  Object* object = new Object();
  object->SetId(itemid);
  object->SetVariationId(variationid);
  object->SetWindow(createIcon(DragDrop::Item, itemid, interactable));

  return object;
}

void DragDrop::MoveObject(Slot* oldslot, Slot* newslot)
{
  if(!newslot->IsEmpty())
  {
    // If slot is occupied: swap.
    Report(PT::Debug, "DragDrop: Swapping objects!");
    Object* object1 = oldslot->GetObject();
    Object* object2 = newslot->GetObject();
    oldslot->SetObject(object2);
    newslot->SetObject(object1);
  }
  else
  {
    oldslot->MoveObjectTo(newslot);
  }
}
