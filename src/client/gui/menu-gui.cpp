/*
  Copyright (C) 2005 - 2007 Development Team of Peragro Tempus

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

#include "client/gui/menu-gui.h"

#include <CEGUI.h>
#include <CEGUIWindowManager.h>
#include <CEGUILogger.h>
#include <ivaria/icegui.h>

#include "client/gui/guimanager.h"
#include "client/gui/skinmanager.h"
#include "client/gui/menumanager.h"

namespace PT
{
  namespace GUI
  {
    namespace Windows
    {

      MenuWindow::MenuWindow(GUIManager* guiManager)
        : GUIWindow (guiManager)
      {
        windowName = MENUWINDOW;
      } // end MenuWindow()

      MenuWindow::~MenuWindow()
      {
      } // end ~MenuWindow()

      bool MenuWindow::MenuButtonPressed(const CEGUI::EventArgs& e)
      {
        CEGUI::Window* btn = guimanager->GetCeguiWindow("Menu/Dialog");
        if (btn)
          btn->isVisible() ? btn->hide() : btn->show();

        return true;
      } // end MenuButtonPressed()

      bool MenuWindow::MouseLeaves(const CEGUI::EventArgs& e)
      {
        using namespace CEGUI;
        Window* btn = guimanager->GetCeguiWindow("Menu/Dialog");
        if (btn && btn->isVisible())
        {
          const MouseEventArgs & ddeam = static_cast<const MouseEventArgs&>(e);

          Rect rect = btn->getPixelRect();
          if (!rect.isPointInRect(ddeam.position))
          {
            if (!guimanager->GetMenuManager()->IsDragging())
            {
              btn->hide();
            }
          }
        }

        return false;
      } // end MouseLeaves()

      bool MenuWindow::Create()
      {
        ReloadWindow();

        return true;
      } // end Create()

      bool MenuWindow::ReloadWindow()
      {
        CEGUI::Window* btn;

        // Load the layout and parent it to the root window.
        window = GUIWindow::LoadLayout ("client/menu.layout");
        GUIWindow::AddToRoot(window);

        // Register the menu button event.
        btn = guimanager->GetCeguiWindow("Menu/MenuButton");
        if (btn) btn->subscribeEvent(CEGUI::PushButton::EventClicked,
          CEGUI::SubscriberSlot(&MenuWindow::MenuButtonPressed, this));

        // Register the mouse leave event.
        btn = guimanager->GetCeguiWindow(ROOTWINDOW);
        if (btn) btn->subscribeEvent(CEGUI::Window::EventMouseMove,
          CEGUI::SubscriberSlot(&MenuWindow::MouseLeaves, this));

        // Hide the dialog.
        btn = guimanager->GetCeguiWindow("Menu/Dialog");
        if (btn) btn->hide();

        return true;
      } // end ReloadWindow()

    } // Windows namespace
  } // GUI namespace
} // PT namespace

