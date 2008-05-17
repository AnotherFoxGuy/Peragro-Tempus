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

namespace PT
{
  namespace GUI
  {
    namespace Windows
    {

	OkDialogWindow::OkDialogWindow(GUIManager* guimanager)
	: GUIWindow (guimanager)
	{
       windowName = OKDIALOGWINDOW;
	}

	OkDialogWindow::~OkDialogWindow()
	{
	}

	bool OkDialogWindow::OnOkButton(const CEGUI::EventArgs& args)
	{
	  using namespace CEGUI;

      winMgr->destroyWindow(window);
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

	bool OkDialogWindow::Create()
    {
      ReloadWindow();
      return true;
    }

    bool OkDialogWindow::ReloadWindow()
    {
      winMgr = cegui->GetWindowManagerPtr ();

	  if(!winMgr->isWindowPresent("OkDialog/Frame"))
	  {
        window = GUIWindow::LoadLayout ("okdialog.xml");
        GUIWindow::AddToRoot(window);
	  }

	  // Get the Ok Button
	  btn = winMgr->getWindow("OkDialog/OkButton");
	  btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&OkDialogWindow::OnOkButton, this));
      return true;
	}

	ConfirmDialogWindow::ConfirmDialogWindow(GUIManager* guimanager)
	: GUIWindow (guimanager)
	{
      windowName = CONFIRMDIALOGWINDOW;
	}

	ConfirmDialogWindow::~ConfirmDialogWindow()
	{
	}

	bool ConfirmDialogWindow::OnYesButton(const CEGUI::EventArgs& args)
	{
	  using namespace CEGUI;

	  winMgr->destroyWindow(window);

	  return true;
	}

	bool ConfirmDialogWindow::OnNoButton(const CEGUI::EventArgs& args)
	{
	  using namespace CEGUI;

	  winMgr->destroyWindow(window);

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

	bool ConfirmDialogWindow::Create()
    {
      ReloadWindow();
      return true;
    }

    bool ConfirmDialogWindow::ReloadWindow()
    {
	  if(!winMgr->isWindowPresent("ConfirmDialog/Frame"))
	  {
        window = GUIWindow::LoadLayout ("client/confirmdialog.xml");
        GUIWindow::AddToRoot(window);
      }

	  // Get the Yes Button
	  btn = winMgr->getWindow("ConfirmDialog/YesButton");
	  btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&ConfirmDialogWindow::OnYesButton, this));

	  // Get the No Button
	  btn = winMgr->getWindow("ConfirmDialog/NoButton");
	  btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&ConfirmDialogWindow::OnNoButton, this));
      return true;
    }
    }
  }
}