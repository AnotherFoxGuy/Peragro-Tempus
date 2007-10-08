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
#include "client/cursor.h"

#include "client/reporter/reporter.h"

#include "client/event/eventmanager.h"
#include "client/event/interfaceevent.h"

#include "client/combat/combatmanager.h"

#define CANCEL_BUTTON "InteractDialog/Cancel"

#define TRADE_BUTTON  "InteractDialog/Trade"
#define PICKUP_BUTTON "InteractDialog/Pickup"
#define DOOR_BUTTON   "InteractDialog/Door"
#define LOCK_BUTTON   "InteractDialog/Lock"
#define UNLOCK_BUTTON "InteractDialog/Unlock"
#define ATTACK_BUTTON "InteractDialog/Attack"
#define TALK_BUTTON   "InteractDialog/Talk"


#define BUTTON_SIZE 50.0f
#define ROOT_SIZE 70.0f

InteractDialogWindow::InteractDialogWindow(GUIManager* guimanager)
: GUIWindow (guimanager)
{
  newDialog = true;
  interactId = -1;
}

InteractDialogWindow::~InteractDialogWindow()
{
}

bool InteractDialogWindow::OnCancelButton(const CEGUI::EventArgs& args)
{
  GUIWindow::HideWindow();

  return true;
}

bool InteractDialogWindow::OnAction(const CEGUI::EventArgs& args)
{
  using namespace CEGUI;

  const WindowEventArgs& ddea = static_cast<const WindowEventArgs&>(args);

  if (interactId == -1) return true;

  if (ddea.window->getName().compare(TRADE_BUTTON) == 0)
  {
    Report(PT::Notify, "OnAction: Requesting trade with: %d.", interactId);
    TradeRequestMessage msg;
    msg.setEntityId(interactId);
    network->send(&msg);
  }
  else if (ddea.window->getName().compare(PICKUP_BUTTON) == 0)
  {
    unsigned int slotid = guimanager->GetInventoryWindow()->FindFreeSlot();
    if(slotid < 30)
    {
      Report(PT::Notify, "OnAction: Requesting picking up: %d for slot %d.", interactId, slotid);
      PickRequestMessage msg;
      msg.setItemEntityId(interactId);
      msg.setSlot(slotid); // TODO: get a free slot for this!
      network->send(&msg);
    }
  }
  else if (ddea.window->getName().compare(DOOR_BUTTON) == 0)
  {
    PT::Entity::Entity* ent = PointerLibrary::getInstance()->getEntityManager()->findPtEntById(interactId);
    if (!ent) return true;
    if (ent->GetType() == 2)// door
    {
      if (((PT::Entity::DoorEntity*)ent)->GetOpen())
      {
        Report(PT::Notify, "OnAction: Requesting open of: %d.", interactId);
        CloseDoorRequestMessage msg;
        msg.setDoorId(interactId);
        network->send(&msg);
      }
      else
      {
        Report(PT::Notify, "OnAction: Requesting close of: %d.", interactId);
        OpenDoorRequestMessage msg;
        msg.setDoorId(interactId);
        network->send(&msg);
      }
    }
  }
  else if (ddea.window->getName().compare(LOCK_BUTTON) == 0)
  {
    Report(PT::Notify, "OnAction: Requesting lock of: %d.", interactId);
    LockDoorRequestMessage msg;
    msg.setDoorId(interactId);
    network->send(&msg);
  }
  else if (ddea.window->getName().compare(UNLOCK_BUTTON) == 0)
  {
    Report(PT::Notify, "OnAction: Requesting unlock of: %d.", interactId);
    UnlockDoorRequestMessage msg;
    msg.setDoorId(interactId);
    network->send(&msg);
  }
  else if (ddea.window->getName().compare(ATTACK_BUTTON) == 0)
  {
    Report(PT::Notify, "OnAction: Requesting attack with: %d.", interactId);

    CombatMGR* combatmanager = PointerLibrary::getInstance()->getCombatManager();

    combatmanager->RequestSkillUsageStart (interactId, guimanager->GetHUDWindow()->GetActiveSkillId());
  }
  else if (ddea.window->getName().compare(TALK_BUTTON) == 0)
  {
    Report(PT::Notify, "OnAction: Requesting dialog with: %d.", interactId);
    NpcStartDialogMessage msg;
    msg.setNpcId(interactId);
    network->send(&msg);
  }


  GUIWindow::HideWindow();

  return true;
}

void InteractDialogWindow::ClearActions()
{
  // Clear the previous actions.
  for (size_t i = 0; i < rootwindow->getChildCount(); i++)
  {
    CEGUI::Window* button = rootwindow->getChildAtIdx(i);
    if (!button) continue;
    if (button->getName().compare(CANCEL_BUTTON) != 0)
    {
      rootwindow->removeChildWindow(button);
      winMgr->destroyWindow(button);
    }
  }
}

