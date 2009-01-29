/*
    Copyright (C) 2005 Development Team of Peragro Tempus

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General public License for more details.

    You should have received a copy of the GNU General public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef BASE_GUI_H
#define BASE_GUI_H

#include <cssysdef.h>

#include <iutil/string.h>
#include <iutil/vfs.h>
#include <iutil/document.h>

#include <CEGUI.h>
#include <ivaria/icegui.h>

#include "common/event/inputevent.h"
#include "common/network/netmessage.h"
#include "common/util/mutex.h"

#include "common/event/eventmanager.h"

#define ROOTWINDOW "Root"

class Network;

namespace PT
{
  namespace GUI
  {
    class GUIManager;

    class GUIWindow
    {
    private:

    protected:
      std::string windowName;
      CEGUI::Window* btn;
      CEGUI::WindowManager* winMgr;
      csRef<iCEGUI> cegui;
      csRef<iVFS> vfs;
      Network* network;
      CEGUI::Window* window;

      GUIManager* guimanager;

      CEGUI::Window* LoadLayout(const char* layoutFile);
      bool AddToRoot(CEGUI::Window* window);
      void SetupToggleListener(const std::string& name);

      PT_CALLBACK_HANDLER_LISTENERS

      bool ToggleWindow(iEvent& ev);

    public:
      GUIWindow(GUIManager* guimanager);
      virtual ~GUIWindow();
      /// Get the window name.
      const char* GetName() { return windowName.c_str(); }
      /// Set the window name.
      void SetName(const char* name) { windowName = name; }
      virtual bool Create() = 0;
      /// Loads the layout and subscribes to events.
      virtual bool ReloadWindow() = 0;

      CEGUI::Window* GetCeguiWindow() { return window; }

      void HideWindow();
      void ShowWindow();
      void ToggleWindow();
      bool IsVisible();
      void EnableWindow();
      void DisableWindow();
      void BringToFront();
    };
  } // GUI namespace
} // PT namespace

#endif // BASE_GUI_H
