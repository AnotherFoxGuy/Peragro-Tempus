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

#ifndef PT_GUI_SKINMANAGER_H
#define PT_GUI_SKINMANAGER_H

#include <cssysdef.h>
#include <csutil/ref.h>
#include <csutil/array.h>

#include <CEGUI.h>

#include "client/event/event.h"

struct iObjectRegistry;
struct iVFS;
struct iCEGUI;
struct Skin;
struct iConfigManager;

namespace PT
{
  namespace Events
  {
    class EventManager;
  }
  namespace GUI
  {
    class GUIManager;

    /**
    * Holds information
    * over a GUI skin.
    */
    struct Skin
    {
      std::string name;
      std::string path;

      const char* GetName() const { return name.c_str();}
      const char* GetPath() const { return path.c_str();}
    };

    /**
    * Manages all GUI skins and
    * related tasks.
    */
    class SkinManager
    {
    private:
      Skin defaultSkin;
      Skin currentSkin;

    private:  
      csRef<iCEGUI> cegui;
      csRef<iObjectRegistry> obj_reg;
      csRef<iVFS> vfs;
      GUIManager* guiManager;
      csRef<iConfigManager> app_cfg;

      CEGUI::SchemeManager* schMgr;
      CEGUI::WindowManager* winMgr;
      CEGUI::System* system;

      csArray<Skin> skinList;

      Skin FindSkin (const char* name);
      void SearchForSkins (const char* path);
      bool LoadDefaultSkin ();
      bool ReloadWindows ();

    private:
      bool LoadPressed (iEvent &ev);

    public:
      SkinManager (GUIManager* guiManager);
      ~SkinManager ();

      /// Initialize the manager.
      bool Initialize ();
      /// Load a layout.
      CEGUI::Window* loadLayout (const char* layoutFile);
      /// Change the skin.
      bool ChangeSkin (const char* skinName);
      /// Add the skinlist to the GUI.
      bool Populate ();
	  /// Return the current skin of the GUI.
	  Skin GetCurrentSkin();

    };
  } // GUI namespace
} // PT namespace

#endif // PT_GUI_SKINMANAGER_H
