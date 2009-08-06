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

#include "client/gui/guimanager.h"

#include "client/gui/skinmanager.h"
#include "client/gui/menumanager.h"
#include "client/gui/dialogconfiguration.h"
#include "client/gui/common/dragdrop-gui.h"

#include <iutil/objreg.h>

#include <CEGUI.h>
#include <CEGUIWindowManager.h>
#include <CEGUILogger.h>

#include "client/network/network.h"

#include "gui.h"

#include "common/reporter/reporter.h"

//Custom windows
#include "config/settingslider.h"
#include "config/settingcombobox.h"

const char* const dialogConfigFile = "/UserData/dialog.cfg";

namespace PT
{
  namespace GUI
  {
    GUIManager::GUIManager()
      : skinMgr(0), dlgConfig(0), menuMgr(0), dragdrop(0), schMgr(0),
      winMgr(0), system(0)
    {
    } // end GUIManager()

    GUIManager::~GUIManager ()
    {

      if (!dlgConfig->SaveConfiguration(dialogConfigFile))
      {
        Report(PT::Warning,"Failed saving dialog.cfg!\n");
      }
      else
      {
        Report(PT::Notify,"Saving dialog.cfg!\n");
      }

      // Delete GUIWindows
      for (size_t i = 0; i < windows.GetSize(); ++i)
      {
        delete windows[i];
      }

      delete skinMgr;
      delete dlgConfig;
      delete menuMgr;
      delete dragdrop;
    } // end ~GUIManager()

    CEGUI::Window* GUIManager::GetCeguiWindow(const char* name)
    {
      if (winMgr->isWindowPresent(name))
      {
        return winMgr->getWindow(name);
      }
      else
      {
        Report(PT::Error, "Couldn't find CEGUI window: '%s'", name);
        return 0;
      }
    } // end GetCeguiWindow()

    bool GUIManager::CreateRootWindow()
    {
      if (winMgr->isWindowPresent(ROOTWINDOW)) return false;

      // Create the Root window and set it as the GUI sheet.
      CEGUI::Window* root = winMgr->createWindow("DefaultWindow",ROOTWINDOW);
      root->setArea(CEGUI::UVector2(CEGUI::UDim(0.0f,0), CEGUI::UDim(0.0f,0)),
        CEGUI::UVector2(CEGUI::UDim(1.0f,0), CEGUI::UDim(1.0f,0)));
      root->setMaxSize(CEGUI::UVector2(CEGUI::UDim(1.0f,0), CEGUI::UDim(1.0f,0)));
      system->setGUISheet(root);
      return true;
    } // end CreateRootWindow()

