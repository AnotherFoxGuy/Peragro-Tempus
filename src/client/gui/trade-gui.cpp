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

#include "client/gui/trade-gui.h"

#include <CEGUI.h>
#include <CEGUIWindowManager.h>
#include <CEGUILogger.h>

#include "client/network/network.h"

#include "common/reporter/reporter.h"

#include "client/gui/guimanager.h"
#include "client/gui/inventory-gui.h"
#include "client/gui/confirmdialog-gui.h"
#include "client/gui/common/slot.h"

namespace PT
{
  namespace GUI
  {
    namespace Windows
    {
      TradeWindow::TradeWindow(GUIManager* guimanager)
        : GUIWindow (guimanager)
      {
        windowName = TRADEWINDOW;
        accept1 = false;
        accept2 = false;
      } // end TradeWindow()

      TradeWindow::~TradeWindow()
      {
        delete trade1;
        delete trade2;
        inventory.DeleteAll();
      } // end ~TradeWindow()

      bool TradeWindow::OnYesConfirm(const CEGUI::EventArgs& args)
      {
        TradeConfirmRequestMessage msg;
        network->send(&msg);

        return true;
      } // end OnYesConfirm()

      bool TradeWindow::OnNoConfirm(const CEGUI::EventArgs& args)
      {
        TradeCancelMessage msg;
        network->send(&msg);

        return true;
      } // end OnNoConfirm()

      bool TradeWindow::OnCloseButton(const CEGUI::EventArgs& args)
      {
        TradeWindow::CancelTrade();

        TradeCancelMessage msg;
        network->send(&msg);

        return true;
      } // end OnCloseButton()

      bool TradeWindow::OnAcceptPlayer1(const CEGUI::EventArgs& args)
      {
        SetAccept(1, true);
        TradeOfferAcceptMessage msg;
        network->send(&msg);
        return true;
      }

      void TradeWindow::ClearItems()
      {
        //dragdrop->ClearSlotsDelete(trade2);
      } // end ClearItems()

      bool TradeWindow::AddItem(unsigned int player, unsigned int itemid,
                   unsigned int slotid, const char* name, const char* iconname)
      {
        if (!(player == 2)) return false;
        if (slotid > numberOfSlots) return false;

        Slot* slot = trade2->GetSlot(slotid);

        if (!slot)
        {
          Report(PT::Error, "TradeWindow: Couldn't add item %d in slot %d!",
            itemid, slotid);
          return false;
        }

        // Create a new non-interactable item.
        slot->SetObject(dragdrop->CreateItem(itemid, name, iconname, false));

        Report(PT::Debug, "TradeWindow: Creating item %d!", itemid);

        return true;
      } // end AddItem()

      void TradeWindow::SetAccept(unsigned int player, bool value)
      {
        if (player == 1)
        {
          btn = winMgr->getWindow("TradeWindow/Player1/Accept");
          value ? btn->disable() : btn->enable();
          accept1 = value;
        }
        else if (player == 2)
        {
          btn = winMgr->getWindow("TradeWindow/Player2/Accept");
          value ? btn->disable() : btn->enable();
          accept2 = value;
        }

        // Check if both are in accept state.
        if (accept1 && accept2)
        {
          PT::GUI::Windows::ConfirmDialogWindow* dialog =
            new PT::GUI::Windows::ConfirmDialogWindow(guimanager);
          dialog->SetText("You sure you want to trade?");
          dialog->SetYesEvent(
            CEGUI::Event::Subscriber(&TradeWindow::OnYesConfirm, this));
          dialog->SetNoEvent(
            CEGUI::Event::Subscriber(&TradeWindow::OnNoConfirm, this));
        }
      } // end SetAccept()

      bool TradeWindow::AddItem(Slot* oldslot, Slot* newslot)
      {
        if (!oldslot || !newslot)
        {
          Report(PT::Error,
            "TradeWindow: Couldn't move item from slot to slot!");
          return false;
        }

        // If we drag the icon around to the same
        // slot in the trade window, return.
        if ((oldslot->GetParent() == Inventory::TradeLeft)
          && oldslot->GetId() == newslot->GetId())
          return true;

        if (oldslot->GetParent() == Inventory::TradeLeft)
        {
          dragdrop->MoveObject(oldslot, newslot);
          inventory.Put(newslot->GetId(), inventory[oldslot->GetId()]);
          inventory.Put(oldslot->GetId(), 0);
        }
        else
        {
          inventory.Put(newslot->GetId(), oldslot);
          // Disable the icon in the inventory.
          Object* object = oldslot->GetObject();
          object->GetWindow()->disable();
          // Create a new item in the trade inventory.
          newslot->SetObject(dragdrop->CreateItem(object->GetId(),
            object->GetName(), object->GetIconName()));
        }

        // Send an updated state of the trade inventory.
        TradeWindow::UpdateOffer();

        return true;
      } // end AddItem()

