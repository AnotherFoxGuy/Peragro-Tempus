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

#include "client/gui/base-gui.h"

#include "CEGUI.h"
#include "CEGUIWindowManager.h"
#include "CEGUILogger.h"

#include "client/pointer/pointer.h"
#include "client/network/network.h"

#include "common/reporter/reporter.h"

#include "client/gui/guimanager.h"
#include "client/gui/skinmanager.h"

namespace PT
{
  namespace GUI
  {
    GUIWindow::GUIWindow(GUIManager* guimanager)
      : guimanager(guimanager)
    {
      window = 0;
      iObjectRegistry* obj_reg =
        PointerLibrary::getInstance()->getObjectRegistry();
      vfs = csQueryRegistry<iVFS>(obj_reg);
      cegui = guimanager->GetCEGUI ();
      network = PointerLibrary::getInstance()->getNetwork();
      winMgr = cegui->GetWindowManagerPtr ();
    } // end GUIWindow()

    GUIWindow::~GUIWindow()
    {
    } // end ~GUIWindow()

    CEGUI::Window* GUIWindow::LoadLayout(const char* layoutFile)
    {
      // Load layout and set as root
      CEGUI::Window* win =
        guimanager->GetSkinMananger()->loadLayout(layoutFile);
      return win;
    } // end LoadLayout()

    bool GUIWindow::AddToRoot(CEGUI::Window* window)
    {
      CEGUI::Window* root = guimanager->GetCeguiWindow(ROOTWINDOW);
      if (root && window)
      {
        root->addChildWindow(window);
        HideWindow();
        return true;
      }
      else
      {
        Report(PT::Error, "Failed adding a window to root!");
        return false;
      }
    } // end AddToRoot()

    void GUIWindow::SetupToggleListener(const std::string& name)
    {
      if (name.empty()) return;

      std::string eventName("input.Toggle");
      eventName += name;

      PT_SETUP_HANDLER
      PT_REGISTER_LISTENER(GUIWindow, ToggleWindow, eventName)
    } // end SetupToggleListener()

    bool GUIWindow::ToggleWindow(iEvent& ev)
    {
      using namespace PT::Events;
      if (InputHelper::GetButtonDown(&ev))
      {
        ToggleWindow();
      }

      return true;
    } // end ToggleWindow()

    void GUIWindow::HideWindow()
    {
      if (window) window->setVisible(false);
    } // end HideWindow()

    void GUIWindow::ShowWindow()
    {
      if (window) window->setVisible(true);
    } // end ShowWindow()

    void GUIWindow::ToggleWindow()
    {
      if (window)
      {
        if (window->isVisible())
        {
          window->setVisible(false);
        }
        else
        {
          window->setVisible(true);
          window->activate();
        }
      }
    } // end ToggleWindow()

    bool GUIWindow::IsVisible()
    {
       if (window)
          return window->isVisible();
        else
          return false;
    } // end IsVisible()

    void GUIWindow::DisableWindow()
    {
      if (window) window->setEnabled(false);
    } // end DisableWindow()

    void GUIWindow::EnableWindow()
    {
      if (window) window->setEnabled(true);
    } // end EnableWindow()

    void GUIWindow::BringToFront()
    {
      if (window) window->activate();
    } // end BringToFront()

  } // GUI namespace
} // PT namespace