    bool GUIManager::Initialize()
    {
      using namespace CEGUI;

      obj_reg = PointerLibrary::getInstance()->getObjectRegistry();
      if (!obj_reg) return Report(PT::Error, "Failed to locate ObjectRegistry!");

      cegui =  csQueryRegistry<iCEGUI> (obj_reg);
      if (!cegui) return Report(PT::Error, "Failed to locate CEGUI plugin!");

      try
      {

        // Initialize CEGUI wrapper
        cegui->Initialize ();

        // Set the logging level
        cegui->GetLoggerPtr ()->setLoggingLevel(CEGUI::Informative);

        // Register our own window factories.
        CEGUI::WindowFactoryManager* wfMgr = cegui->GetWindowFactoryManagerPtr();
        wfMgr->addFactory(&CEGUI_WINDOW_FACTORY(SettingSlider));
        //wfMgr->addFactory<SettingSliderFactory>();
        wfMgr->addFalagardWindowMapping("Peragro/SettingSlider",
                                        "CEGUI/SettingSlider",
                                        "Peragro/SettingSlider",
                                        "Falagard/Default");

        wfMgr->addFactory(&CEGUI_WINDOW_FACTORY(SettingComboBox));
        //wfMgr->addFactory<SettingSliderFactory>();
        wfMgr->addFalagardWindowMapping("Peragro/SettingComboBox",
                                        "CEGUI/SettingComboBox",
                                        "Peragro/SettingComboBox",
                                        "Falagard/Default");

        // Initialize the skin manager.
        skinMgr = new SkinManager(this);
        if (!skinMgr->Initialize()) return false;

        // Initialize the dialog configuration class.
        dlgConfig = new DialogConfiguration(obj_reg);

        // Initialize the menu manager.
        menuMgr = new PT::GUI::MenuManager();

        csRef<iVFS> vfs =  csQueryRegistry<iVFS> (obj_reg);
        vfs->ChDir ("/peragro/art/skins/");

        // Get some pointers.
        schMgr = cegui->GetSchemeManagerPtr();
        winMgr = cegui->GetWindowManagerPtr();
        system = cegui->GetSystemPtr();

        // Create the Root window and set it as the GUI sheet.
        CEGUI::Window* root = cegui->GetWindowManagerPtr()->
          createWindow("DefaultWindow","Root");
        root->setArea(CEGUI::UVector2(CEGUI::UDim(0.0f,0), CEGUI::UDim(0.0f,0)),
          CEGUI::UVector2(CEGUI::UDim(1.0f,0), CEGUI::UDim(1.0f,0)));
        root->setMaxSize(CEGUI::UVector2(CEGUI::UDim(1.0f,0),
          CEGUI::UDim(1.0f,0)));
        cegui->GetSystemPtr()->setGUISheet(root);

        // Set up the drag and drop.
        dragdrop = new PT::GUI::Windows::DragDrop (this);
        if (!dragdrop)
          return Report(PT::Error, "Failed to create DragDrop class!");

        isInitialized = false;

        // Create windows.
        PT::GUI::Windows::OptionsWindow* options =
          new PT::GUI::Windows::OptionsWindow(this);
        options->Create();
        windows.Push(options);

        PT::GUI::Windows::SkinOptionsWindow* skins =
          new PT::GUI::Windows::SkinOptionsWindow(this);
        skins->Create();
        windows.Push(skins);

        PT::GUI::Windows::VideoOptionsWindow* video =
          new PT::GUI::Windows::VideoOptionsWindow(this);
        video->Create();
        windows.Push(video);

        PT::GUI::Windows::CameraOptionsWindow* camera =
          new PT::GUI::Windows::CameraOptionsWindow(this);
        camera->Create();
        windows.Push(camera);

        PT::GUI::Windows::MovementOptionsWindow* movement =
          new PT::GUI::Windows::MovementOptionsWindow(this);
        movement->Create();
        windows.Push(movement);

        PT::GUI::Windows::ControlOptionsWindow* controls =
          new PT::GUI::Windows::ControlOptionsWindow(this);
        controls->Create();
        windows.Push(controls);

        PT::GUI::Windows::ChatOptionsWindow* chatoptions =
          new PT::GUI::Windows::ChatOptionsWindow(this);
        chatoptions->Create();
        windows.Push(chatoptions);

        options->AddOption("Skins", skins->GetName());
        skinMgr->Populate();
        options->AddOption("Video", video->GetName());
        options->AddOption("Camera", camera->GetName());
        options->AddOption("Movement", movement->GetName());
        options->AddOption("Controls", controls->GetName());
        options->AddOption("Chat", chatoptions->GetName());

        // Create the windows to be registered and used later.
        PT::GUI::Windows::ServerWindow* server =
          new PT::GUI::Windows::ServerWindow(this);
        server->Create();
        windows.Push(server);

        PT::GUI::Windows::LoginWindow* login =
          new PT::GUI::Windows::LoginWindow(this);
        login->Create();
        windows.Push(login);

        PT::GUI::Windows::MenuWindow* menu =
          new PT::GUI::Windows::MenuWindow(this);
        menu->Create();
        windows.Push(menu);

        PT::GUI::Windows::BuyWindow* buy =
          new PT::GUI::Windows::BuyWindow(this);
        buy->Create();
        windows.Push(buy);

        PT::GUI::Windows::ChatWindow* chat =
          new PT::GUI::Windows::ChatWindow(this);
        chat->Create();
        windows.Push(chat);

        PT::GUI::Windows::ChatInputWindow* chatinput =
          new PT::GUI::Windows::ChatInputWindow(this);
        chatinput->Create();
        windows.Push(chatinput);

        PT::GUI::Windows::StatsHUDWindow* statshud =
          new PT::GUI::Windows::StatsHUDWindow(this);
        statshud->Create();
        windows.Push(statshud);

        PT::GUI::Windows::SkillsHUDWindow* skillshud =
          new PT::GUI::Windows::SkillsHUDWindow(this);
        skillshud->Create();
        windows.Push(skillshud);

        PT::GUI::Windows::SelectCharWindow* charsel =
          new PT::GUI::Windows::SelectCharWindow(this);
        charsel->Create();
        windows.Push(charsel);

        PT::GUI::Windows::SellWindow* sell =
          new PT::GUI::Windows::SellWindow(this);
        sell->Create();
        windows.Push(sell);

        PT::GUI::Windows::StatusWindow* status =
          new PT::GUI::Windows::StatusWindow(this);
        status->Create();
        windows.Push(status);

        PT::GUI::Windows::TradeWindow* trade =
          new PT::GUI::Windows::TradeWindow(this);
        trade->Create();
        windows.Push(trade);

        PT::GUI::Windows::ServerSetupWindow* serverSetupWindow =
          new PT::GUI::Windows::ServerSetupWindow(this);
        serverSetupWindow->Create();
        windows.Push(serverSetupWindow);

        PT::GUI::Windows::WhisperWindow* whisperWindow =
          new PT::GUI::Windows::WhisperWindow(this);
        whisperWindow->Create();
        windows.Push(whisperWindow);

        PT::GUI::Windows::NpcDialogWindow* npcWindow =
          new PT::GUI::Windows::NpcDialogWindow(this);
        npcWindow->Create();
        windows.Push(npcWindow);

        PT::GUI::Windows::InventoryWindow* inventoryWindow =
          new PT::GUI::Windows::InventoryWindow(this);
        inventoryWindow->Create();
        windows.Push(inventoryWindow);

        PT::GUI::Windows::BuddyWindow* buddyWindow =
          new PT::GUI::Windows::BuddyWindow(this);
        buddyWindow->Create();
        windows.Push(buddyWindow);

        PT::GUI::Windows::LoadScreenWindow* loadScreenWindow =
          new PT::GUI::Windows::LoadScreenWindow(this);
        loadScreenWindow->Create();
        windows.Push(loadScreenWindow);

        PT::GUI::Windows::BackgroundWindow* backgroundWindow =
          new PT::GUI::Windows::BackgroundWindow(this);
        backgroundWindow->Create();
        windows.Push(backgroundWindow);

        PT::GUI::Windows::InteractDialogWindow* interactDialogWindow =
          new PT::GUI::Windows::InteractDialogWindow(this);
        interactDialogWindow->Create();
        windows.Push(interactDialogWindow);

        PT::GUI::Windows::BookWindow* bookWindow =
          new PT::GUI::Windows::BookWindow(this);
        bookWindow->Create();
        windows.Push(bookWindow);

        // Dialogs to save position, size and visable properties
        dlgConfig->AddDialog(server->GetName());
        dlgConfig->AddDialog(buy->GetName());
        dlgConfig->AddDialog(chat->GetName());
        dlgConfig->AddDialog(chatinput->GetName());
        dlgConfig->AddDialog(skillshud->GetName());
        dlgConfig->AddDialog(statshud->GetName());
        dlgConfig->AddDialog(status->GetName());
        dlgConfig->AddDialog(sell->GetName());
        dlgConfig->AddDialog(npcWindow->GetName());
        dlgConfig->AddDialog(inventoryWindow->GetName());
        dlgConfig->AddDialog(buddyWindow->GetName());
        dlgConfig->AddDialog(bookWindow->GetName());


        if (!dlgConfig->LoadConfiguration(dialogConfigFile))
        {
          Report(PT::Warning, "Unable to load dialogs settings!");
        }

        isInitialized = true;
      }
      catch ( CEGUI::Exception& e)
      {
        Report(PT::Error, "Failed Initializing GUIManager! %s",
          e.getMessage().c_str());
        return false;
      }

      return true;
    } // end Initialize()

    void GUIManager::Render()
    {
      cegui->Render ();
    } // end Render()

    void GUIManager::SavePositions()
    {
      dlgConfig->SavePositions();
    } // end SavePositions()

    void GUIManager::Reload()
    {
      menuMgr->Reload();
      dlgConfig->Reload();
    } // end Reload()

    bool GUIManager::IsInitialized()
    {
      return isInitialized;
    } // end IsInitialized()

  } // GUI namespace
} // PT namespace

