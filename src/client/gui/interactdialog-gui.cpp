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
#include "client/cursor/cursor.h"

#include "common/reporter/reporter.h"

#include "common/event/eventmanager.h"
#include "common/event/interfaceevent.h"

#include "client/combat/combatmanager.h"

#define CANCEL_BUTTON "InteractDialog/Cancel"

#define TRADE_BUTTON     "InteractDialog/Trade"
#define PICKUP_BUTTON    "InteractDialog/Pickup"
#define DROP_BUTTON      "InteractDialog/Drop"
#define DOOR_BUTTON      "InteractDialog/Door"
#define LOCK_BUTTON      "InteractDialog/Lock"
#define UNLOCK_BUTTON    "InteractDialog/Unlock"
#define ATTACK_BUTTON    "InteractDialog/Attack"
#define TALK_BUTTON      "InteractDialog/Talk"
#define INVENTORY_BUTTON "InteractDialog/Inventory"
#define STATS_BUTTON     "InteractDialog/Stats"
#define READ_BUTTON      "InteractDialog/Read"
#define WRITE_BUTTON     "InteractDialog/Write"
#define EAT_BUTTON       "InteractDialog/Eat"
#define ACTIVATE_BUTTON  "InteractDialog/Activate"

namespace PT
{
  namespace GUI
  {
    namespace Windows
    {

      InteractDialogWindow::InteractDialogWindow(GUIManager* guimanager)
        : GUIWindow (guimanager)
      {
        windowName = INTERACTDIALOGWINDOW;
        iObjectRegistry* oreg =
          PointerLibrary::getInstance()->getObjectRegistry();
        csRef<iConfigManager> app_cfg (
          csQueryRegistry<iConfigManager> (oreg)
          );

        root_size =
          app_cfg->GetFloat("peragro.icons.interact.rootsize", 70.0f);
        button_size =
          app_cfg->GetFloat("peragro.icons.interact.buttonsize", 50.0f);

        newDialog = true;
        interactId = -1;
      } // end InteractDialogWindow()

      InteractDialogWindow::~InteractDialogWindow()
      {
      } // end ~InteractDialogWindow()

      bool InteractDialogWindow::OnCancelButton(const CEGUI::EventArgs& args)
      {
        GUIWindow::HideWindow();

        return true;
      } // end OnCancelButton()

