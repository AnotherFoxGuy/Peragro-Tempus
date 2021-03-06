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

#include "skinmanager.h"
#include "gui.h"
#include "guimanager.h"
#include "common/eventcs/eventmanager.h"
#include "common/reporter/reporter.h"

#include <iutil/vfs.h>
#include <iutil/objreg.h>
#include <iutil/stringarray.h>
#include <iutil/cfgmgr.h>

#include <CEGUI.h>
#include <CEGUIWindowManager.h>
#include <CEGUILogger.h>
#include <ivaria/icegui.h>

#include "client/pointer/pointer.h"
#include "client/gui/options/skin-gui.h"

namespace PT
{
  namespace GUI
  {
    SkinManager::SkinManager(GUIManager* guiManager)
      : guiManager(guiManager)
    {
      iObjectRegistry* obj_reg =
        PointerLibrary::getInstance()->getObjectRegistry();
      vfs = csQueryRegistry<iVFS>(obj_reg);
      cegui = guiManager->GetCEGUI ();
      app_cfg = csQueryRegistry<iConfigManager>(obj_reg);
      std::string skinpath = "/peragro/art/skins/";
      skinpath += app_cfg->GetStr("Peragro.Skin");
      skinpath += "/";

      defaultSkin.name = "Peragro";
      defaultSkin.path = skinpath;
    } // end SkinManager()

    SkinManager::~SkinManager()
    {
    } // end ~SkinManager()

    bool SkinManager::LoadPressed(iEvent &ev)
    {
      using namespace CEGUI;

      Windows::SkinOptionsWindow* skinWindow =
        guiManager->GetWindow<Windows::SkinOptionsWindow>("SkinOptionsWindow");
      if (!skinWindow) return false;

      Skin* skin = skinWindow->GetSelectedSkin();
      if (!skin) return false;

      if (skin->name == currentSkin.name && skin->path == currentSkin.path)
      {
        Report(PT::Notify, "User attempted to load skin already in use.");
        return false;
      }
      else
      {
        ChangeSkin(skin->GetName());
      }

      return true;
    } // end LoadPressed()

    Skin& SkinManager::FindSkin(const char* name)
    {
      for (size_t i = 0; i < skinList.GetSize();i++)
      {
        Skin& skin = skinList.Get(i);
        if (strcmp(skin.GetName(), name) == 0)
          return skin;
      } // for

      Report(PT::Warning, "Couldn't find skin: '%s'", name);
      return defaultSkin;
    } // end FindSkin()

    void SkinManager::SearchForSkins(const char* path)
    {
      Report(PT::Notify,"Looking for skins:");

      // Clear the skinlist and add the default.
      skinList.DeleteAll();
      skinList.Push(defaultSkin);
      Report(PT::Notify, "Found '%s' at '%s'", defaultSkin.GetName(),
        defaultSkin.GetPath());

      // Search for directories in the path.
      vfs->ChDir (path);
      csRef<iStringArray> files = vfs->FindFiles(path);
      for (size_t i = 0; i < files->GetSize();i++)
      {
        std::string file(files->Get(i));
        std::string spath(path);
        std::string name(file.substr(spath.length(), file.length()));
        name = name.substr(0, name.length()-1);
        // Check if the name doesn't start with '.', a hidden file.
        if (name.substr(0,1).compare(".") == 0) continue;
        // Check if the name isn't default.
        if (name.compare("default") == 0) continue;

        // Add 'Peragro_' to the name.
        std::string skinName = "Peragro_";
        skinName += name;
        Report(PT::Notify, "Found '%s' at '%s'", skinName.c_str(),
          file.c_str());
        Skin skin;
        skin.name = skinName;
        skin.path = file;
        skinList.Push(skin);
      } // for

      Report(PT::Debug, "");
    } // end SearchForSkins()

    bool SkinManager::LoadDefaultSkin()
    {
      vfs->ChDir(defaultSkin.GetPath());

      // Load the default skin.
      cegui->GetSchemeManagerPtr()->create("widgets/peragro.scheme");
      cegui->GetSchemeManagerPtr()->create("widgets/alias.scheme");

      // Set the default mouse cursor.
      cegui->GetSystemPtr()->
        setDefaultMouseCursor(defaultSkin.GetName(), "MouseArrow");

      // Load the default font.
      cegui->GetFontManagerPtr()->createFreeTypeFont("Vera", 10, true,
        "/peragro/art/skins/default/font/vera.ttf");

      cegui->GetFontManagerPtr()->createFreeTypeFont("Commonwealth-8", 8, true,
        "/peragro/art/skins/default/font/commonv2c.ttf");

      cegui->GetFontManagerPtr()->createFreeTypeFont("Commonwealth-10", 10, true,
        "/peragro/art/skins/default/font/commonv2c.ttf");

      cegui->GetFontManagerPtr()->createFreeTypeFont("CommonWealth", 10, true,
        "/peragro/art/skins/default/font/commonv2c.ttf");

      cegui->GetFontManagerPtr()->createFreeTypeFont("Tahoma-12", 12, true,
        "/peragro/art/skins/default/font/commonv2c.ttf");

      currentSkin = defaultSkin;
      Report(PT::Notify, "Current skin is: '%s' at '%s'",
        currentSkin.GetName(), currentSkin.GetPath());

      return true;
    } // end LoadDefaultSkin()

