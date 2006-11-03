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

      default: printf("DragDrop: Unknown ParentType %d !\n", parent);
      }

      slotarray.Put(slot->GetId(), slot);
    }
  }
}
