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
      VideoOptionsWindow::VideoOptionsWindow(GUIManager* guiManager)
        : GUIWindow (guiManager)
      {
        windowName = VIDEOOPTIONSWINDOW;
      } // end VideoOptionsWindow()

      VideoOptionsWindow::~VideoOptionsWindow()
      {
      } // end ~VideoOptionsWindow()

      void VideoOptionsWindow::SaveConfig()
      {
        app_cfg->Save();
      } // end SaveConfig()

      bool VideoOptionsWindow::Create()
      {
        ReloadWindow();

        return true;
      } // end Create()

      bool VideoOptionsWindow::ReloadWindow()
      {
        // Load the layout.
        window = GUIWindow::LoadLayout("client/options/video.xml");

        app_cfg = csQueryRegistry<iConfigManager>
          (PointerLibrary::getInstance()->getClient()->GetObjectRegistry());
        if (!app_cfg)
        {
          Report(PT::Error, "Can't find the config manager!");
          return false;
        }

        // Set up the button behaviour for Reflections.
        CreateDropListReflections();
        btn = winMgr->getWindow("Options/Video/Reflections/DropList");
        btn->subscribeEvent(CEGUI::Combobox::EventListSelectionAccepted,
          CEGUI::Event::Subscriber(&VideoOptionsWindow::OnDropListReflections, this));

        // Set up the button behaviour for Texture quality.
        CreateDropListTexture();
        btn = winMgr->getWindow("Options/Video/TextureQuality/DropList");
        btn->subscribeEvent(CEGUI::Combobox::EventListSelectionAccepted,
          CEGUI::Event::Subscriber(&VideoOptionsWindow::OnDropListTexture, this));

        // Set up the Fullscreen checkbox.
        CreateFullScreenCheckBox();
        btn = winMgr->getWindow("Options/Video/Fullscreen");
        btn->subscribeEvent(CEGUI::Checkbox::EventCheckStateChanged,
          CEGUI::Event::Subscriber(&VideoOptionsWindow::OnFullScreenCheckBox, this));

        // Set up the Adaptive distance clipping checkbox and spinners.
        CreateAdaptiveClippingCheckBox();
        btn = winMgr->getWindow("Options/Video/Distance_Clipping");
        btn->subscribeEvent(CEGUI::Checkbox::EventCheckStateChanged,
          CEGUI::Event::Subscriber(&VideoOptionsWindow::OnAdaptiveClippingCheckBox, this));
        CreateAdaptiveSpinners();
        btn = winMgr->getWindow("Options/Video/Minimum_FPS/Spinner");
        btn->subscribeEvent(CEGUI::Spinner::EventValueChanged,
          CEGUI::Event::Subscriber(&VideoOptionsWindow::OnMinFPSSpinnerChanged, this));
        btn = winMgr->getWindow("Options/Video/Maximum_FPS/Spinner");
        btn->subscribeEvent(CEGUI::Spinner::EventValueChanged,
          CEGUI::Event::Subscriber(&VideoOptionsWindow::OnMaxFPSSpinnerChanged, this));
        btn = winMgr->getWindow("Options/Video/Minimum_Distance/Spinner");
        btn->subscribeEvent(CEGUI::Spinner::EventValueChanged,
          CEGUI::Event::Subscriber(&VideoOptionsWindow::OnMinDistanceSpinnerChanged, this));

        // Register listener for distance clipping shortcut.
        using namespace PT::Events;
        EventHandler<VideoOptionsWindow>* cbDistClip =
          new EventHandler<VideoOptionsWindow>
          (&VideoOptionsWindow::ToggleDistClip, this);
        PointerLibrary::getInstance()->getEventManager()->
          AddListener("input.ACTION_TOGGLEDISTCLIP", cbDistClip);

        return true;
      } // end ReloadWindow()

      void VideoOptionsWindow::SendUpdateEvent()
      {
        PT::Events::EventManager* evmgr =
          PointerLibrary::getInstance()->getEventManager();
        evmgr->AddEvent(evmgr->CreateEvent("options.update.video"));
      } // end SendUpdateEvent()

      bool VideoOptionsWindow::ToggleDistClip(iEvent& e)
      {
        CreateAdaptiveClippingCheckBox();
        return false;
      } // end ToggleDistClip()

      bool VideoOptionsWindow::OnDropListReflections(const CEGUI::EventArgs& e)
      {
        btn = winMgr->getWindow("Options/Video/Reflections/DropList");
        uint id = ((CEGUI::Combobox*)btn)->getSelectedItem()->getID();

        bool ref = false;

        switch(id)
        {
        case 0: // Off
          ref = false;
          break;
        case 1: // On
          ref = true;
          break;

        default: Report(PT::Error, "OnDropListReflections: failed %d", id);
        }

        app_cfg->SetBool("Client.Video.WaterReflections", ref);
        SaveConfig();
        return true;
      } // end OnDropListReflections()

      void VideoOptionsWindow::CreateDropListReflections()
      {

        btn = winMgr->getWindow("Options/Video/Reflections/DropList");

        bool ref = app_cfg->GetBool("Client.Video.WaterReflections", true);

        if (ref)
          ((CEGUI::Combobox*)btn)->setText("On");
        else
          ((CEGUI::Combobox*)btn)->setText("Off");


        CEGUI::ListboxItem* charIdItem =
          new CEGUI::ListboxTextItem((CEGUI::utf8*)"Off", 0);
        ((CEGUI::Combobox*)btn)->addItem(charIdItem);

        charIdItem = new CEGUI::ListboxTextItem((CEGUI::utf8*)"On", 1);
        ((CEGUI::Combobox*)btn)->addItem(charIdItem);

        ((CEGUI::Combobox*)btn)->setReadOnly(true);

      } // end CreateDropListReflections()

      bool VideoOptionsWindow::OnDropListTexture(const CEGUI::EventArgs& e)
      {
        btn = winMgr->getWindow("Options/Video/TextureQuality/DropList");
        uint id = ((CEGUI::Combobox*)btn)->getSelectedItem()->getID();

        int quality = 0;

        switch(id)
        {
        case 0: // Normal
          quality = 0;
          break;
        case 1: // Low
          quality = 1;
          break;
        case 2: // Very Low
          quality = 2;
          break;
        case 3: // Lowest
          quality = 3;
          break;

        default:
          Report(PT::Error, "OnDropListTexture: failed %d", id);
        }

        app_cfg->SetInt("Video.OpenGL.TextureDownsample", quality);
        SaveConfig();
        return true;
      } // end OnDropListTexture()

      void VideoOptionsWindow::CreateDropListTexture()
      {

        btn = winMgr->getWindow("Options/Video/TextureQuality/DropList");

        int quality = app_cfg->GetInt("Video.OpenGL.TextureDownsample", 0);

        switch(quality)
        {
        case 0:
          ((CEGUI::Combobox*)btn)->setText("Normal");
          break;
        case 1:
          ((CEGUI::Combobox*)btn)->setText("Low");
          break;
        case 2:
          ((CEGUI::Combobox*)btn)->setText("Very Low");
          break;
        case 3:
          ((CEGUI::Combobox*)btn)->setText("Lowest");
          break;
        default:
          ((CEGUI::Combobox*)btn)->setText("Custom");
        }

        CEGUI::ListboxItem* charIdItem =
          new CEGUI::ListboxTextItem((CEGUI::utf8*)"Normal", 0);
        ((CEGUI::Combobox*)btn)->addItem(charIdItem);

        charIdItem = new CEGUI::ListboxTextItem((CEGUI::utf8*)"Low", 1);
        ((CEGUI::Combobox*)btn)->addItem(charIdItem);

        charIdItem = new CEGUI::ListboxTextItem((CEGUI::utf8*)"Very Low", 2);
        ((CEGUI::Combobox*)btn)->addItem(charIdItem);

        charIdItem = new CEGUI::ListboxTextItem((CEGUI::utf8*)"Lowest", 3);
        ((CEGUI::Combobox*)btn)->addItem(charIdItem);

        ((CEGUI::Combobox*)btn)->setReadOnly(true);

      } // end CreateDropListTexture()

      bool VideoOptionsWindow::OnFullScreenCheckBox(const CEGUI::EventArgs& e)
      {
        btn = winMgr->getWindow("Options/Video/Fullscreen");
        bool fs = ((CEGUI::Checkbox*)btn)->isSelected();

        app_cfg->SetBool("Video.FullScreen", fs);
        SaveConfig();
        return true;
      } // end OnFullScreenCheckBox()

      void VideoOptionsWindow::CreateFullScreenCheckBox()
      {
        btn = winMgr->getWindow("Options/Video/Fullscreen");

        bool fs = app_cfg->GetBool("Video.FullScreen", false);

        ((CEGUI::Checkbox*)btn)->setSelected(fs);
      } // end CreateFullScreenCheckBox()

      bool VideoOptionsWindow::OnAdaptiveClippingCheckBox(const CEGUI::EventArgs& e)
      {
        btn = winMgr->getWindow("Options/Video/Distance_Clipping");
        bool dc = ((CEGUI::Checkbox*)btn)->isSelected();

        app_cfg->SetBool("Client.Video.DistanceClipping", dc);
        SendUpdateEvent();
        SaveConfig();
        return true;
      } // end OnAdaptiveClippingCheckBox()

      void VideoOptionsWindow::CreateAdaptiveClippingCheckBox()
      {
        btn = winMgr->getWindow("Options/Video/Distance_Clipping");

        bool dc = app_cfg->GetBool("Client.Video.DistanceClipping", false);

        ((CEGUI::Checkbox*)btn)->setSelected(dc);
      } // end CreateAdaptiveClippingCheckBox()

      bool VideoOptionsWindow::OnMinFPSSpinnerChanged(const CEGUI::EventArgs &e)
      {
        btn = winMgr->getWindow("Options/Video/Minimum_FPS/Spinner");
        float value = ((CEGUI::Spinner*)btn)->getCurrentValue();

        app_cfg->SetFloat("Client.Video.MinFPS", value);
        SendUpdateEvent();
        SaveConfig();
        return true;
      } // end OnMinFPSSpinnerChanged()

      bool VideoOptionsWindow::OnMaxFPSSpinnerChanged(const CEGUI::EventArgs &e)
      {
        btn = winMgr->getWindow("Options/Video/Maximum_FPS/Spinner");
        float value = ((CEGUI::Spinner*)btn)->getCurrentValue();

        app_cfg->SetFloat("Client.Video.MaxFPS", value);
        SendUpdateEvent();
        SaveConfig();
        return true;
      } // end OnMaxFPSSpinnerChanged()

      bool VideoOptionsWindow::OnMinDistanceSpinnerChanged(const CEGUI::EventArgs &e)
      {
        btn = winMgr->getWindow("Options/Video/Minimum_Distance/Spinner");
        float value = ((CEGUI::Spinner*)btn)->getCurrentValue();

        app_cfg->SetFloat("Client.Video.MinDistance", value);
        SendUpdateEvent();
        SaveConfig();
        return true;
      } // end OnMinDistanceSpinnerChanged()

      void VideoOptionsWindow::CreateAdaptiveSpinners()
      {
        btn = winMgr->getWindow("Options/Video/Minimum_FPS/Spinner");
        ((CEGUI::Spinner*)btn)->
          setCurrentValue(app_cfg->GetFloat("Client.Video.MinFPS", 20.0f));

        btn = winMgr->getWindow("Options/Video/Maximum_FPS/Spinner");
        ((CEGUI::Spinner*)btn)->
          setCurrentValue(app_cfg->GetFloat("Client.Video.MaxFPS", 60.0f));

        btn = winMgr->getWindow("Options/Video/Minimum_Distance/Spinner");
        ((CEGUI::Spinner*)btn)->
          setCurrentValue(app_cfg->GetFloat("Client.Video.MinDistance", 100.0f));
      } // end CreateAdaptiveSpinners()

    } // Windows namespace
  } // GUI namespace
} // PT namespace

