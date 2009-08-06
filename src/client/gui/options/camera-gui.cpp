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

#include "client/gui/options/camera-gui.h"

#include <CEGUI.h>
#include <CEGUIWindowManager.h>
#include <CEGUILogger.h>
#include <ivaria/icegui.h>

#include "client/pointer/pointer.h"

#include "client/gui/skinmanager.h"

namespace PT
{
  namespace GUI
  {
    namespace Windows
    {
      CameraOptionsWindow::CameraOptionsWindow(GUIManager* guiManager)
        : GUIWindow (guiManager)
      {
        windowName = CAMERAOPTIONSWINDOW;
      } // end CameraOptionsWindow()

      CameraOptionsWindow::~CameraOptionsWindow()
      {
      } // end ~CameraOptionsWindow()

      bool CameraOptionsWindow::Create()
      {
        ReloadWindow();

        return true;
      } // end Create()

      bool CameraOptionsWindow::ReloadWindow()
      {
        // Load the layout.
        window = GUIWindow::LoadLayout("client/options/camera.layout");

        return true;
      } // end ReloadWindow()


    } // Windows namespace
  } // GUI namespace
} // PT namespace