void InteractDialogWindow::LayoutIcons()
{
  // Clear the previous actions.
  for (size_t i = 0; i < rootwindow->getChildCount(); i++)
  {
    CEGUI::Window* button = rootwindow->getChildAtIdx(i);
    if (!button) continue;
    float in = (float)i+1;
    float all = (float)rootwindow->getChildCount();
    float offset = (PI*2)/all;
    float y = (cos(((in/all)*PI*2)-offset)+1)/2;
    float x = (sin(((in/all)*PI*2)-offset)+1)/2;
    x = x * ROOT_SIZE;
    y = y * ROOT_SIZE;
    x -= BUTTON_SIZE/2;
    y -= BUTTON_SIZE/2;
    Report(PT::Notify, "COORDS: %f(%f)   %f   %f", in, all, x, y);
    button->setPosition(CEGUI::UVector2(CEGUI::UDim(0.0f,x), CEGUI::UDim(0.0f,y)));
  }
}

void InteractDialogWindow::AddAction(const char* action /*,Callback* callback*/)
{
  if (newDialog)
  {
    ClearActions();
    newDialog = false;
  }

  CEGUI::String name, normal, hover;
  if (strcmp(action, "Trade") == 0)
  {
    name = TRADE_BUTTON;
    normal = "set:InteractionIcons image:trade_normal";
    hover = "set:InteractionIcons image:trade_hover";
  }
  else if (strcmp(action, "Pickup") == 0)
  {
    name = PICKUP_BUTTON;
    normal = "set:InteractionIcons image:pickup_normal";
    hover = "set:InteractionIcons image:pickup_hover";
  }
  else if (strcmp(action, "Door") == 0)
  {
    name = DOOR_BUTTON;
    normal = "set:InteractionIcons image:door_normal";
    hover = "set:InteractionIcons image:door_hover";
  }
  else if (strcmp(action, "Lock") == 0)
  {
    name = LOCK_BUTTON;
    normal = "set:InteractionIcons image:lock_normal";
    hover = "set:InteractionIcons image:lock_hover";
  }
  else if (strcmp(action, "Unlock") == 0)
  {
    name = UNLOCK_BUTTON;
    normal = "set:InteractionIcons image:unlock_normal";
    hover = "set:InteractionIcons image:unlock_hover";
  }
  else if (strcmp(action, "Attack") == 0)
  {
    name = ATTACK_BUTTON;
    normal = "set:InteractionIcons image:attack_normal";
    hover = "set:InteractionIcons image:attack_hover";
  }
  else if (strcmp(action, "Talk") == 0)
  {
    name = TALK_BUTTON;
    normal = "set:InteractionIcons image:talk_normal";
    hover = "set:InteractionIcons image:talk_hover";
  }

  if (!winMgr->isWindowPresent(name))
  {
    CEGUI::Window* button = winMgr->createWindow("Peragro/ImageButton", name);
    button->setPosition(CEGUI::UVector2(CEGUI::UDim(0.5f,-BUTTON_SIZE/2), CEGUI::UDim(1.0f,-BUTTON_SIZE)));
    button->setSize(CEGUI::UVector2(CEGUI::UDim(0.0f,BUTTON_SIZE), CEGUI::UDim(0.0f,BUTTON_SIZE)));
    button->setProperty("NormalImage", normal);
    button->setProperty("HoverImage", hover);
    button->setProperty("PushedImage", normal);
    button->setProperty("DisabledImage", normal);
    button->setProperty("ClippedByParent", "False");
    button->setTooltipText(action);
    rootwindow->addChildWindow(button);
    button->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&InteractDialogWindow::OnAction, this));
  }

  LayoutIcons();
}

bool InteractDialogWindow::OnInteract (PT::Events::Eventp ev)
{
  using namespace PT::Events;

  InterfaceInteract* interfaceEv = GetInterfaceEvent<InterfaceInteract*>(ev);
  if (!interfaceEv) return false;

  interactId = interfaceEv->entityId;

  newDialog = true;
  ClearActions();
  rootwindow->setVisible(true);

  int x = PointerLibrary::getInstance()->getCursor()->GetMouseX();
  int y = PointerLibrary::getInstance()->getCursor()->GetMouseY();
  x = (int)(x - ( ROOT_SIZE / 2 ));
  y = (int)(y - ( ROOT_SIZE / 2 ));
  rootwindow->setPosition(CEGUI::UVector2(CEGUI::UDim(0,x), CEGUI::UDim(0,y)));

  /* TODO
    Behaviours should listen to the interface.interact event
    and on that event register their action(s) with a callback to this
    window which will create a button for each callback.
  */
  if (interfaceEv->Contains("Trade"))
    AddAction("Trade");
  if (interfaceEv->Contains("Pickup"))
    AddAction("Pickup");
  if (interfaceEv->Contains("Door"))
    AddAction("Door");
  if (interfaceEv->Contains("Lock"))
    AddAction("Lock");
  if (interfaceEv->Contains("Unlock"))
    AddAction("Unlock");
  if (interfaceEv->Contains("Attack"))
    AddAction("Attack");
  if (interfaceEv->Contains("Talk"))
    AddAction("Talk");



  return true;
}

