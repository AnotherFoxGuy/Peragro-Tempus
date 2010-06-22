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

#include "client/gui/buy-gui.h"

#include <CEGUI.h>
#include <CEGUIWindowManager.h>
#include <CEGUILogger.h>

#include "client/network/network.h"

#include "common/reporter/reporter.h"

#include "client/gui/guimanager.h"
#include "client/gui/common/dragdrop-gui.h"
#include "client/gui/inventory-gui.h"
#include "client/gui/common/slot.h"

namespace PT
{
  namespace GUI
  {
    namespace Windows
    {
      BuyWindow::BuyWindow(GUIManager* guimanager)
        : GUIWindow (guimanager)
      {
        windowName = BUYWINDOW;
        totalmoney = 0;
      } // end BuyWindow()

      BuyWindow::~BuyWindow()
      {
        delete lowerslots;
        delete upperslots;
      } // end ~BuyWindow()

      bool BuyWindow::OnCloseButton(const CEGUI::EventArgs& args)
      {
        CancelTrade();

        return true;
      } // end OnCloseButton()

      bool BuyWindow::OnAccept(const CEGUI::EventArgs& args)
      {
        UpdateOffer();
        return true;
      } // end OnAccept()

      bool BuyWindow::OnScroll(const CEGUI::EventArgs& args)
      {
        using namespace CEGUI;

        const WindowEventArgs& ddea = static_cast<const WindowEventArgs&>(args);

        CEGUI::Scrollbar* scrollbar =
          static_cast<CEGUI::Scrollbar*>(ddea.window);
        float page = scrollbar->getScrollPosition();

        Update((int)page);

        Report(PT::Debug, "Scrolling: page %f", page);

        return true;
      } // end OnScroll()

      void BuyWindow::MoveItem(Slot* oldslot, Slot* newslot)
      {
        unsigned int itemid = oldslot->GetObject()->GetId();

        // Item has been moved to be bought. Add to totalmoney.
        if (oldslot->GetParent() == Inventory::BuyUpper)
        {
          for (size_t i = 0; i < items.GetSize(); i++)
          {
            if (itemid == items.Get(i).itemid)
            {
              Report(PT::Debug, "BuyWindow:: Deleted index for itemid %d\n",
                itemid);
              totalmoney += items.Get(i).price;
              items.DeleteIndex(i);
              break;
            }
          }
        }
        // Item has been moved back, substract from totalmoney.
        else if (oldslot->GetParent() == Inventory::BuyLower)
        {
          Item item;
          item.itemid = oldslot->GetObject()->GetId();
          item.price = oldslot->GetObject()->GetPrice();
          items.Push(item);
          totalmoney -= item.price;
        }

        dragdrop->MoveObject(oldslot, newslot);
        SetTotalMoney(totalmoney);
      } // end MoveItem()

      void BuyWindow::SetTotalMoney(unsigned int price)
      {
        btn = winMgr->getWindow("BuyWindow/Money/TotalMoney");
        btn->setText(dragdrop->IntToStr(price));
      } // end SetTotalMoney()

      void BuyWindow::SetYourMoney(unsigned int price)
      {
        btn = winMgr->getWindow("BuyWindow/Money/YourMoney");
        btn->setText(dragdrop->IntToStr(price));
      } // end SetYourMoney()

      bool BuyWindow::AddItem(unsigned int itemid, const char* name, const char* iconname, unsigned int price)
      {
        winMgr->getWindow("BuyWindow/Frame")->setVisible(true);
        Item item;
        item.itemid = itemid;
        item.name = name;
        item.iconname = iconname;
        item.price = price;
        items.Push(item);
        Update(0);
        return true;
      } // end AddItem()

      void BuyWindow::Update(int linenr)
      {
        int nrInventorySlots = 12;
        linenr = linenr*nrInventorySlots;

        CEGUI::Scrollbar* scrollbar = static_cast<CEGUI::Scrollbar*>
          (winMgr->getWindow("BuyWindow/UpperSlots/UpperBag/scrollbar"));
        scrollbar->setDocumentSize(items.GetSize());

        // Clearing the old items.
        upperslots->ClearSlotsDelete();

        // Putting the items in.
        int counter = 0;
        for (size_t i=linenr; i<items.GetSize(); i++)
        {
          if (counter > nrInventorySlots-1) break;
          Slot* slot = upperslots->GetSlot(counter);
          Item item = items.Get(i);
          slot->SetObject(dragdrop->CreateItem(item.itemid, item.name, item.iconname));
          slot->GetObject()->SetPrice(item.price);
          counter += 1;
        }
      } // end Update()

