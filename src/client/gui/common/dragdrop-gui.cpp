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

#include "client/gui/common/slot.h"
#include "client/gui/common/object.h"

#include "client/serversetup/serversetupmanager.h" //remove

#include "client/data/item/itemdatamanager.h"
#include "client/data/item/item.h"

#include "CEGUI.h"
#include "CEGUIWindowManager.h"
#include "CEGUILogger.h"

#include "client/pointer/pointer.h"
#include "client/network/network.h"

#include "common/eventcs/eventmanager.h"
#include "common/eventcs/interfaceevent.h"

#include "common/reporter/reporter.h"

#include "client/gui/guimanager.h"
#include "client/gui/buy-gui.h"
#include "client/gui/trade-gui.h"

using namespace PT;

namespace PT
{
  namespace GUI
  {
    namespace Windows
    {
      DragDrop::DragDrop(PT::GUI::GUIManager* guimanager)
      {
        this->guimanager = guimanager;
        winMgr = guimanager->GetCEGUI()->GetWindowManagerPtr();
        itemDataManager = PointerLibrary::getInstance()->
          getServerSetupManager()->GetItemDataManager(); //TODO remove
        network = PointerLibrary::getInstance()->getNetwork();

        counter = 0;
      } // end DragDrop()

      DragDrop::~DragDrop()
      {
      } // end ~DragDrop()

      bool DragDrop::handleDragEnter(const CEGUI::EventArgs& args)
      {
        using namespace CEGUI;

        const DragDropEventArgs& ddea =
          static_cast<const DragDropEventArgs&>(args);
        ddea.window->setProperty("FrameColours",
          "tl:FF00FF00 tr:FF00FF00 bl:FF00FF00 br:FF00FF00");

        return true;
      } // end handleDragEnter()

      bool DragDrop::handleDragLeave(const CEGUI::EventArgs& args)
      {
        using namespace CEGUI;

        const DragDropEventArgs& ddea =
          static_cast<const DragDropEventArgs&>(args);
        ddea.window->setProperty("FrameColours",
          "tl:FFFFFFFF tr:FFFFFFFF bl:FFFFFFFF br:FFFFFFFF");

        return true;
      } // end handleDragLeave()

      bool DragDrop::handleDragDropped(const CEGUI::EventArgs& args)
      {
        using namespace CEGUI;

        const DragDropEventArgs& ddea =
          static_cast<const DragDropEventArgs&>(args);
        ddea.window->setProperty("FrameColours",
          "tl:FFFFFFFF tr:FFFFFFFF bl:FFFFFFFF br:FFFFFFFF");

        Slot* oldslot =
          static_cast<Slot*>(ddea.dragDropItem->getParent()->getUserData());
        Slot* newslot = static_cast<Slot*>(ddea.window->getUserData());

        if (!newslot->IsEmpty())
        {
          Object* obj = newslot->GetObject();
          if (obj->GetWindow()->isDisabled())
            return true;
        }

        if (oldslot->GetParent() == Inventory::TradeLeft)
        {
          // Enable the inventory icon again.
          TradeWindow* tradeWindow =
            guimanager->GetWindow<TradeWindow>(TRADEWINDOW);
          Slot* oldinvslot = tradeWindow->GetOldSlot(oldslot);
          oldinvslot->GetObject()->GetWindow()->enable();
          // Destroy the trade icon.
          Object* tradeobj = oldslot->GetObject();
          tradeobj->GetWindow()->destroy();
          delete tradeobj;
          oldslot->Clear();

          if (oldinvslot) oldslot = oldinvslot;
          tradeWindow->UpdateOffer();
        }

        //TODO
        /*
        InventoryMoveItemRequestMessage slotchangemsg;
        slotchangemsg.setNewSlot(newslot->GetId());
        slotchangemsg.setOldSlot(oldslot->GetId());
        if (network) network->send(&slotchangemsg);*/

        return true;
      } // end handleDragDropped()

