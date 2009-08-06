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

#include "client/gui/background-gui.h"

#include "CEGUI.h"
#include "CEGUIWindowManager.h"
#include "CEGUILogger.h"

namespace PT
{
  namespace GUI
  {
    namespace Windows
    {
      BackgroundWindow::BackgroundWindow(GUIManager* guimanager)
        : GUIWindow (guimanager)
      {
        windowName = BACKGROUNDWINDOW;
      } // end LoadScreenWindow()

      BackgroundWindow::~BackgroundWindow()
      {
      } // end ~LoadScreenWindow()

      bool BackgroundWindow::Create()
      {
        ReloadWindow();
        return true;
      } // end Create()

      bool BackgroundWindow::ReloadWindow()
      {
        window = winMgr->createWindow("Peragro/StaticImage", "Background");
        GUIWindow::AddToRoot(window);

        window->setPosition(CEGUI::UVector2(CEGUI::UDim(0,0), CEGUI::UDim(0,0)));
        window->setSize(CEGUI::UVector2(CEGUI::UDim(1,0), CEGUI::UDim(1,0)));

        vfs->ChDir ("/peragro/art/skins/default/images/");
        CEGUI::ImagesetManager* immgr = cegui->GetImagesetManagerPtr();
        if (!immgr->isImagesetPresent("Background"))
          immgr->createImagesetFromImageFile("Background", "background1600.jpg");

        // set the background image
        window->setProperty("Image", "set:Background image:full_image");
        window->setProperty("BackgroundEnabled", "True");
        window->setProperty("FrameEnabled", "False");
        window->setProperty("Disabled", "True");
        window->moveToBack();

        HideWindow();

        return true;
      } // end ReloadWindow()

      void BackgroundWindow::ShowWindow()
      {
        GUIWindow::ShowWindow();
      } // end ShowWindow()

      void BackgroundWindow::HideWindow()
      {
        GUIWindow::HideWindow();
      } // end HideWindow()

      bool BackgroundWindow::IsVisible()
      {
        return GUIWindow::IsVisible();
      } // end IsVisible()

    } // Windows namespace
  } // GUI namespace
} // PT namespace

