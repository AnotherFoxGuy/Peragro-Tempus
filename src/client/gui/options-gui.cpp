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

#include "client/reporter/reporter.h"

namespace PT
{
  namespace GUI
  {
    namespace Windows
    {
      OptionsWindow::OptionsWindow (GUIManager* guimanager)
        : GUIWindow (guimanager)
      {
        windowName = OPTIONSWINDOW;
      }

      OptionsWindow::~OptionsWindow ()
      {
      }

      void OptionsWindow::SaveConfig()
      {
        app_cfg->Save();
      }

      void OptionsWindow::HideWindow()
      {
        // Hide the Options button.
        btn = winMgr->getWindow("Options/Options_Button");
        btn->setVisible(false);

        // Hide the Option menu.
        btn = winMgr->getWindow("Options/Frame");
        btn->setVisible(false);
      }

      bool OptionsWindow::OptionButtonPressed(const CEGUI::EventArgs& e)
      {
        // Hide the Options button.
        btn = winMgr->getWindow("Options/Options_Button");
        btn->setVisible(false);

        // Show the Option menu.
        btn = winMgr->getWindow("Options/Frame");
        btn->setVisible(true);
        btn->activate();
        return true;
      }

      bool OptionsWindow::OkButtonPressed(const CEGUI::EventArgs& e)
      {
        // Show the Options button.
        btn = winMgr->getWindow("Options/Options_Button");
        btn->setVisible(true);

        // Hide the Option menu.
        btn = winMgr->getWindow("Options/Frame");
        btn->setVisible(false);

        PT::Events::EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
        csRef<iEvent> interfaceEvent = evmgr->CreateEvent("interface.options");
        evmgr->AddEvent(interfaceEvent);

        return true;
      }

      bool OptionsWindow::OnDropListReflections(const CEGUI::EventArgs& e)
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
      }

      void OptionsWindow::CreateDropListReflections()
      {

        btn = winMgr->getWindow("Video/Reflections/DropList");

        bool ref = app_cfg->GetBool("Client.waterreflections");

        if (ref)
          ((CEGUI::Combobox*)btn)->setText("On");
        else
          ((CEGUI::Combobox*)btn)->setText("Off");


        CEGUI::ListboxItem* charIdItem = new CEGUI::ListboxTextItem((CEGUI::utf8*)"Off", 0);
        ((CEGUI::Combobox*)btn)->addItem(charIdItem);

        charIdItem = new CEGUI::ListboxTextItem((CEGUI::utf8*)"On", 1);
        ((CEGUI::Combobox*)btn)->addItem(charIdItem);

        ((CEGUI::Combobox*)btn)->setReadOnly(true);

      }

      bool OptionsWindow::OnDropListTexture(const CEGUI::EventArgs& e)
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

        default: Report(PT::Error, "OnDropListTexture: failed %d", id);
        }