      bool DragDrop::handleDragDroppedIcon(const CEGUI::EventArgs& args)
      {
        using namespace CEGUI;

        const DragDropEventArgs& ddea =
          static_cast<const DragDropEventArgs&>(args);

        DragDropEventArgs& ddea1 = const_cast<DragDropEventArgs&>(ddea);

        // An icon has been dropped on an icon,
        // relay the event to the parent, the slot.
        ddea1.window = ddea.window->getParent();

        DragDrop::handleDragDropped(ddea1);

        Report(PT::Debug, "DragDrop::handleDragDroppedIcon.");

        return true;
      } // end handleDragDroppedIcon()

      bool DragDrop::handleDragDroppedRoot(const CEGUI::EventArgs& args)
      {
        using namespace CEGUI;

        const DragDropEventArgs& ddea =
          static_cast<const DragDropEventArgs&>(args);
        int objectid = -1;

        Slot* slot =
          static_cast<Slot*>(ddea.dragDropItem->getParent()->getUserData());
        if (!slot) return false;
        objectid = slot->GetObject()->GetId();

        if (objectid < 1) return false;

        //TODO
        /*
        DropRequestMessage msg;
        //msg.setItemId(objectid);
        msg.setSlot(slot->GetId());
        if (network) network->send(&msg);
        */

        return true;
      } // end handleDragDroppedRoot()

      bool DragDrop::handleDragDroppedTrade(const CEGUI::EventArgs& args)
      {
        using namespace CEGUI;

        const DragDropEventArgs& ddea =
          static_cast<const DragDropEventArgs&>(args);
        ddea.window->setProperty("FrameColours",
          "tl:FFFFFFFF tr:FFFFFFFF bl:FFFFFFFF br:FFFFFFFF");

        Slot* oldslot =
          static_cast<Slot*>(ddea.dragDropItem->getParent()->getUserData());
        Slot* newslot = static_cast<Slot*>(ddea.window->getUserData());

        TradeWindow* tradeWindow =
          guimanager->GetWindow<TradeWindow>(TRADEWINDOW);
        tradeWindow->AddItem(oldslot, newslot);

        return true;
      } // end handleDragDroppedTrade()

      bool DragDrop::handleDragDroppedBuy(const CEGUI::EventArgs& args)
      {
        using namespace CEGUI;

        const DragDropEventArgs& ddea =
          static_cast<const DragDropEventArgs&>(args);
        ddea.window->setProperty("FrameColours",
          "tl:FFFFFFFF tr:FFFFFFFF bl:FFFFFFFF br:FFFFFFFF");

        Slot* oldslot =
          static_cast<Slot*>(ddea.dragDropItem->getParent()->getUserData());
        Slot* newslot = static_cast<Slot*>(ddea.window->getUserData());

        if (oldslot->GetParent() == Inventory::BuyLower ||
           oldslot->GetParent() == Inventory::BuyUpper)
        {
          BuyWindow* buyWindow = guimanager->GetWindow<BuyWindow>(BUYWINDOW);
          buyWindow->MoveItem(oldslot, newslot);
        }
        return true;
      } // end handleDragDroppedBuy()

      bool DragDrop::handleRightClickedIcon(const CEGUI::EventArgs& args)
      {
        using namespace CEGUI;

        const MouseEventArgs& mouseArgs =
          static_cast<const MouseEventArgs&>(args);

        // If input is different from RightButton,
        // swallow the event and don't do anything.
        if (mouseArgs.button != RightButton)
        {
          return true;
        }

        Slot* slot =
          static_cast<Slot*>(mouseArgs.window->getParent()->getUserData());
        if (!slot) return false;
        Object* object = slot->GetObject();
        if (!object) return false;

        PT::Events::EventManager* evmgr =
          PointerLibrary::getInstance()->getEventManager();
        csRef<iEvent> interfaceEvent =
          evmgr->CreateEvent("interface.interact");
        interfaceEvent->Add("entityId", slot->GetId());
        interfaceEvent->Add("objectId", object->GetId());

        // @TODO: Use equiptype in items.xml and add itemtype instead of
        // hardcoding here.
        std::string actions = "Drop";

        if (object->GetId() == 1) // apple
        {
          actions += ", Eat";
        }
        if (object->GetId() == 6) // Book
        {
          // @TODO
          actions += ", Read";
          actions += ", Write";
        }

        interfaceEvent->Add("actions", actions.c_str());
        evmgr->AddEvent(interfaceEvent);

        return true;
      } // end handleRightClickedIcon()

