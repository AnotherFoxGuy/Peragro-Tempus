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

#include "client/gui/gui.h"
#include "client/gui/guimanager.h"
#include "client/gui/skinmanager.h"
#include "client/pointer/pointer.h"

#include <CEGUI.h>
#include <CEGUIWindowManager.h>
#include <CEGUILogger.h>
#include <ivaria/icegui.h>

#include "common/reporter/reporter.h"
#include "common/event/eventmanager.h"
#include "common/event/interfaceevent.h"

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
        window = GUIWindow::LoadLayout("client/options/camera.xml");

        app_cfg = csQueryRegistry<iConfigManager>
          (PointerLibrary::getInstance()->getObjectRegistry());
        if (!app_cfg)
        {
          Report(PT::Error, "Can't find the config manager!");
          return false;
        }

        // Set up the Adaptive distance clipping checkbox and spinners.
        CreateAdaptiveClippingCheckBox();
        btn = winMgr->getWindow("Options/Camera/Distance_Clipping");
        btn->subscribeEvent(CEGUI::Checkbox::EventCheckStateChanged,
          CEGUI::Event::Subscriber(&CameraOptionsWindow::OnAdaptiveClippingCheckBox, this));

        // Register listener for distance clipping shortcut.
        PT_SETUP_HANDLER
        PT_REGISTER_LISTENER(CameraOptionsWindow, ToggleDistClip, "input.DistanceClipping")

        return true;
      } // end ReloadWindow()

      void CameraOptionsWindow::SendUpdateEvent()
      {
        PT::Events::EventManager* evmgr =
          PointerLibrary::getInstance()->getEventManager();
        evmgr->AddEvent(evmgr->CreateEvent("interface.options.view"));
      } // end SendUpdateEvent()

      bool CameraOptionsWindow::ToggleDistClip(iEvent& e)
      {
        CreateAdaptiveClippingCheckBox();
        return false;
      } // end ToggleDistClip()

      bool CameraOptionsWindow::OnAdaptiveClippingCheckBox(const CEGUI::EventArgs& e)
      {
        btn = winMgr->getWindow("Options/Camera/Distance_Clipping");
        bool dc = ((CEGUI::Checkbox*)btn)->isSelected();

        app_cfg->SetBool("Peragro.Video.DistanceClipping", dc);
        SendUpdateEvent();
        app_cfg->Save();
        return true;
      } // end OnAdaptiveClippingCheckBox()

      void CameraOptionsWindow::CreateAdaptiveClippingCheckBox()
      {
        btn = winMgr->getWindow("Options/Camera/Distance_Clipping");

        bool dc = app_cfg->GetBool("Peragro.Video.DistanceClipping", false);

        ((CEGUI::Checkbox*)btn)->setSelected(dc);
      } // end CreateAdaptiveClippingCheckBox()

    } // Windows namespace
  } // GUI namespace
} // PT namespace

