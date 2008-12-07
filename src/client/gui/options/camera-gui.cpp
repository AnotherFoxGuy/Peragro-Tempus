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

      void CameraOptionsWindow::SaveConfig()
      {
        app_cfg->Save();
      } // end SaveConfig()

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
        CreateAdaptiveSliders();
        btn = winMgr->getWindow("Options/Camera/Minimum_FPS/Slider");
        btn->subscribeEvent(CEGUI::Slider::EventValueChanged,
          CEGUI::Event::Subscriber(&CameraOptionsWindow::OnMinFPSSliderChanged, this));
        btn = winMgr->getWindow("Options/Camera/Maximum_FPS/Slider");
        btn->subscribeEvent(CEGUI::Slider::EventValueChanged,
          CEGUI::Event::Subscriber(&CameraOptionsWindow::OnMaxFPSSliderChanged, this));
        btn = winMgr->getWindow("Options/Camera/Minimum_Distance/Slider");
        btn->subscribeEvent(CEGUI::Slider::EventValueChanged,
          CEGUI::Event::Subscriber(&CameraOptionsWindow::OnMinDistanceSliderChanged, this));

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
        SaveConfig();
        return true;
      } // end OnAdaptiveClippingCheckBox()

      void CameraOptionsWindow::CreateAdaptiveClippingCheckBox()
      {
        btn = winMgr->getWindow("Options/Camera/Distance_Clipping");

        bool dc = app_cfg->GetBool("Peragro.Video.DistanceClipping", false);

        ((CEGUI::Checkbox*)btn)->setSelected(dc);
      } // end CreateAdaptiveClippingCheckBox()

      bool CameraOptionsWindow::OnMinFPSSliderChanged(const CEGUI::EventArgs &e)
      {
        btn = winMgr->getWindow("Options/Camera/Minimum_FPS/Slider");
        float value = ceil(((CEGUI::Slider*)btn)->getCurrentValue());

        std::stringstream ss;
        ss.setf(std::ios_base::dec);
        ss << value;

        btn = winMgr->getWindow("Options/Camera/Minimum_FPS/Value");
        btn->setText(ss.str());

        app_cfg->SetFloat("Peragro.Video.MinFPS", value);
        SendUpdateEvent();
        SaveConfig();
        return true;
      } // end OnMinFPSSliderChanged()

      bool CameraOptionsWindow::OnMaxFPSSliderChanged(const CEGUI::EventArgs &e)
      {
        btn = winMgr->getWindow("Options/Camera/Maximum_FPS/Slider");
        float value = ceil(((CEGUI::Slider*)btn)->getCurrentValue());

        std::stringstream ss;
        ss.setf(std::ios_base::dec);
        ss << value;

        btn = winMgr->getWindow("Options/Camera/Maximum_FPS/Value");
        btn->setText(ss.str());

        app_cfg->SetFloat("Peragro.Video.MaxFPS", value);
        SendUpdateEvent();
        SaveConfig();
        return true;
      } // end OnMaxFPSSliderChanged()

      bool CameraOptionsWindow::OnMinDistanceSliderChanged(const CEGUI::EventArgs &e)
      {
        btn = winMgr->getWindow("Options/Camera/Minimum_Distance/Slider");
        float value = ceil(((CEGUI::Slider*)btn)->getCurrentValue());

        std::stringstream ss;
        ss.setf(std::ios_base::dec);
        ss << value;

        btn = winMgr->getWindow("Options/Camera/Minimum_Distance/Value");
        btn->setText(ss.str());

        app_cfg->SetFloat("Peragro.Video.MinDistance", value);
        SendUpdateEvent();
        SaveConfig();
        return true;
      } // end OnMinDistanceSliderChanged()

      void CameraOptionsWindow::CreateAdaptiveSliders()
      {
        btn = winMgr->getWindow("Options/Camera/Minimum_FPS/Slider");
        ((CEGUI::Slider*)btn)->
          setCurrentValue(app_cfg->GetFloat("Peragro.Video.MinFPS", 20.0f));

        btn = winMgr->getWindow("Options/Camera/Maximum_FPS/Slider");
        ((CEGUI::Slider*)btn)->
          setCurrentValue(app_cfg->GetFloat("Peragro.Video.MaxFPS", 60.0f));

        btn = winMgr->getWindow("Options/Camera/Minimum_Distance/Slider");
        ((CEGUI::Slider*)btn)->
          setCurrentValue(app_cfg->GetFloat("Peragro.Video.MinDistance", 100.0f));

        // To show the values.
        CEGUI::EventArgs e;
        OnMinFPSSliderChanged(e);
        OnMaxFPSSliderChanged(e);
        OnMinDistanceSliderChanged(e);
      } // end CreateAdaptiveSliders()

    } // Windows namespace
  } // GUI namespace
} // PT namespace

