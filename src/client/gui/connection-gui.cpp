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

/*====================//
// GUIConnectWindow   //
//====================*/
ConnectWindow::ConnectWindow(GUIManager* guimanager)
: GUIWindow (guimanager)
{
}

ConnectWindow::~ConnectWindow()
{
}

bool ConnectWindow::ConnectButtonPressed(const CEGUI::EventArgs& e) 
{
  // Get the connect window and disable it
  GUIWindow::DisableWindow();

  ConnectRequestMessage msg;
  SocketAddress addr = Socket::getSocketAddress(GetServer(), 12345);
  network->setServerAddress(addr);
  network->send(&msg);

  return true;
}

csString ConnectWindow::GetServer()  
{
  btn = winMgr->getWindow("Server");
  CEGUI::String str = btn->getText();
  csString serverip = str.c_str();
  return serverip;
}

void ConnectWindow::CreateGUIWindow()
{
  GUIWindow::CreateGUIWindow ("connect.xml");

  winMgr = cegui->GetWindowManagerPtr ();

  // Get the root window
  rootwindow = winMgr->getWindow("Connect");

  // Register the button events.
  btn = winMgr->getWindow("Connect_Button");
  btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&ConnectWindow::ConnectButtonPressed, this));
}
