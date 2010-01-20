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

#include "client/gui/inventory-gui.h"

#include "CEGUI.h"
#include "CEGUIWindowManager.h"
#include "CEGUILogger.h"

#include "client/pointer/pointer.h"
#include "client/network/network.h"

#include "common/reporter/reporter.h"

#include "client/gui/guimanager.h"
#include "client/gui/common/slot.h"

namespace PT
{
  namespace GUI
  {
    namespace Windows
    {

      InventoryWindow::InventoryWindow(GUIManager* guimanager)
        : GUIWindow (guimanager)
      {
        windowName = INVENTORYWINDOW;
        iObjectRegistry* obj_reg =
          PointerLibrary::getInstance()->getObjectRegistry();
        vfs =  csQueryRegistry<iVFS> (obj_reg);

        dragdrop = 0;
      } // end InventoryWindow()

      InventoryWindow::~InventoryWindow()
      {
        delete inventory;
      } // end ~InventoryWindow()

      bool InventoryWindow::handleCloseButton(const CEGUI::EventArgs& args)
      {
        winMgr->getWindow("Inventory/Frame")->setVisible(false);
        return true;
      } // end handleCloseButton()

      Slot* InventoryWindow::GetSlot(unsigned int slotid)
      {
        return inventory->GetSlot(slotid);
      } // end GetSlot()

      bool InventoryWindow::AddItem(unsigned int itemid,
                                    const char* name,
                                    const char* iconname,
                                    unsigned int slotid)
      {
        if (slotid > numberOfSlots) return false;

        Slot* slot = inventory->GetSlot(slotid);

        if (!slot)
        {
          Report(PT::Error, "InventoryWindow: Couldn't add item %d in slot %d!",
            itemid, slotid);
          return false;
        }

        if (!slot->IsEmpty())
        {
          Report(PT::Error,
            "InventoryWindow: Slot %d already occupied!", slotid);
          return false;
        }

        // Create a new item.
        if (slot->IsEmpty())
          slot->SetObject(dragdrop->CreateItem(itemid, name, iconname));

        return true;
      } // end AddItem()

      bool InventoryWindow::MoveItem(unsigned int oldslotid,
                                     unsigned int newslotid)
      {
        if (oldslotid == newslotid) return true;
        Slot* oldslot = inventory->GetSlot(oldslotid);
        Slot* newslot = inventory->GetSlot(newslotid);

        return MoveItem(oldslot, newslot);
      } // end MoveItem()

      bool InventoryWindow::MoveItem(Slot* oldslot, Slot* newslot)
      {
        if (!oldslot || !newslot)
        {
          Report(PT::Error,
            "InventoryWindow: Couldn't move item from slot to slot!");
          return false;
        }

        dragdrop->MoveObject(oldslot, newslot);

        return true;
      } // end MoveItem()

      bool InventoryWindow::RemoveItem(unsigned int slotid)
      {
        if (slotid > numberOfSlots) return false;

        if (inventory->RemoveObject(slotid))
          return true;
        else
        {
          Report(PT::Error,
            "InventoryWindow: Failed to remove item in slot %d!", slotid);
          return false;
        }
      } // end RemoveItem()

      unsigned int InventoryWindow::FindItem(unsigned int itemid)
      {
        return inventory->FindObject(itemid);
      } // end FindItem()

      unsigned int InventoryWindow::FindFreeSlot()
      {
        return inventory->FindFreeSlot();
      } // end FindFreeSlot()

      void InventoryWindow::SetupEquipSlot(unsigned int id, const char* window)
      {
        Slot* slot = new Slot();
        slot->SetId(id);
        slot->SetType(DragDrop::Item);
        CEGUI::Window* slotwin = winMgr->getWindow(window);
        slotwin->subscribeEvent(CEGUI::Window::EventDragDropItemEnters,
          CEGUI::Event::Subscriber(&DragDrop::handleDragEnter, dragdrop));
        slotwin->subscribeEvent(CEGUI::Window::EventDragDropItemLeaves,
          CEGUI::Event::Subscriber(&DragDrop::handleDragLeave, dragdrop));
        slotwin->subscribeEvent(CEGUI::Window::EventDragDropItemDropped,
          CEGUI::Event::Subscriber(&DragDrop::handleDragDropped, dragdrop));
        slot->SetWindow(slotwin);
        slot->GetWindow()->setUserData(slot);
        inventory->GetSlotArray()->Put(slot->GetId(), slot);
      } // end SetupEquipSlot()

      bool InventoryWindow::Create()
      {
        ReloadWindow();
        SetupToggleListener("Inventory");
        return true;
      } // end Create()

      bool InventoryWindow::ReloadWindow()
      {
        // First 10 slots(0-9) are equipment,
        // other 20 are inventory.
        numberOfSlots = 30-1;

        //TODO
        //Load the inventory icon imageset
        vfs->ChDir ("/peragro/art/skins/default/");
        cegui->GetImagesetManagerPtr()->create(
          "/peragro/art/skins/default/images/inventory.imageset", "Inventory");

        window = GUIWindow::LoadLayout ("client/inventory.layout");
        GUIWindow::AddToRoot(window);
        winMgr = cegui->GetWindowManagerPtr ();

        dragdrop = guimanager->GetDragDrop();

        // Get the frame window
        CEGUI::FrameWindow* frame = static_cast<CEGUI::FrameWindow*>
          (winMgr->getWindow("Inventory/Frame"));
        frame->subscribeEvent(CEGUI::FrameWindow::EventCloseClicked,
          CEGUI::Event::Subscriber(&InventoryWindow::handleCloseButton, this));

        CEGUI::Window* root = winMgr->getWindow("Root");
        root->subscribeEvent(CEGUI::Window::EventDragDropItemDropped,
          CEGUI::Event::Subscriber(&DragDrop::handleDragDroppedRoot, dragdrop));

        // Populate the bag with slots.
        CEGUI::Window* bag = winMgr->getWindow("Inventory/Bag");
        inventory = new Inventory(guimanager);
        inventory->Create(bag, Inventory::InventoryLower, DragDrop::Item, 4, 5, 10);

        // Setup the equipslots.
        SetupEquipSlot(0, "Inventory/EquipFrame/WeaponRight");

        return true;
      } // end ReloadWindow()

    } // Windows namespace
  } // GUI namespace
} // PT namespace