      void TradeWindow::UpdateOffer()
      {
        // Make a list of items and send it to the network.
        // Get actual items.
        ExchangeOffersListMessage msg;
        csArray<Inventory::ObjectAndSlot> objandslot = trade1->GetAllObjects();

        // Make the offer list.
        msg.setOffersCount((unsigned char)objandslot.GetSize());
        Report(PT::Debug, "------------------------------------------");
        Report(PT::Debug, "TradeWindow: Creating Offer List Pvp for %d items",
          objandslot.GetSize());
        for (size_t i=0; i<objandslot.GetSize(); i++)
        {
          Inventory::ObjectAndSlot objslot = objandslot.Get(i);
          Report(PT::Debug, "item %d in slot %d!", objslot.object->GetId(),
            objslot.slot->GetId());
          msg.setItemId(i, objslot.object->GetId());
          msg.setSlotId(i, objslot.slot->GetId());
        }

        network->send(&msg);
        Report(PT::Debug, "SEND");

        Report(PT::Debug, "------------------------------------------");
      } // end UpdateOffer()

      Slot* TradeWindow::GetOldSlot(Slot* slot)
      {
        if (!slot)
        {
          Report(PT::Error,
            "TradeWindow: Couldn't get old slot, invalid slot!");
          return 0;
        }

        Slot* oldslot = inventory[slot->GetId()];

        if (!oldslot)
        {
          Report(PT::Error, "TradeWindow: Couldn't get old slot!");
          return 0;
        }

        return oldslot;
      } // end GetOldSlot()

      void TradeWindow::CancelTrade()
      {
        winMgr->getWindow("TradeWindow/Frame")->setVisible(false);

        // Putting the items back in the inventory.
        for (unsigned int i=0; i<numberOfSlots; i++)
        {
          Slot* slot = trade1->GetSlot(i);
          if (!slot->IsEmpty())
          {
            Slot* oldslot = inventory[i];
            oldslot->GetObject()->GetWindow()->enable();
          }
        }

        trade1->ClearSlotsDelete();
        trade2->ClearSlotsDelete();
        inventory.DeleteAll();
        SetAccept(1, false);
        SetAccept(2, false);

      } // end CancelTrade()

      void TradeWindow::AcceptTrade()
      {
        winMgr->getWindow("TradeWindow/Frame")->setVisible(false);

        int nrInventorySlots = 30;

        // Putting the new items in the inventory.
        int counter = 10;
        InventoryWindow* inventoryWindow =
          guimanager->GetWindow<InventoryWindow>(INVENTORYWINDOW);

        for (unsigned int i=0; i<numberOfSlots; i++)
        {
          Slot* slot = trade2->GetSlot(i);
          if (!slot->IsEmpty())
          {
            Object* object = slot->GetObject();
            while (!inventoryWindow->
              AddItem(object->GetId(), object->GetName(), object->GetIconName(), counter)
              && counter < nrInventorySlots)
            {
              counter += 1;
            }
          }
        }

        // Deleting the traded items in the inventory.
        for (unsigned int i=0; i<numberOfSlots; i++)
        {
          Slot* slot = trade1->GetSlot(i);
          if (!slot->IsEmpty())
          {
            Slot* oldslot = inventory[i];
            inventoryWindow->RemoveItem(oldslot->GetId());
          }
        }

        trade1->ClearSlotsDelete();
        trade2->ClearSlotsDelete();
        inventory.DeleteAll();
        SetAccept(1, false);
        SetAccept(2, false);
      } // end AcceptTrade()

      bool TradeWindow::Create()
      {
        ReloadWindow();
        return true;
      } // end Create()

      bool TradeWindow::ReloadWindow()
      {
        numberOfSlots = 16;

        window = GUIWindow::LoadLayout ("client/trade-pc.layout");
        GUIWindow::AddToRoot(window);
        winMgr = cegui->GetWindowManagerPtr ();

        dragdrop = guimanager->GetDragDrop();

        // Get the frame window.
        CEGUI::FrameWindow* frame = static_cast<CEGUI::FrameWindow*>
          (winMgr->getWindow("TradeWindow/Frame"));
        frame->subscribeEvent(CEGUI::FrameWindow::EventCloseClicked,
          CEGUI::Event::Subscriber(&TradeWindow::OnCloseButton, this));

        // Get the player1 accept button.
        CEGUI::PushButton* accept1 = static_cast<CEGUI::PushButton*>
          (winMgr->getWindow("TradeWindow/Player1/Accept"));
        accept1->subscribeEvent(CEGUI::PushButton::EventClicked,
          CEGUI::Event::Subscriber(&TradeWindow::OnAcceptPlayer1, this));

        // Get the player2 accept button.
        CEGUI::PushButton* accept2 = static_cast<CEGUI::PushButton*>
          (winMgr->getWindow("TradeWindow/Player2/Accept"));
        accept2->setMousePassThroughEnabled(true);

        // Populate the Player1 bag with slots.
        CEGUI::Window* bag1 = winMgr->getWindow("TradeWindow/Player1/Bag");
        trade1 = new Inventory(guimanager);
        trade1->Create(bag1, Inventory::TradeLeft, DragDrop::Item, 4, 4);

        // Populate the Player2 bag with slots.
        CEGUI::Window* bag2 = winMgr->getWindow("TradeWindow/Player2/Bag");
        trade2 = new Inventory(guimanager);
        trade2->Create(bag2, Inventory::TradeRight, DragDrop::Item, 4, 4);

        return true;
      } // end ReloadWindow()

    } // Windows namespace
  } // GUI namespace
} // PT namespace

