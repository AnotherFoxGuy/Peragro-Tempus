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
#include "client/event/eventmanager.h"
#include "client/event/interfaceevent.h"

namespace PT
{
  namespace GUI
  {
    namespace Windows
    {
      VideoWindow::VideoWindow(GUIManager* guiManager)
        : GUIWindow (guiManager)
      {
        windowName = VIDEOWINDOW;
      } // end VideoWindow()

      VideoWindow::~VideoWindow()
      {
      } // end ~VideoWindow()

      void VideoWindow::SaveConfig()
      {
        app_cfg->Save();
      } // end SaveConfig()

      bool VideoWindow::Create()
      {
        ReloadWindow();

        return true;
      } // end Create()

      bool VideoWindow::ReloadWindow()
      {
        // Load the layout.
        window = GUIWindow::LoadLayout ("client/options/video.xml");

        app_cfg = csQueryRegistry<iConfigManager>
          (PointerLibrary::getInstance()->getClient()->GetObjectRegistry());
        if (!app_cfg)
        {
          Report(PT::Error, "Can't find the config manager!");
          return false;
        }

        // Set up the button behaviour for Reflections.
        CreateDropListReflections();
        btn = winMgr->getWindow("Video/Reflections/DropList");
        btn->subscribeEvent(CEGUI::Combobox::EventListSelectionAccepted,
          CEGUI::Event::Subscriber(&VideoWindow::OnDropListReflections, this));

        // Set up the button behaviour for Texture quality.
        CreateDropListTexture();
        btn = winMgr->getWindow("Video/TextureQuality/DropList");
        btn->subscribeEvent(CEGUI::Combobox::EventListSelectionAccepted,
          CEGUI::Event::Subscriber(&VideoWindow::OnDropListTexture, this));

        // Set up the Fullscreen checkbox.
        CreateFullScreenCheckBox();
        btn = winMgr->getWindow("Video/Fullscreen");
        btn->subscribeEvent(CEGUI::Checkbox::EventCheckStateChanged,
          CEGUI::Event::Subscriber(&VideoWindow::OnFullScreenCheckBox, this));

        // Set up the Adaptive distance clipping spinners.
        CreateAdaptiveSpinners();
        btn = winMgr->getWindow("Video/Minimum_FPS/Spinner");
        btn->subscribeEvent(CEGUI::Spinner::EventValueChanged,
          CEGUI::Event::Subscriber(&VideoWindow::OnMinFPSSpinnerChanged, this));
        btn = winMgr->getWindow("Video/Maximum_FPS/Spinner");
        btn->subscribeEvent(CEGUI::Spinner::EventValueChanged,
          CEGUI::Event::Subscriber(&VideoWindow::OnMaxFPSSpinnerChanged, this));
        btn = winMgr->getWindow("Video/Minimum_Distance/Spinner");
        btn->subscribeEvent(CEGUI::Spinner::EventValueChanged,
          CEGUI::Event::Subscriber(&VideoWindow::OnMinDistanceSpinnerChanged, this));

        return true;
      } // end ReloadWindow()

      bool VideoWindow::OnDropListReflections(const CEGUI::EventArgs& e)
      {
        btn = winMgr->getWindow("Video/Reflections/DropList");
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

        app_cfg->SetBool("Client.waterreflections", ref);
        SaveConfig();
        return true;
      } // end OnDropListReflections()

      void VideoWindow::CreateDropListReflections()
      {

        btn = winMgr->getWindow("Video/Reflections/DropList");

        bool ref = app_cfg->GetBool("Client.waterreflections");

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

      bool VideoWindow::OnDropListTexture(const CEGUI::EventArgs& e)
      {
        btn = winMgr->getWindow("Video/TextureQuality/DropList");
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

      void VideoWindow::CreateDropListTexture()
      {

        btn = winMgr->getWindow("Video/TextureQuality/DropList");

        int quality = app_cfg->GetInt("Video.OpenGL.TextureDownsample");

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

      bool VideoWindow::OnFullScreenCheckBox(const CEGUI::EventArgs& e)
      {
        btn = winMgr->getWindow("Video/Fullscreen");
        bool fs = ((CEGUI::Checkbox*)btn)->isSelected();

        app_cfg->SetBool("Video.FullScreen", fs);
        SaveConfig();
        return true;
      } // end OnFullScreenCheckBox()

      void VideoWindow::CreateFullScreenCheckBox()
      {
        btn = winMgr->getWindow("Video/Fullscreen");

        bool fs = app_cfg->GetBool("Video.FullScreen");

        ((CEGUI::Checkbox*)btn)->setSelected(fs);
      } // end CreateFullScreenCheckBox()

      bool VideoWindow::OnMinFPSSpinnerChanged(const CEGUI::EventArgs &e)
      {
        btn = winMgr->getWindow("Video/Minimum_FPS/Spinner");
        float value = ((CEGUI::Spinner*)btn)->getCurrentValue();

        app_cfg->SetFloat("Client.minFPS", value);
        SaveConfig();
        return true;
      } // end OnMinFPSSpinnerChanged()

      bool VideoWindow::OnMaxFPSSpinnerChanged(const CEGUI::EventArgs &e)
      {
        btn = winMgr->getWindow("Video/Maximum_FPS/Spinner");
        float value = ((CEGUI::Spinner*)btn)->getCurrentValue();

        app_cfg->SetFloat("Client.maxFPS", value);
        SaveConfig();
        return true;
      } // end OnMaxFPSSpinnerChanged()

      bool VideoWindow::OnMinDistanceSpinnerChanged(const CEGUI::EventArgs &e)
      {
        btn = winMgr->getWindow("Video/Minimum_Distance/Spinner");
        float value = ((CEGUI::Spinner*)btn)->getCurrentValue();

        app_cfg->SetFloat("Client.minDistance", value);
        SaveConfig();
        return true;
      } // end OnMinDistanceSpinnerChanged()

      void VideoWindow::CreateAdaptiveSpinners()
      {
        btn = winMgr->getWindow("Video/Minimum_FPS/Spinner");
        ((CEGUI::Spinner*)btn)->
          setCurrentValue(app_cfg->GetFloat("Client.minFPS"));

        btn = winMgr->getWindow("Video/Maximum_FPS/Spinner");
        ((CEGUI::Spinner*)btn)->
          setCurrentValue(app_cfg->GetFloat("Client.maxFPS"));

        btn = winMgr->getWindow("Video/Minimum_Distance/Spinner");
        ((CEGUI::Spinner*)btn)->
          setCurrentValue(app_cfg->GetFloat("Client.minDistance"));
      } // end CreateAdaptiveSpinners()

    } // Windows namespace
  } // GUI namespace
} // PT namespace

