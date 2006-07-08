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

/*=================//
//    HUDWindow   //
//=================*/
HUDWindow::HUDWindow (GUIManager* guimanager)
: GUIWindow (guimanager)
{
}

HUDWindow::~HUDWindow ()
{
}

void HUDWindow::CreateGUIWindow ()
{
  GUIWindow::CreateGUIWindow ("playerhud.xml");
  GUIWindow::CreateGUIWindow ("entityhud.xml");
  winMgr = cegui->GetWindowManagerPtr ();

  // Get the root window
  rootwindow = winMgr->getWindow("PlayerHUD/Frame");

  // Set up the progressmeters
  hpbar   = (CEGUI::ProgressBar*) winMgr->getWindow("PlayerHUD/HP");
  mpbar   = (CEGUI::ProgressBar*) winMgr->getWindow("PlayerHUD/MP");
  xpbar   = (CEGUI::ProgressBar*) winMgr->getWindow("PlayerHUD/XP");

  hpbar->setProgress(1.0f);
  mpbar->setProgress(1.0f);
  xpbar->setProgress(1.0f);
}

void HUDWindow::SetHP (float hp)
{
  hpbar->setProgress(hp);
}

void HUDWindow::SetMP (float mp)
{
  mpbar->setProgress(mp);
}
void HUDWindow::SetName (const char* name)
{
  CEGUI::StaticText* namewin   = (CEGUI::StaticText*) winMgr->getWindow("HUD/Name");

  namewin->setText((CEGUI::String)name);
}