        app_cfg->SetInt("Video.OpenGL.TextureDownsample", quality);
        SaveConfig();
        return true;
      }

      void OptionsWindow::CreateDropListTexture()
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

        CEGUI::ListboxItem* charIdItem = new CEGUI::ListboxTextItem((CEGUI::utf8*)"Normal", 0);
        ((CEGUI::Combobox*)btn)->addItem(charIdItem);

        charIdItem = new CEGUI::ListboxTextItem((CEGUI::utf8*)"Low", 1);
        ((CEGUI::Combobox*)btn)->addItem(charIdItem);

        charIdItem = new CEGUI::ListboxTextItem((CEGUI::utf8*)"Very Low", 2);
        ((CEGUI::Combobox*)btn)->addItem(charIdItem);

        charIdItem = new CEGUI::ListboxTextItem((CEGUI::utf8*)"Lowest", 3);
        ((CEGUI::Combobox*)btn)->addItem(charIdItem);

        ((CEGUI::Combobox*)btn)->setReadOnly(true);

      }

      bool OptionsWindow::OnDropListMovement(const CEGUI::EventArgs& e)
      {
        btn = winMgr->getWindow("Options/MovementMode/DropList");
        uint id = ((CEGUI::Combobox*)btn)->getSelectedItem()->getID();

        bool local = true;

        switch(id)
        {
        case 0: // Direct
          local = true;
          break;
        case 1: // Via server
          local = false;
          break;
        default: Report(PT::Error, "OnDropListMovement: failed %d", id);
        }

        app_cfg->SetBool("Client.local_movement", local);
        SaveConfig();
        return true;
      }

      void OptionsWindow::CreateDropListMovement()
      {

        btn = winMgr->getWindow("Options/MovementMode/DropList");

        bool local = app_cfg->GetBool("Client.local_movement");

        if (local)
          ((CEGUI::Combobox*)btn)->setText("Direct");
        else
          ((CEGUI::Combobox*)btn)->setText("Via server");

        CEGUI::ListboxItem* charIdItem = new CEGUI::ListboxTextItem((CEGUI::utf8*)"Direct", 0);
        ((CEGUI::Combobox*)btn)->addItem(charIdItem);

        charIdItem = new CEGUI::ListboxTextItem((CEGUI::utf8*)"Via server", 1);
        ((CEGUI::Combobox*)btn)->addItem(charIdItem);

        ((CEGUI::Combobox*)btn)->setReadOnly(true);

      }

      bool OptionsWindow::OnFullScreenCheckBox(const CEGUI::EventArgs& e)
      {
        btn = winMgr->getWindow("Video/Fullscreen");
        bool fs = ((CEGUI::Checkbox*)btn)->isSelected();

        app_cfg->SetBool("Video.FullScreen", fs);
        SaveConfig();
        return true;
      }

      void OptionsWindow::CreateFullScreenCheckBox()
      {
        btn = winMgr->getWindow("Video/Fullscreen");

        bool fs = app_cfg->GetBool("Video.FullScreen");

        ((CEGUI::Checkbox*)btn)->setSelected(fs);
      }


      bool OptionsWindow::OnReverseCheckBox(const CEGUI::EventArgs& e)
      {
        btn = winMgr->getWindow("Options/Reverse");
        bool fs = ((CEGUI::Checkbox*)btn)->isSelected();

        app_cfg->SetBool("Client.backwardreverse", fs);
        SaveConfig();
        return true;
      }

      void OptionsWindow::CreateReverseCheckBox()
      {
        btn = winMgr->getWindow("Options/Reverse");

        bool fs = app_cfg->GetBool("Client.backwardreverse");

        ((CEGUI::Checkbox*)btn)->setSelected(fs);
      }

      bool OptionsWindow::OnYAxisCheckBox(const CEGUI::EventArgs& e)
      {
        btn = winMgr->getWindow("Options/Invert_Y_Axis");
        bool ya = ((CEGUI::Checkbox*)btn)->isSelected();

        app_cfg->SetBool("Client.invertYAxis", ya);
        SaveConfig();
        return true;
      }

      void OptionsWindow::CreateYAxisCheckBox()
      {
        btn = winMgr->getWindow("Options/Invert_Y_Axis");

        bool ya = app_cfg->GetBool("Client.invertYAxis");

        ((CEGUI::Checkbox*)btn)->setSelected(ya);
      }

      bool OptionsWindow::OnMinFPSSpinnerChanged(const CEGUI::EventArgs &e)
      {
        btn = winMgr->getWindow("Video/Minimum_FPS/Spinner");
        float value = ((CEGUI::Spinner*)btn)->getCurrentValue();

        app_cfg->SetFloat("Client.minFPS", value);
        SaveConfig();
        return true;
      }

      bool OptionsWindow::OnMaxFPSSpinnerChanged(const CEGUI::EventArgs &e)
      {
        btn = winMgr->getWindow("Video/Maximum_FPS/Spinner");
        float value = ((CEGUI::Spinner*)btn)->getCurrentValue();

        app_cfg->SetFloat("Client.maxFPS", value);
        SaveConfig();
        return true;
      }

      bool OptionsWindow::OnMinDistanceSpinnerChanged(const CEGUI::EventArgs &e)
      {
        btn = winMgr->getWindow("Video/Minimum_Distance/Spinner");
        float value = ((CEGUI::Spinner*)btn)->getCurrentValue();

        app_cfg->SetFloat("Client.minDistance", value);
        SaveConfig();
        return true;
      }

      void OptionsWindow::CreateAdaptiveSpinners()
      {
        btn = winMgr->getWindow("Video/Minimum_FPS/Spinner");
        ((CEGUI::Spinner*)btn)->setCurrentValue(app_cfg->GetFloat("Client.minFPS"));

        btn = winMgr->getWindow("Video/Maximum_FPS/Spinner");
        ((CEGUI::Spinner*)btn)->setCurrentValue(app_cfg->GetFloat("Client.maxFPS"));

        btn = winMgr->getWindow("Video/Minimum_Distance/Spinner");
        ((CEGUI::Spinner*)btn)->setCurrentValue(app_cfg->GetFloat("Client.minDistance"));

      }

      bool OptionsWindow::Create()
      {
        ReloadWindow();
        return true;
      }

      bool OptionsWindow::ReloadWindow()
      {
        window = GUIWindow::LoadLayout ("client/options.xml");
        GUIWindow::AddToRoot(window);

        winMgr = cegui->GetWindowManagerPtr ();

        // Hide the Option menu.
        btn = winMgr->getWindow("Options/Frame");
        btn->setVisible(false);

        app_cfg =  csQueryRegistry<iConfigManager> (PointerLibrary::getInstance()->getClient()->GetObjectRegistry());
        if (!app_cfg)
        {
          Report(PT::Error, "Can't find the config manager!");
          return false;
        }

        vfs =  csQueryRegistry<iVFS> (PointerLibrary::getInstance()->getClient()->GetObjectRegistry());
        if (!vfs)
        {
          Report(PT::Error, "Can't find the vfs!");
          return false;
        }

        // Set up the close button.
        btn = winMgr->getWindow("Options/Frame");
        btn->subscribeEvent(CEGUI::FrameWindow::EventCloseClicked, CEGUI::Event::Subscriber(&OptionsWindow::OkButtonPressed, this));

        // Set up the button behaviour for Reflections.
        CreateDropListReflections();
        btn = winMgr->getWindow("Video/Reflections/DropList");
        btn->subscribeEvent(CEGUI::Combobox::EventListSelectionAccepted, CEGUI::Event::Subscriber(&OptionsWindow::OnDropListReflections, this));

        // Set up the button behaviour for Texture quality.
        CreateDropListTexture();
        btn = winMgr->getWindow("Video/TextureQuality/DropList");
        btn->subscribeEvent(CEGUI::Combobox::EventListSelectionAccepted, CEGUI::Event::Subscriber(&OptionsWindow::OnDropListTexture, this));

        // Set up the button behaviour for Movement Mode.
        CreateDropListMovement();
        btn = winMgr->getWindow("Options/MovementMode/DropList");
        btn->subscribeEvent(CEGUI::Combobox::EventListSelectionAccepted, CEGUI::Event::Subscriber(&OptionsWindow::OnDropListMovement, this));

        // Set up the OK button.
        btn = winMgr->getWindow("Options/OK_Button");
        btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&OptionsWindow::OkButtonPressed, this));

        // Set up the Options button.
        btn = winMgr->getWindow("Options/Options_Button");
        winMgr->getWindow("Root")->addChildWindow(btn);
        btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&OptionsWindow::OptionButtonPressed, this));

        // Set up the Fullscreen checkbox.
        CreateFullScreenCheckBox();
        btn = winMgr->getWindow("Video/Fullscreen");
        btn->subscribeEvent(CEGUI::Checkbox::EventCheckStateChanged, CEGUI::Event::Subscriber(&OptionsWindow::OnFullScreenCheckBox, this));

        // Set up the Backward reverse checkbox.
        CreateReverseCheckBox();
        btn = winMgr->getWindow("Options/Reverse");
        btn->subscribeEvent(CEGUI::Checkbox::EventCheckStateChanged, CEGUI::Event::Subscriber(&OptionsWindow::OnReverseCheckBox, this));

        // Set up the Invert Y Axis checkbox.
        CreateYAxisCheckBox();
        btn = winMgr->getWindow("Options/Invert_Y_Axis");
        btn->subscribeEvent(CEGUI::Checkbox::EventCheckStateChanged, CEGUI::Event::Subscriber(&OptionsWindow::OnYAxisCheckBox, this));

        // Set up the Adaptive distance clipping spinners.
        CreateAdaptiveSpinners();
        btn = winMgr->getWindow("Video/Minimum_FPS/Spinner");
        btn->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&OptionsWindow::OnMinFPSSpinnerChanged, this));
        btn = winMgr->getWindow("Video/Maximum_FPS/Spinner");
        btn->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&OptionsWindow::OnMaxFPSSpinnerChanged, this));
        btn = winMgr->getWindow("Video/Minimum_Distance/Spinner");
        btn->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&OptionsWindow::OnMinDistanceSpinnerChanged, this));
        return true;
      }

      bool OptionsWindow::CreateOptionItem(const char* optionname)
      {
        // Get the window.
        CEGUI::Window* btn = guimanager->GetCeguiWindow("Options/List");
        if (!btn) return false;

        // Create and add the item to the list window.
        CEGUI::ListboxItem* optionItem = new CEGUI::ListboxTextItem(optionname); // CEGUI deletes this.
        optionItem->setSelectionBrushImage("Peragro", "TextSelectionBrush");
        ((CEGUI::Listbox*)btn)->addItem(optionItem);

        return true;
      }

      bool OptionsWindow::AddOption(const char* optionName, const char* windowName)
      {
        CreateOptionItem(optionName);

        // Add the option to the array.
        Option option;
        option.name = optionName;
        option.window = windowName;
        optionList.Push(option);

        return true;
      }
    }
  }
}