void InteractDialogWindow::CreateGUIWindow()
{
  winMgr = cegui->GetWindowManagerPtr ();

  // Create our root window.
  rootwindow = winMgr->createWindow("Peragro/StaticImage", "InteractDialog/Frame");
  rootwindow->setVisible(false);
  rootwindow->setPosition(CEGUI::UVector2(CEGUI::UDim(0.5f,0.0f), CEGUI::UDim(0.5f,0.0f)));
  rootwindow->setSize(CEGUI::UVector2(CEGUI::UDim(0.0f,ROOT_SIZE), CEGUI::UDim(0.0f,ROOT_SIZE)));
  rootwindow->setProperty("FrameEnabled", "False");
  rootwindow->setProperty("BackgroundEnabled", "False");
  CEGUI::Window* root = winMgr->getWindow("Root");
  root->addChildWindow(rootwindow);

  // Register listener for interact.
  using namespace PT::Events;
  EventHandler<InteractDialogWindow>* cbInteract = new EventHandler<InteractDialogWindow>(&InteractDialogWindow::OnInteract, this);
  PointerLibrary::getInstance()->getEventManager()->AddListener("interface.interact", cbInteract);

  // Cancel Button
  CEGUI::Window* button = winMgr->createWindow("Peragro/ImageButton", CANCEL_BUTTON);
  button->setPosition(CEGUI::UVector2(CEGUI::UDim(0.5f,-BUTTON_SIZE/2), CEGUI::UDim(1.0f,-BUTTON_SIZE)));
  button->setSize(CEGUI::UVector2(CEGUI::UDim(0.0f,BUTTON_SIZE), CEGUI::UDim(0.0f,BUTTON_SIZE)));
  button->setProperty("NormalImage", "set:InteractionIcons image:cancel_normal");
  button->setProperty("HoverImage", "set:InteractionIcons image:cancel_hover");
  button->setProperty("PushedImage", "set:InteractionIcons image:cancel_normal");
  button->setProperty("DisabledImage", "set:InteractionIcons image:cancel_normal");
  button->setProperty("ClippedByParent", "False");
  button->setTooltipText("Cancel");
  rootwindow->addChildWindow(button);
  button->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&InteractDialogWindow::OnCancelButton, this));
/*
  // Trade Button
  button = winMgr->createWindow("Peragro/ImageButton", TRADE_BUTTON);
  button->setVisible(false);
  button->setPosition(CEGUI::UVector2(CEGUI::UDim(0.5f,-BUTTON_SIZE/2), CEGUI::UDim(0.0f,0.0f)));
  button->setSize(CEGUI::UVector2(CEGUI::UDim(0.0f,BUTTON_SIZE), CEGUI::UDim(0.0f,BUTTON_SIZE)));
  button->setProperty("NormalImage", "set:InteractionIcons image:trade_normal");
  button->setProperty("HoverImage", "set:InteractionIcons image:trade_hover");
  button->setProperty("PushedImage", "set:InteractionIcons image:trade_normal");
  button->setProperty("ClippedByParent", "False");
  button->setTooltipText("Trade");
  rootwindow->addChildWindow(button);
  button->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&InteractDialogWindow::OnAction, this));

  // Attack Button
  button = winMgr->createWindow("Peragro/ImageButton", ATTACK_BUTTON);
  button->setVisible(false);
  button->setPosition(CEGUI::UVector2(CEGUI::UDim(0.0f,0.0f), CEGUI::UDim(0.5f,-BUTTON_SIZE/2)));
  button->setSize(CEGUI::UVector2(CEGUI::UDim(0.0f,BUTTON_SIZE), CEGUI::UDim(0.0f,BUTTON_SIZE)));
  button->setProperty("NormalImage", "set:InteractionIcons image:attack_normal");
  button->setProperty("HoverImage", "set:InteractionIcons image:attack_hover");
  button->setProperty("PushedImage", "set:InteractionIcons image:attack_normal");
  button->setProperty("ClippedByParent", "False");
  button->setTooltipText("Attack");
  rootwindow->addChildWindow(button);
  button->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&InteractDialogWindow::OnAction, this));

  // Talk Button
  button = winMgr->createWindow("Peragro/ImageButton", TALK_BUTTON);
  button->setVisible(false);
  button->setPosition(CEGUI::UVector2(CEGUI::UDim(1.0f,-BUTTON_SIZE), CEGUI::UDim(0.5f,-BUTTON_SIZE/2)));
  button->setSize(CEGUI::UVector2(CEGUI::UDim(0.0f,BUTTON_SIZE), CEGUI::UDim(0.0f,BUTTON_SIZE)));
  button->setProperty("NormalImage", "set:InteractionIcons image:talk_normal");
  button->setProperty("HoverImage", "set:InteractionIcons image:talk_hover");
  button->setProperty("PushedImage", "set:InteractionIcons image:talk_normal");
  button->setProperty("ClippedByParent", "False");
  button->setTooltipText("Talk");
  rootwindow->addChildWindow(button);
  button->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&InteractDialogWindow::OnAction, this));
*/
}