      bool InteractDialogWindow::OnAction(const CEGUI::EventArgs& args)
      {
        using namespace CEGUI;

        const WindowEventArgs& ddea =
          static_cast<const WindowEventArgs&>(args);

        if (interactId == (unsigned int)-1) return true;

        if (ddea.window->getName().compare(TRADE_BUTTON) == 0)
        {
          Report(PT::Notify, "OnAction: Requesting trade with: %d.",
            interactId);
          TradeRequestMessage msg;
          msg.setEntityId(interactId);
          network->send(&msg);
        }
        else if (ddea.window->getName().compare(PICKUP_BUTTON) == 0)
        {
          InventoryWindow* inventoryWindow =
            guimanager->GetWindow<InventoryWindow>(INVENTORYWINDOW);
          unsigned int slotid = inventoryWindow->FindFreeSlot();
          if (slotid < 30)
          {
            Report(PT::Notify, "OnAction: Requesting picking up: %d for slot %d.",
              interactId, slotid);
            PickRequestMessage msg;
            msg.setItemEntityId(interactId);
            msg.setSlot(slotid); // TODO: get a free slot for this!
            network->send(&msg);
          }
        }
        else if (ddea.window->getName().compare(DROP_BUTTON) == 0)
        {
          if (interactId < 30) // slot id
          {
            Report(PT::Notify, "OnAction: Requesting dropping slot %d.",
              interactId);
            DropRequestMessage msg;
            msg.setSlot(interactId);
            network->send(&msg);
          }
        }
        else if (ddea.window->getName().compare(DOOR_BUTTON) == 0)
        {
          PT::Entity::Entity* ent = PointerLibrary::getInstance()->
            getEntityManager()->findPtEntById(interactId);
          if (!ent) return true;
          if (ent->GetType() == 2)// door
          {
            if (((PT::Entity::DoorEntity*)ent)->GetOpen())
            {
              Report(PT::Notify, "OnAction: Requesting open of: %d.",
                interactId);
              CloseDoorRequestMessage msg;
              msg.setDoorId(interactId);
              network->send(&msg);
            }
            else
            {
              Report(PT::Notify, "OnAction: Requesting close of: %d.",
                interactId);
              OpenDoorRequestMessage msg;
              msg.setDoorId(interactId);
              network->send(&msg);
            }
          }
        }
        else if (ddea.window->getName().compare(LOCK_BUTTON) == 0)
        {
          Report(PT::Notify, "OnAction: Requesting lock of: %d.",
            interactId);
          LockDoorRequestMessage msg;
          msg.setDoorId(interactId);
          network->send(&msg);
        }
        else if (ddea.window->getName().compare(UNLOCK_BUTTON) == 0)
        {
          Report(PT::Notify, "OnAction: Requesting unlock of: %d.",
            interactId);
          UnlockDoorRequestMessage msg;
          msg.setDoorId(interactId);
          network->send(&msg);
        }
        else if (ddea.window->getName().compare(ATTACK_BUTTON) == 0)
        {
          Report(PT::Notify, "OnAction: Requesting attack with: %d.",
            interactId);

          PT::Combat::CombatManager* combatmanager =
            PointerLibrary::getInstance()->getCombatManager();
          HUDWindow* hudWindow = guimanager->GetWindow<HUDWindow>(HUDWINDOW);
          combatmanager->
            RequestSkillUsageStart(interactId, hudWindow->GetActiveSkillId());
        }
        else if (ddea.window->getName().compare(TALK_BUTTON) == 0)
        {
          Report(PT::Notify, "OnAction: Requesting dialog with: %d.",
            interactId);
          NpcStartDialogMessage msg;
          msg.setNpcId(interactId);
          network->send(&msg);
        }
        else if (ddea.window->getName().compare(INVENTORY_BUTTON) == 0)
        {
          InventoryWindow* inventoryWindow =
            guimanager->GetWindow<InventoryWindow>(INVENTORYWINDOW);
          inventoryWindow->ShowWindow();
        }
        else if (ddea.window->getName().compare(STATS_BUTTON) == 0)
        {
          StatusWindow* statusWindow =
            guimanager->GetWindow<StatusWindow>(STATUSWINDOW);
          statusWindow->ShowWindow();
        }
        else if (ddea.window->getName().compare(READ_BUTTON) == 0)
        {
          BookReadRequestMessage msg;
          msg.setItemId(itemId);
          msg.setBookId(variationId);
          network->send(&msg);
          BookWindow* bookWindow =
            guimanager->GetWindow<BookWindow>(BOOKWINDOW);
          bookWindow->SetBook(itemId, interactId);
        }
        else if (ddea.window->getName().compare(WRITE_BUTTON) == 0)
        {
          Report(PT::Debug, "Opening book to write.");
          //guimanager->GetBookWindow()->ShowWindow();
          // Display Book Writing GUI
        }
        else if (ddea.window->getName().compare(EAT_BUTTON) == 0)
        {
          // Send UseMessage to server... not implemented
        }
        else if (ddea.window->getName().compare(ACTIVATE_BUTTON) == 0)
        {
          // Send UseMessage to server... not implemented
        }


        GUIWindow::HideWindow();

        return true;
      } // end OnAction()

      void InteractDialogWindow::ClearActions()
      {
        // Clear the previous actions.
        for (size_t i = 0; i < window->getChildCount(); i++)
        {
          CEGUI::Window* button = window->getChildAtIdx(i);
          if (!button) continue;
          if (button->getName().compare(CANCEL_BUTTON) != 0)
          {
            window->removeChildWindow(button);
            winMgr->destroyWindow(button);
          }
        }
      } // end ClearActions()

      void InteractDialogWindow::LayoutIcons()
      {
        for (size_t i = 0; i < window->getChildCount(); i++)
        {
          CEGUI::Window* button = window->getChildAtIdx(i);
          if (!button) continue;
          float in = (float)i+1;
          float all = (float)window->getChildCount();
          float offset = (PI*2)/all;
          float y = (cos(((in/all)*PI*2)-offset)+1)/2;
          float x = (sin(((in/all)*PI*2)-offset)+1)/2;
          x = x * root_size;
          y = y * root_size;
          x -= button_size/2;
          y -= button_size/2;
          Report(PT::Notify, "COORDS: %f(%f)   %f   %f", in, all, x, y);
          button->setPosition(CEGUI::UVector2(CEGUI::UDim(0.0f,x),
            CEGUI::UDim(0.0f,y)));
        }
      } // end LayoutIcons()

