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

#define CANCEL_BUTTON "InteractDialog/Cancel"
#define TRADE_BUTTON "InteractDialog/Trade"
#define ATTACK_BUTTON "InteractDialog/Attack"
#define PARTY_BUTTON "InteractDialog/Party"

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
      button->hide();
  }
}

void InteractDialogWindow::AddAction(const char* action /*,Callback* callback*/)
{
  if (newDialog)
  {
    ClearActions();
    newDialog = false;
  }
}

bool InteractDialogWindow::OnInteract (PT::Events::Eventp ev)
{
  using namespace PT::Events;

  InterfaceInteract* interfaceEv = GetInterfaceEvent<InterfaceInteract*>(ev);
  if (!interfaceEv) return false;

  interactId = interfaceEv->entityId;

  ClearActions();
  rootwindow->setVisible(true);

  int x = PointerLibrary::getInstance()->getCursor()->GetMouseX();
  int y = PointerLibrary::getInstance()->getCursor()->GetMouseY();
  x = x - ( 80 / 2 );
  y = y - ( 80 / 2 );
  rootwindow->setPosition(CEGUI::UVector2(CEGUI::UDim(0,x), CEGUI::UDim(0,y)));
  
  /* TODO
    Behaviours should listen to the interface.interact event
    and on that event register their action(s) with a callback to this
    window which will create a button for each callback.
  */
  if (interfaceEv->Contains("trade"))
    winMgr->getWindow(TRADE_BUTTON)->setVisible(true);
  if (interfaceEv->Contains("attack"))
    winMgr->getWindow(ATTACK_BUTTON)->setVisible(true);
  if (interfaceEv->Contains("party"))
    winMgr->getWindow(PARTY_BUTTON)->setVisible(true);


  return true;
}

void InteractDialogWindow::CreateGUIWindow()
{
  winMgr = cegui->GetWindowManagerPtr ();

  // Create our root window.
  rootwindow = winMgr->createWindow("Peragro/StaticImage", "InteractDialog/Frame");
  rootwindow->setVisible(false);
  rootwindow->setPosition(CEGUI::UVector2(CEGUI::UDim(0.5f,0.0f), CEGUI::UDim(0.5f,0.0f)));
  rootwindow->setSize(CEGUI::UVector2(CEGUI::UDim(0.0f,80.0f), CEGUI::UDim(0.0f,80.0f)));
  rootwindow->setProperty("FrameEnabled", "False");
  rootwindow->setProperty("BackgroundEnabled", "False");
  CEGUI::Window* root = winMgr->getWindow("Root");
  root->addChildWindow(rootwindow);

  // Register listener for interact.
  using namespace PT::Events;
  EventHandler<InteractDialogWindow>* cbInteract = new EventHandler<InteractDialogWindow>(&InteractDialogWindow::OnInteract, this);
  PointerLibrary::getInstance()->getEventManager()->AddListener("interface.interact", cbInteract);

  // Cancel Button
  CEGUI::Window* button = winMgr->createWindow("Peragro/Button", CANCEL_BUTTON);
  button->setPosition(CEGUI::UVector2(CEGUI::UDim(0.5f,-10.0f), CEGUI::UDim(1.0f,-25.0f)));
  button->setSize(CEGUI::UVector2(CEGUI::UDim(0.0f,20.0f), CEGUI::UDim(0.0f,20.0f)));
  button->setProperty("NormalImage", "set:Peragro image:NewCloseButtonNormal");
  button->setProperty("HoverImage", "set:Peragro image:NewCloseButtonNormal");
  button->setProperty("PushedImage", "set:Peragro image:NewCloseButtonNormal");
  button->setProperty("ClippedByParent", "False");
  button->setTooltipText("Cancel");
  rootwindow->addChildWindow(button);
  button->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&InteractDialogWindow::OnCancelButton, this));

  // Trade Button
  button = winMgr->createWindow("Peragro/Button", TRADE_BUTTON);
  button->setVisible(false);
  button->setPosition(CEGUI::UVector2(CEGUI::UDim(0.5f,-10.0f), CEGUI::UDim(0.0f,0.0f)));
  button->setSize(CEGUI::UVector2(CEGUI::UDim(0.0f,20.0f), CEGUI::UDim(0.0f,20.0f)));
  button->setProperty("NormalImage", "set:Peragro image:NewCloseButtonNormal");
  button->setProperty("HoverImage", "set:Peragro image:NewCloseButtonNormal");
  button->setProperty("PushedImage", "set:Peragro image:NewCloseButtonNormal");
  button->setProperty("ClippedByParent", "False");
  button->setTooltipText("Trade");
  rootwindow->addChildWindow(button);
  button->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&InteractDialogWindow::OnAction, this));

  // Attack Button
  button = winMgr->createWindow("Peragro/Button", ATTACK_BUTTON);
  button->setVisible(false);
  button->setPosition(CEGUI::UVector2(CEGUI::UDim(0.0f,0.0f), CEGUI::UDim(0.5f,-10.0f)));
  button->setSize(CEGUI::UVector2(CEGUI::UDim(0.0f,20.0f), CEGUI::UDim(0.0f,20.0f)));
  button->setProperty("NormalImage", "set:Peragro image:NewCloseButtonNormal");
  button->setProperty("HoverImage", "set:Peragro image:NewCloseButtonNormal");
  button->setProperty("PushedImage", "set:Peragro image:NewCloseButtonNormal");
  button->setProperty("ClippedByParent", "False");
  button->setTooltipText("Attack");
  rootwindow->addChildWindow(button);
  button->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&InteractDialogWindow::OnAction, this));

  // Party Button
  button = winMgr->createWindow("Peragro/Button", PARTY_BUTTON);
  button->setVisible(false);
  button->setPosition(CEGUI::UVector2(CEGUI::UDim(1.0f,-20.0f), CEGUI::UDim(0.5f,-10.0f)));
  button->setSize(CEGUI::UVector2(CEGUI::UDim(0.0f,20.0f), CEGUI::UDim(0.0f,20.0f)));
  button->setProperty("NormalImage", "set:Peragro image:NewCloseButtonNormal");
  button->setProperty("HoverImage", "set:Peragro image:NewCloseButtonNormal");
  button->setProperty("PushedImage", "set:Peragro image:NewCloseButtonNormal");
  button->setProperty("ClippedByParent", "False");
  button->setTooltipText("Party");
  rootwindow->addChildWindow(button);
  button->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&InteractDialogWindow::OnAction, this));

}


