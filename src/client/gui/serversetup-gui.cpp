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

#include "client/gui/guimanager.h"

#include "client/pointer/pointer.h"

#include "client/serversetup/serversetupmanager.h"

ServerSetupWindow::ServerSetupWindow(GUIManager* guimanager)
: GUIWindow (guimanager)
{
}

ServerSetupWindow::~ServerSetupWindow()
{
}

bool ServerSetupWindow::DoneButton(const CEGUI::EventArgs& e) 
{
  btn = winMgr->getWindow("CharSelect/Frame");
  btn->setVisible(true);
  btn = winMgr->getWindow("ServerSetup/Frame");
  btn->setVisible(false);
  return true;
}

bool ServerSetupWindow::UploadButton(const CEGUI::EventArgs& e) 
{
  PointerLibrary::getInstance()->getServerDataManager()->UploadServerData();
  return true;
}

bool ServerSetupWindow::DownloadButton(const CEGUI::EventArgs& e) 
{
  PointerLibrary::getInstance()->getServerDataManager()->DownloadServerData();
  return true;
}

void ServerSetupWindow::CreateGUIWindow()
{
  GUIWindow::CreateGUIWindow ("serversetup.xml");

  winMgr = cegui->GetWindowManagerPtr ();

  // Get the root window
  rootwindow = winMgr->getWindow("CharSelect/Frame");

  // Register the button events.
  btn = winMgr->getWindow("ServerSetup/Done");
  btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&ServerSetupWindow::DoneButton, this));
  btn = winMgr->getWindow("ServerSetup/Upload");
  btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&ServerSetupWindow::UploadButton, this));
  btn = winMgr->getWindow("ServerSetup/Download");
  btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&ServerSetupWindow::DownloadButton, this));

  btn = winMgr->getWindow("ServerSetup/Frame");
  btn->subscribeEvent(CEGUI::FrameWindow::EventCloseClicked, CEGUI::Event::Subscriber(&ServerSetupWindow::DoneButton, this));

}