      void InteractDialogWindow::AddAction(const char* action /*,Callback* callback*/)
      {
        if (newDialog)
        {
          ClearActions();
          newDialog = false;
        }

        CEGUI::String name, normal, hover;
        name = "InteractDialog/";
        name += action;

        normal = "set:InteractionIcons image:";
        size_t x = normal.length();
        normal += action;
        normal[x] += 'a' - 'A';
        hover = normal;
        normal += "_normal";
        hover += "_hover";

        if (!winMgr->isWindowPresent(name))
        {
          CEGUI::Window* button =
            winMgr->createWindow("Peragro/ImageButton", name);
          button->setPosition(CEGUI::UVector2(CEGUI::UDim(0.5f,-button_size/2),
            CEGUI::UDim(1.0f,-button_size)));
          button->setSize(CEGUI::UVector2(CEGUI::UDim(0.0f,button_size),
            CEGUI::UDim(0.0f,button_size)));
          button->setProperty("NormalImage", normal);
          button->setProperty("HoverImage", hover);
          button->setProperty("PushedImage", normal);
          button->setProperty("DisabledImage", normal);
          button->setProperty("ClippedByParent", "False");
          button->setTooltipText(action);
          window->addChildWindow(button);
          button->subscribeEvent(CEGUI::PushButton::EventClicked,
            CEGUI::Event::Subscriber(&InteractDialogWindow::OnAction, this));
        }

        LayoutIcons();
      } // end AddAction()

      bool InteractDialogWindow::OnInteract(iEvent& ev)
      {
        using namespace PT::Events;

        if (ev.Retrieve("entityId", interactId))
          return Report(PT::Error, "InteractDialogWindow::OnInteract failed!");

        ev.Retrieve("objectId", itemId);
        ev.Retrieve("variationId", variationId);

        newDialog = true;
        ClearActions();
        ShowWindow();

        int x = PointerLibrary::getInstance()->getCursor()->GetMouseX();
        int y = PointerLibrary::getInstance()->getCursor()->GetMouseY();
        x = (int)(x - ( root_size / 2 ));
        y = (int)(y - ( root_size / 2 ));
        window->setPosition(CEGUI::UVector2(CEGUI::UDim(0,x),
          CEGUI::UDim(0,y)));

        /* @TODO
        Behaviours should listen to the interface.interact event
        and on that event register their action(s) with a callback to this
        window which will create a button for each callback.
        */
        using namespace PT::Events;
        if (InterfaceHelper::ContainsAction(&ev, "Trade"))
          AddAction("Trade");
        if (InterfaceHelper::ContainsAction(&ev, "Pickup"))
          AddAction("Pickup");
        if (InterfaceHelper::ContainsAction(&ev, "Drop"))
          AddAction("Drop");
        if (InterfaceHelper::ContainsAction(&ev, "Door"))
          AddAction("Door");
        if (InterfaceHelper::ContainsAction(&ev, "Lock"))
          AddAction("Lock");
        if (InterfaceHelper::ContainsAction(&ev, "Unlock"))
          AddAction("Unlock");
        if (InterfaceHelper::ContainsAction(&ev, "Attack"))
          AddAction("Attack");
        if (InterfaceHelper::ContainsAction(&ev, "Talk"))
          AddAction("Talk");
        if (InterfaceHelper::ContainsAction(&ev, "Inventory"))
          AddAction("Inventory");
        if (InterfaceHelper::ContainsAction(&ev, "Stats"))
          AddAction("Stats");
        if (InterfaceHelper::ContainsAction(&ev, "Read"))
          AddAction("Read");
        if (InterfaceHelper::ContainsAction(&ev, "Write"))
          AddAction("Write");
        if (InterfaceHelper::ContainsAction(&ev, "Eat"))
          AddAction("Eat");
        if (InterfaceHelper::ContainsAction(&ev, "Activate"))
          AddAction("Activate");

        return true;
      } // end OnInteract()