      CEGUI::Window* DragDrop::createIcon(int icontype, int objectid,
                     const char* name, const char* iconname, bool interactable)
      {
        char uniquename[1024];
        counter += 1;
        snprintf(uniquename, 1024, "%d_%d_icon", objectid, counter);

        // Create a drag/drop Icon
        CEGUI::Window* icon;
        if (interactable)
          icon = winMgr->createWindow("DragContainer", uniquename);
        else
        {
          icon = winMgr->createWindow("DragContainer", uniquename);
          icon->disable();
        }

        icon->setPosition(CEGUI::UVector2(CEGUI::UDim(0.0f,0.0f),
          CEGUI::UDim(0.0f,0.0f)));
        icon->setSize(CEGUI::UVector2(CEGUI::UDim(0.0f,43.2f),
          CEGUI::UDim(0.0f,43.2f)));
        icon->setHorizontalAlignment(CEGUI::HA_CENTRE);
        icon->setVerticalAlignment(CEGUI::VA_CENTRE);

        // Setup event for swapping.
        icon->subscribeEvent(CEGUI::Window::EventDragDropItemDropped,
          CEGUI::Event::Subscriber(&DragDrop::handleDragDroppedIcon, this));

        // Setup event for interaction
        icon->subscribeEvent(CEGUI::Window::EventMouseButtonDown,
          CEGUI::Event::Subscriber(&DragDrop::handleRightClickedIcon, this));

        // Set a static image as drag container's contents
        CEGUI::Window* iconImage = winMgr->createWindow("Peragro/StaticImage");
        icon->addChildWindow(iconImage);
        iconImage->setPosition(CEGUI::UVector2(CEGUI::UDim(0.0f,0.0f),
          CEGUI::UDim(0.0f,0.0f)));
        iconImage->setSize(CEGUI::UVector2(CEGUI::UDim(1.0f,0.0f),
          CEGUI::UDim(1.0f,0.0f)));
        iconImage->setHorizontalAlignment(CEGUI::HA_CENTRE);
        iconImage->setVerticalAlignment(CEGUI::VA_CENTRE);
        iconImage->setProperty("FrameEnabled", "False");
        // Disable to allow inputs to pass through.
        iconImage->disable();

        // Lets decide what to make of the icon: Item or Skill.
        if (icontype == DragDrop::Item)
        {
          // Set some variables.
          icon->setUserString("itemid" , IntToStr(objectid));
          icon->setUserString("icontype" , IntToStr(icontype));
          icon->setTooltipText(name);

          std::string setAndIcon;
          // An image file is specified.
          if (!strstr(iconname, "set:"))
          {
            CEGUI::ImagesetManager* imgmgr = guimanager->GetCEGUI()->GetImagesetManagerPtr();
            if (!imgmgr->isDefined(iconname))
            {
              ///TODO: This imageset should be released when the item is destroyed.
              imgmgr->createFromImageFile(iconname, iconname);
            }
            setAndIcon = std::string("set:") + iconname;
            setAndIcon += std::string(" image:") + "full_image";
          }
          // A CEGUI imageset is specified.
          else
          {
            setAndIcon = iconname;
          }

          iconImage->setProperty("Image", setAndIcon.c_str());

        }
        else if (icontype == DragDrop::Skill)
        {

        }

        return icon;
      } // end createIcon()

      CEGUI::String DragDrop::IntToStr(int number)
      {
        char buffer[1024];
        snprintf(buffer, 1024, "%d", number);
        CEGUI::String value = (CEGUI::String)buffer;

        return value;
      } // end IntToStr()

      Object* DragDrop::CreateItem(uint itemid,
                   const char* name, const char* iconname, bool interactable)
      {
        Object* object = new Object();
        object->SetId(itemid);
        object->SetName(name);
        object->SetIconName(iconname);
        object->SetWindow(createIcon(DragDrop::Item, itemid, name, iconname, interactable));

        return object;
      } // end CreateItem()

      void DragDrop::MoveObject(Slot* oldslot, Slot* newslot)
      {
        if (!newslot->IsEmpty())
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
      } // end MoveObject()

    } // Windows namespace
  } // GUI namespace
} // PT namespace
