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

#include "loadscreen-gui.h"

LoadScreenWindow::LoadScreenWindow (GUIManager* guimanager)
: GUIWindow (guimanager)
{
}

LoadScreenWindow::~LoadScreenWindow ()
{
}

void LoadScreenWindow::CreateGUIWindow ()
{
  GUIWindow::CreateGUIWindow ("loadscreen.xml");
  winMgr = cegui->GetWindowManagerPtr ();

  // Get the root window
  rootwindow = winMgr->getWindow("LoadScreen");
}

void LoadScreenWindow::ShowWindow()
{
  rootwindow->show();
}

void LoadScreenWindow::HideWindow()
{
  rootwindow->hide();
}

void LoadScreenWindow::SetProgress(float progress)
{
  CEGUI::ProgressBar* bar = static_cast<CEGUI::ProgressBar*>(winMgr->getWindow("LoadScreen/Bar"));
  bar->setProgress(progress);
}

bool LoadScreenWindow::IsVisible()
{
  return rootwindow->isVisible();
}