      bool InteractDialogWindow::Create()
      {
        winMgr = cegui->GetWindowManagerPtr ();

        window = winMgr->createWindow("StaticImage", "InteractDialog/Frame");
        GUIWindow::AddToRoot(window);
        window->setVisible(false);
        window->setPosition(CEGUI::UVector2(CEGUI::UDim(0.5f,0.0f),
          CEGUI::UDim(0.5f,0.0f)));
        window->setSize(CEGUI::UVector2(CEGUI::UDim(0.0f,root_size),
          CEGUI::UDim(0.0f,root_size)));
        window->setProperty("FrameEnabled", "False");
        window->setProperty("BackgroundEnabled", "False");

        // Register listener for interact.
        SETUP_HANDLER
        REGISTER_LISTENER(InteractDialogWindow, OnInteract, "interface.interact")

        // Cancel Button
        CEGUI::Window* button = winMgr->createWindow("Peragro/ImageButton",
          CANCEL_BUTTON);
        button->setPosition(CEGUI::UVector2(CEGUI::UDim(0.5f,-button_size/2),
          CEGUI::UDim(1.0f,-button_size)));
        button->setSize(CEGUI::UVector2(CEGUI::UDim(0.0f,button_size),
          CEGUI::UDim(0.0f,button_size)));
        button->setProperty("NormalImage",
          "set:InteractionIcons image:cancel_normal");
        button->setProperty("HoverImage",
          "set:InteractionIcons image:cancel_hover");
        button->setProperty("PushedImage",
          "set:InteractionIcons image:cancel_normal");
        button->setProperty("DisabledImage",
          "set:InteractionIcons image:cancel_normal");
        button->setProperty("ClippedByParent", "False");
        button->setTooltipText("Cancel");
        window->addChildWindow(button);
        button->subscribeEvent(CEGUI::PushButton::EventClicked,
          CEGUI::Event::Subscriber(&InteractDialogWindow::OnCancelButton, this));
        /*
        // Trade Button
        button = winMgr->createWindow("Peragro/ImageButton", TRADE_BUTTON);
        button->setVisible(false);
        button->setPosition(CEGUI::UVector2(CEGUI::UDim(0.5f,-BUTTON_SIZE/2),
          CEGUI::UDim(0.0f,0.0f)));
        button->setSize(CEGUI::UVector2(CEGUI::UDim(0.0f,BUTTON_SIZE),
          CEGUI::UDim(0.0f,BUTTON_SIZE)));
        button->setProperty("NormalImage",
          "set:InteractionIcons image:trade_normal");
        button->setProperty("HoverImage",
          "set:InteractionIcons image:trade_hover");
        button->setProperty("PushedImage",
          "set:InteractionIcons image:trade_normal");
        button->setProperty("ClippedByParent", "False");
        button->setTooltipText("Trade");
        rootwindow->addChildWindow(button);
        button->subscribeEvent(CEGUI::PushButton::EventClicked,
          CEGUI::Event::Subscriber(&InteractDialogWindow::OnAction, this));

        // Attack Button
        button = winMgr->createWindow("Peragro/ImageButton", ATTACK_BUTTON);
        button->setVisible(false);
        button->setPosition(CEGUI::UVector2(CEGUI::UDim(0.0f,0.0f),
          CEGUI::UDim(0.5f,-BUTTON_SIZE/2)));
        button->setSize(CEGUI::UVector2(CEGUI::UDim(0.0f,BUTTON_SIZE),
          CEGUI::UDim(0.0f,BUTTON_SIZE)));
        button->setProperty("NormalImage",
          "set:InteractionIcons image:attack_normal");
        button->setProperty("HoverImage",
          "set:InteractionIcons image:attack_hover");
        button->setProperty("PushedImage",
          "set:InteractionIcons image:attack_normal");
        button->setProperty("ClippedByParent", "False");
        button->setTooltipText("Attack");
        rootwindow->addChildWindow(button);
        button->subscribeEvent(CEGUI::PushButton::EventClicked,
          CEGUI::Event::Subscriber(&InteractDialogWindow::OnAction, this));

        // Talk Button
        button = winMgr->createWindow("Peragro/ImageButton", TALK_BUTTON);
        button->setVisible(false);
        button->setPosition(CEGUI::UVector2(CEGUI::UDim(1.0f,-BUTTON_SIZE),
          CEGUI::UDim(0.5f,-BUTTON_SIZE/2)));
        button->setSize(CEGUI::UVector2(CEGUI::UDim(0.0f,BUTTON_SIZE),
          CEGUI::UDim(0.0f,BUTTON_SIZE)));
        button->setProperty("NormalImage",
          "set:InteractionIcons image:talk_normal");
        button->setProperty("HoverImage",
          "set:InteractionIcons image:talk_hover");
        button->setProperty("PushedImage",
          "set:InteractionIcons image:talk_normal");
        button->setProperty("ClippedByParent", "False");
        button->setTooltipText("Talk");
        rootwindow->addChildWindow(button);
        button->subscribeEvent(CEGUI::PushButton::EventClicked,
          CEGUI::Event::Subscriber(&InteractDialogWindow::OnAction, this));
        */

        return true;
      } // end Create()

    } // Windows namespace
  } // GUI namespace
} // PT namespace