    bool SkinManager::ReloadWindows()
    {
      // Create the root window.
      guiManager->CreateRootWindow();

      // Get all windows and reload them.
      csArray<GUIWindow*> windows = guiManager->GetWindows();
      for (size_t i = 0; i < windows.GetSize();i++)
      {
        GUIWindow* win = windows.Get(i);
        if (win)
          win->ReloadWindow();
      } // for
      guiManager->Reload();

      return true;
    } // end ReloadWindows()

    bool SkinManager::Initialize()
    {
      // Get some pointers.
      schMgr = cegui->GetSchemeManagerPtr();
      winMgr = cegui->GetWindowManagerPtr();
      system = cegui->GetSystemPtr();

      if (!LoadDefaultSkin())
        return false;

      SearchForSkins("/peragro/art/skins/");

      using namespace PT::Events;

      // Register listener for interface.
      PT_SETUP_HANDLER
      PT_REGISTER_LISTENER(SkinManager, LoadPressed, "interface.skinwindow.buttons.load")

      return true;
    } // end Initialize()

    CEGUI::Window* SkinManager::loadLayout(const char* layoutFile)
    {
      std::string layoutPath = currentSkin.path;
      layoutPath += "layouts/";
      vfs->ChDir (layoutPath.c_str());

      CEGUI::Window* win = 0;
      try
      {
          win = winMgr->loadWindowLayout(layoutFile);
      }
      catch ( CEGUI::Exception& e )
      {
        Report(PT::Warning, "No layout supplied for: %s, using Default.",
          e.getMessage().c_str());
        layoutPath = defaultSkin.path;
        layoutPath += "layouts/";
        vfs->ChDir (layoutPath.c_str());
        try
        {
          win = winMgr->loadWindowLayout(layoutFile);
        }
        catch ( CEGUI::Exception& e )
        {
          Report(PT::Error, "Failed setting layout: %s!",
            e.getMessage().c_str());
        }
      }
      return win;
    } // end loadLayout()

    bool SkinManager::ChangeSkin(const char* skinname)
    {
      Report(PT::Notify, "Reloading!");

      // Unload.
      guiManager->SavePositions();
      winMgr->destroyAllWindows();
      winMgr->cleanDeadPool();
      system->setDefaultMouseCursor(0);
      schMgr->destroy("Alias");
      // If current skin is different from default
      // or if new skin is default(reload): unload current skin.
      if ((strcmp(defaultSkin.GetName(), currentSkin.GetName()) != 0)
          || strcmp(defaultSkin.GetName(), skinname) == 0)
      {
        if (schMgr->isDefined(currentSkin.GetName()))
        {
          Report(PT::Notify, "Unloading scheme file: '%s'",
            currentSkin.GetName());
          schMgr->get(currentSkin.GetName()).unloadResources();
          schMgr->destroy(currentSkin.GetName());
        }
      }

      // Switch to the new skin.
      Skin& newSkin = FindSkin(skinname);

      currentSkin = newSkin;
      Report(PT::Notify, "Switching to new skin: '%s' at '%s'",
        currentSkin.GetName(), currentSkin.GetPath());

      // Load new scheme.
      try
      {
        vfs->ChDir (currentSkin.GetPath());

        if (!schMgr->isDefined(currentSkin.GetName()))
          schMgr->create("widgets/peragro.scheme");
        schMgr->create("widgets/alias.scheme");
      }
      catch ( CEGUI::Exception& e )
      {
        Report(PT::Error, "Failed switching skin: %s", e.getMessage().c_str());
        // Switch to default skin.
        vfs->ChDir (defaultSkin.GetPath());
        schMgr->create("widgets/alias.scheme");
      }

      // Recreate layouts.
      try
      {
        ReloadWindows();
      }
      catch ( CEGUI::Exception& e )
      {
        Report(PT::Error, "Failed recreating windows: %s",
          e.getMessage().c_str());
      }

      // Set mousepointer.
      try
      {
        system->setDefaultMouseCursor(currentSkin.name, "MouseArrow");
      }
      catch ( CEGUI::Exception& e )
      {
        Report(PT::Error, "Failed switching Mouse cursor: %s",
          e.getMessage().c_str());
      }

      return true;
    } // end ChangeSkin()

    bool SkinManager::Populate()
    {
      Windows::SkinOptionsWindow* skinWindow =
        guiManager->GetWindow<Windows::SkinOptionsWindow>("SkinOptionsWindow");
      if (!skinWindow) return false;

      // Add the skins to the GUI.
      for (size_t i = 0; i < skinList.GetSize();i++)
      {
        Skin skin = skinList.Get(i);
        skinWindow->AddSkin(skin);
      } // for

      return true;
    } // end Populate()
    Skin SkinManager::GetCurrentSkin()
    {
        return currentSkin;
    } // end GetCurrentSkin()

  } // GUI namespace
} // PT namespace

