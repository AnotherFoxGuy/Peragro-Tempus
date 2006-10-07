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

/*==============//
//   OkDialog   //
//==============*/
OkDialogWindow::OkDialogWindow(GUIManager* guimanager)
: GUIWindow (guimanager)
{
}

OkDialogWindow::~OkDialogWindow()
{
}

bool OkDialogWindow::OnOkButton(const CEGUI::EventArgs& args)
{
  using namespace CEGUI;

  const WindowEventArgs& ddea = static_cast<const WindowEventArgs&>(args);
  winMgr->destroyWindow(rootwindow);

  return true;
}

void OkDialogWindow::SetOkEvent(CEGUI::Event::Subscriber subscriber, bool destroywindow)
{
  btn = winMgr->getWindow("OkDialog/OkButton");
  if(!destroywindow)
    btn->removeEvent(CEGUI::PushButton::EventClicked);
  btn->subscribeEvent(CEGUI::PushButton::EventClicked, subscriber);
}

void OkDialogWindow::SetText(const char* text)
{
  btn = winMgr->getWindow("OkDialog/Dialog");
  btn->setText(text);
}

void OkDialogWindow::CreateGUIWindow()
{
  winMgr = cegui->GetWindowManagerPtr ();

  if(!winMgr->isWindowPresent("OkDialog/Frame"))
    GUIWindow::CreateGUIWindow ("okdialog.xml");

  // Get the root window
  rootwindow = winMgr->getWindow("OkDialog/Frame");

  // Get the Ok Button
  btn = winMgr->getWindow("OkDialog/OkButton");
  btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&OkDialogWindow::OnOkButton, this));

}

/*=================//
//  ConfirmDialog  //
//=================*/
ConfirmDialogWindow::ConfirmDialogWindow(GUIManager* guimanager)
: GUIWindow (guimanager)
{
}

ConfirmDialogWindow::~ConfirmDialogWindow()
{
}

bool ConfirmDialogWindow::OnYesButton(const CEGUI::EventArgs& args)
{
  using namespace CEGUI;

  const WindowEventArgs& ddea = static_cast<const WindowEventArgs&>(args);
  winMgr->destroyWindow(rootwindow);

  return true;
}

bool ConfirmDialogWindow::OnNoButton(const CEGUI::EventArgs& args)
{
  using namespace CEGUI;

  const WindowEventArgs& ddea = static_cast<const WindowEventArgs&>(args);
  winMgr->destroyWindow(rootwindow);

  return true;
}

void ConfirmDialogWindow::SetText(const char* text)
{
  btn = winMgr->getWindow("ConfirmDialog/Dialog");
  btn->setText(text);
}

void ConfirmDialogWindow::SetYesEvent(CEGUI::Event::Subscriber subscriber, bool destroywindow)
{
  btn = winMgr->getWindow("ConfirmDialog/YesButton");
  if(!destroywindow)
    btn->removeEvent(CEGUI::PushButton::EventClicked);
  btn->subscribeEvent(CEGUI::PushButton::EventClicked, subscriber);
}

void ConfirmDialogWindow::SetNoEvent(CEGUI::Event::Subscriber subscriber, bool destroywindow)
{
  btn = winMgr->getWindow("ConfirmDialog/NoButton");
  if(!destroywindow)
    btn->removeEvent(CEGUI::PushButton::EventClicked);
  btn->subscribeEvent(CEGUI::PushButton::EventClicked, subscriber);
}

void ConfirmDialogWindow::CreateGUIWindow()
{
  winMgr = cegui->GetWindowManagerPtr ();

  if(!winMgr->isWindowPresent("ConfirmDialog/Frame"))
    GUIWindow::CreateGUIWindow ("confirmdialog.xml");

  // Get the root window
  rootwindow = winMgr->getWindow("ConfirmDialog/Frame");

  // Get the Yes Button
  btn = winMgr->getWindow("ConfirmDialog/YesButton");
  btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&ConfirmDialogWindow::OnYesButton, this));

  // Get the No Button
  btn = winMgr->getWindow("ConfirmDialog/NoButton");
  btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&ConfirmDialogWindow::OnNoButton, this));

}