      void BuyWindow::UpdateOffer()
      {
        // Make a list of items and send it to the network.
        // Get actual items.
        TradeOrderListNpcMessage msg;
        csArray<Inventory::ObjectAndSlot> objandslot =
          lowerslots->GetAllObjects();

        msg.setIsBuy(1);

        // Make the offer list.
        msg.setOrdersCount((unsigned char)objandslot.GetSize());
        Report(PT::Debug, "------------------------------------------");
        Report(PT::Debug, "BuyWindow: Creating Trade Offer List Pvp");
        for (size_t i=0; i<objandslot.GetSize(); i++)
        {
          Inventory::ObjectAndSlot objslot = objandslot.Get(i);
          Report(PT::Debug, "item %d", objslot.object->GetId());
          msg.setItemId(i, objslot.object->GetId());
        }

        network->send(&msg);
        Report(PT::Debug, "SEND");

        Report(PT::Debug, "------------------------------------------");
      } // end UpdateOffer()

      void BuyWindow::AcceptTrade()
      {
        /* TODO
        winMgr->getWindow("BuyWindow/Frame")->setVisible(false);

        int nrInventorySlots = 30;

        // Get all the items.
        csArray<Inventory::ObjectAndSlot> objandslot =
          lowerslots->GetAllObjects();

        // Putting the new items in the inventory.
        int counter = 10;

        InventoryWindow* inventoryWindow =
          guimanager->GetWindow<InventoryWindow>(INVENTORYWINDOW);
        for (size_t i=0; i<objandslot.GetSize(); i++)
        {
          unsigned int objid = objandslot.Get(i).object->GetId();
          const char* name = objandslot.Get(i).object->GetName();
          const char* iconname = objandslot.Get(i).object->GetIconName();
          while (!inventoryWindow->AddItem(objid, name, iconname, counter)
            && counter < nrInventorySlots)
          {
            counter += 1;
          }
        }

        upperslots->ClearSlotsDelete();
        lowerslots->ClearSlotsDelete();
        items.DeleteAll();
        */
      } // end AcceptTrade()

      void BuyWindow::CancelTrade()
      {
        winMgr->getWindow("BuyWindow/Frame")->setVisible(false);

        upperslots->ClearSlotsDelete();
        lowerslots->ClearSlotsDelete();
        items.DeleteAll();

      } // end CancelTrade()

      bool BuyWindow::Create()
      {
        ReloadWindow();
        return true;
      } // end Create()

      bool BuyWindow::ReloadWindow()
      {
        window = GUIWindow::LoadLayout ("client/buy.layout");
        GUIWindow::AddToRoot(window);

        dragdrop = guimanager->GetDragDrop();

        // Get the frame window
        CEGUI::FrameWindow* frame = static_cast<CEGUI::FrameWindow*>
          (winMgr->getWindow("BuyWindow/Frame"));
        frame->subscribeEvent(CEGUI::FrameWindow::EventCloseClicked,
          CEGUI::Event::Subscriber(&BuyWindow::OnCloseButton, this));

        // Get the frame window
        CEGUI::PushButton* accept1 = static_cast<CEGUI::PushButton*>
          (winMgr->getWindow("BuyWindow/Accept"));
        accept1->subscribeEvent(CEGUI::PushButton::EventClicked,
          CEGUI::Event::Subscriber(&BuyWindow::OnAccept, this));

        // Get the frame window
        CEGUI::Scrollbar* scrollbar = static_cast<CEGUI::Scrollbar*>
          (winMgr->getWindow("BuyWindow/UpperSlots/UpperBag/scrollbar"));
        scrollbar->subscribeEvent(CEGUI::Scrollbar::EventScrollPositionChanged,
          CEGUI::Event::Subscriber(&BuyWindow::OnScroll, this));
        scrollbar->setStepSize(1.0);
        scrollbar->setOverlapSize(0.0);
        scrollbar->setPageSize(12);

        // Populate the upper bag with slots.
        CEGUI::Window* bag1 = winMgr->getWindow("BuyWindow/UpperSlots/UpperBag");
        upperslots = new Inventory(guimanager);
        upperslots->Create(bag1, Inventory::BuyUpper, DragDrop::Item, 3, 4);

        // Populate the lower bag with slots.
        CEGUI::Window* bag2 = winMgr->getWindow("BuyWindow/LowerSlots/LowerBag");
        lowerslots = new Inventory(guimanager);
        lowerslots->Create(bag2, Inventory::BuyLower, DragDrop::Item, 2, 4);

        Update(0);
        return true;
      } // end ReloadWindow()

    } // Windows namespace
  } // GUI namespace
} // PT namespace

