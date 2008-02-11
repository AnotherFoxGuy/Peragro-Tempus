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

#include "gui.h"

#include "CEGUI.h"
#include "CEGUIWindowManager.h" 
#include "CEGUILogger.h"

#include "client/network/network.h"
#include "client/gui/guimanager.h"

GUIWindow::GUIWindow(GUIManager* guimanager)
: guimanager(guimanager)
{
  iObjectRegistry* obj_reg = PointerLibrary::getInstance()->getObjectRegistry();
  vfs = csQueryRegistry<iVFS>(obj_reg);
  cegui = guimanager->GetCEGUI ();
  network = PointerLibrary::getInstance()->getNetwork();
  winMgr = cegui->GetWindowManagerPtr ();
}
GUIWindow::~GUIWindow()
{
}

void GUIWindow::CreateGUIWindow(const char* layoutFile)
{
  //cegui->GetSystemPtr ()->setGUISheet(LoadLayout (layoutFile));
  CEGUI::Window* root = winMgr->getWindow("Root");
  root->addChildWindow(LoadLayout (layoutFile));
}

CEGUI::Window* GUIWindow::LoadLayout(const char* layoutFile)
{
  // Load layout and set as root
  vfs->ChDir ("/peragro/gui/client/");
  return winMgr->loadWindowLayout(layoutFile);
}

void GUIWindow::HideWindow()  
{
  rootwindow->setVisible(false);
}

void GUIWindow::ShowWindow() 
{
  rootwindow->setVisible(true);
}

bool GUIWindow::IsVisible()
{
  return rootwindow->isVisible();
}

void GUIWindow::DisableWindow() 
{
  rootwindow->setEnabled(false);
}

void GUIWindow::EnableWindow() 
{
  rootwindow->setEnabled(true);
}

void GUIWindow::BringToFront() 
{
  rootwindow->activate();
}
