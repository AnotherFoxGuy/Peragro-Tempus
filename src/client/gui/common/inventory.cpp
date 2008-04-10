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

#include "client/gui/common/inventory.h"

#include "client/reporter/reporter.h"


Inventory::Inventory(GUIManager* guimanager)
{
  this->guimanager = guimanager;
  dragdrop = guimanager->GetDragDrop();
  winMgr = guimanager->GetCEGUI()->GetWindowManagerPtr ();
}

Inventory::~Inventory()
{
  ClearSlotsDelete();
  slotarray.DeleteAll();
}

CEGUI::Window* Inventory::createDragDropSlot(CEGUI::Window* parent, const CEGUI::UVector2& position)
{
  // Create the slot
  CEGUI::Window* slot = winMgr->createWindow("Peragro/StaticImage");
  parent->addChildWindow(slot);
  slot->setPosition(position);
  slot->setSize(CEGUI::UVector2(CEGUI::UDim(0,24.0f), CEGUI::UDim(0,24.0f)));
  slot->subscribeEvent(CEGUI::Window::EventDragDropItemEnters, CEGUI::Event::Subscriber(&DragDrop::handleDragEnter, dragdrop));
  slot->subscribeEvent(CEGUI::Window::EventDragDropItemLeaves, CEGUI::Event::Subscriber(&DragDrop::handleDragLeave, dragdrop));
  slot->subscribeEvent(CEGUI::Window::EventDragDropItemDropped, CEGUI::Event::Subscriber(&DragDrop::handleDragDropped, dragdrop));

  return slot;
}

void Inventory::ClearSlotsDelete()
{
  // Clears the inventory and deletes the objects.
  for (size_t i=0; i<slotarray.GetSize(); i++)
  {
    Slot* slot = slotarray[i];
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

unsigned int Inventory::FindFreeSlot()
{
  for (size_t i = 10; i < slotarray.GetSize(); i++)
  {
    Slot* slot = slotarray.Get(i);

    if (!slot)
      continue;

    if (slot->IsEmpty())
      return slot->GetId();
  }
  Report(PT::Error, "Inventory: Inventory full!");
  return ~0;
}

unsigned int Inventory::FindObject(unsigned int itemid)
{
  for (size_t i = 0; i < slotarray.GetSize(); i++)
  {
    Slot* slot = slotarray.Get(i);

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

bool Inventory::RemoveObject(unsigned int slotid)
{
  Slot* slot = slotarray[slotid];
  if (!slot) return false;
  Object* object = slot->GetObject();
  if (!object) return false;
  object->GetWindow()->destroy();
  delete object;
  slot->Clear();
  return true;
}

csArray<Inventory::ObjectAndSlot> Inventory::GetAllObjects()
{
  csArray<ObjectAndSlot> objects;
  for (size_t i=0; i<slotarray.GetSize(); i++)
  {
    Slot* slot = slotarray[i];
    if (!slot) continue;
    Object* object = slot->GetObject();
    if(object)
    {
      ObjectAndSlot objandslot;
      objandslot.object = object;
      objandslot.slot = slot;
      objects.Push(objandslot);
    }
  }

  return objects;
}

void Inventory::Create(CEGUI::Window* bag, Inventory::ParentType parent, DragDrop::Type type , int rows, int columns, int offset)
{
  for (int j=0; j<rows; j++)
  {
    for (int i=0; i<columns; i++)
    {
      Slot* slot = new Slot();
      slot->SetId((i+(j*columns))+offset);
      slot->SetType(type);
      slot->SetParent(parent);
      slot->SetWindow(createDragDropSlot(bag, CEGUI::UVector2(CEGUI::UDim(0,4.0f+(28*i)), CEGUI::UDim(0,4.0f+(28*j)))));
      slot->GetWindow()->setUserData(slot);

      switch(parent)
      {
      case Inventory::BuyUpper:
        slot->GetWindow()->removeEvent(CEGUI::Window::EventDragDropItemDropped);
        slot->GetWindow()->subscribeEvent(CEGUI::Window::EventDragDropItemDropped,
          CEGUI::Event::Subscriber(&DragDrop::handleDragDroppedBuy, dragdrop));
        break;
      case Inventory::BuyLower:
        slot->GetWindow()->removeEvent(CEGUI::Window::EventDragDropItemDropped);
        slot->GetWindow()->subscribeEvent(CEGUI::Window::EventDragDropItemDropped,
          CEGUI::Event::Subscriber(&DragDrop::handleDragDroppedBuy, dragdrop));
        break;
      case Inventory::InventoryLower:
        // Nothing to do.
        break;
      case Inventory::TradeLeft:
        slot->GetWindow()->removeEvent(CEGUI::Window::EventDragDropItemDropped);
        slot->GetWindow()->subscribeEvent(CEGUI::Window::EventDragDropItemDropped,
          CEGUI::Event::Subscriber(&DragDrop::handleDragDroppedTrade, dragdrop));
        break;
      case Inventory::TradeRight:
        slot->GetWindow()->removeAllEvents();
        break;

      default: Report(PT::Error, "DragDrop: Unknown ParentType %d !", parent);
      }

      slotarray.Put(slot->GetId(), slot);
    }
  }
}
