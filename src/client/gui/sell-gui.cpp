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

#include "client/gui/gui.h"

#include "CEGUI.h"
#include "CEGUIWindowManager.h"
#include "CEGUILogger.h"

#include "client/network/network.h"
#include "client/gui/guimanager.h"

#include "common/reporter/reporter.h"

namespace PT
{
  namespace GUI
  {
    namespace Windows
    {
      SellWindow::SellWindow(GUIManager* guimanager)
        : GUIWindow (guimanager)
      {
         windowName = SELLWINDOW;
      } // end SellWindow()

      SellWindow::~SellWindow()
      {
      } // end ~SellWindow()

      bool SellWindow::OnCloseButton(const CEGUI::EventArgs& args)
      {
        return true;
      } // end OnCloseButton()

      bool SellWindow::OnAccept(const CEGUI::EventArgs& args)
      {
        return true;
      } // end OnAccept()

      bool SellWindow::AddItem(unsigned int itemid, unsigned int slotid, const char* name, const char* iconname)
      {
        if (slotid > 12) return false;

        Slot* slot = upperslots[slotid];

        if (!slot)
        {
          Report(PT::Error, "SellWindow: Couldn't add item %d in slot %d!",
            itemid, slotid);
          return false;
        }

        slot->SetObject(dragdrop->CreateItem(itemid, 0, name, iconname));

        return true;
      } // end AddItem()

      void SellWindow::AcceptTrade()
      {
        winMgr->getWindow("SellWindow/Frame")->setVisible(false);

        int nrInventorySlots = 30;

        // Putting the new items in the inventory.
        int counter = 10;
        InventoryWindow* inventoryWindow =
          guimanager->GetWindow<InventoryWindow>(INVENTORYWINDOW);

        for (size_t i=0; i<lowerslots.GetSize(); i++)
        {
          Slot* slot = lowerslots[i];
          if (!slot->IsEmpty())
          {
            Object* object = slot->GetObject();

            while (!inventoryWindow->
              AddItem(object->GetId(), object->GetVariationId(), object->GetName(), object->GetIconName(), counter)
              && counter < nrInventorySlots)
            {
              counter += 1;
            }
          }
        }
      } // end AcceptTrade()

      bool SellWindow::Create()
      {
        ReloadWindow();
        return true;
      } // end Create()

      bool SellWindow::ReloadWindow()
      {
        window = GUIWindow::LoadLayout ("client/sell.xml");
        GUIWindow::AddToRoot(window);
        winMgr = cegui->GetWindowManagerPtr ();

        dragdrop = guimanager->GetDragDrop();

        // Get the frame window
        CEGUI::FrameWindow* frame = static_cast<CEGUI::FrameWindow*>
          (winMgr->getWindow("SellWindow/Frame"));
        frame->subscribeEvent(CEGUI::FrameWindow::EventCloseClicked,
          CEGUI::Event::Subscriber(&SellWindow::OnCloseButton, this));

        // Get the frame window
        CEGUI::PushButton* accept1 = static_cast<CEGUI::PushButton*>
          (winMgr->getWindow("SellWindow/Accept"));
        accept1->subscribeEvent(CEGUI::PushButton::EventClicked,
          CEGUI::Event::Subscriber(&SellWindow::OnAccept, this));

        // Populate the Player1 bag with slots.
        //CEGUI::Window* bag1 =
        //  winMgr->getWindow("SellWindow/UpperSlots/UpperBag");
        //dragdrop->CreateBag(bag1, &upperslots,
        //  Inventory::SellUpper, DragDrop::Item, 3, 4);

        // Populate the lower bag with slots.
        //CEGUI::Window* bag2 =
        //  winMgr->getWindow("SellWindow/LowerSlots/LowerBag");
        //dragdrop->CreateBag(bag2, &lowerslots,
        //  Inventory::SellLower, DragDrop::Item, 2, 4);

        return true;
      } // end ReloadWindow()

    } // Windows namespace
  } // GUI namespace
} // PT namespace

