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

#ifndef GUIMANAGER_H
#define GUIMANAGER_H

#include <cssysdef.h>
#include <csutil/ref.h>
#include <iutil/vfs.h>

//#include <ivaria/icegui.h>
#include <CEGUI.h>

#include "common/dragdrop-gui.h"

struct iObjectRegistry;
struct iVFS;
struct iCEGUI;

namespace PT
{
  namespace GUI
  {
    namespace Windows
    {
      class LoginWindow;
      class ServerWindow;
      class SelectCharWindow;
      class ServerSetupWindow;
      class ChatWindow;
      class InventoryWindow;
      class HUDWindow;
      class OptionsWindow;
      class StatusWindow;
      class BuddyWindow;
      class WhisperWindow;
      class NpcDialogWindow;
      class TradeWindow;
      class ConfirmDialogWindow;
      class OkDialogWindow;
      class BuyWindow;
      class SellWindow;
      class InteractDialogWindow;
      class BookWindow;
      class LoadScreenWindow;
      class SkinWindow;
      class MenuWindow;
	  class DragDrop;
	}
	class SkinManager;
	class GUIWindow;
	class DialogConfiguration;
    class MenuManager;
    

	class GUIManager
    {
    private:  
      csRef<iCEGUI> cegui;
      csRef<iObjectRegistry> obj_reg;
      SkinManager* skinMgr;
      DialogConfiguration * dlgConfig;
	  PT::GUI::MenuManager * menuMgr;
      PT::GUI::Windows::DragDrop* dragdrop;
      bool isInitialized;

      CEGUI::SchemeManager* schMgr;
      CEGUI::WindowManager* winMgr;
      CEGUI::System* system;

      csArray<PT::GUI::GUIWindow*> windows;

    public:
      GUIManager ();
      ~GUIManager ();

      /// Initialize the manager.
      bool Initialize ();
      /// Renders CEGUI to the screen.
      void Render ();

      /// Returns the cegui singleton.
      iCEGUI* GetCEGUI () {return cegui;}
      /// Returns the ObjectRegistry.
      iObjectRegistry* GetObjectRegistry () {return obj_reg;}
      /// Returns the skin manager.
      SkinManager* GetSkinMananger () {return skinMgr;}
      /// Returns windows array.
      csArray<GUIWindow*> GetWindows () {return windows;}

      /// Return a window by name.
      template<class Interface>
      Interface* GetWindow (const char* name)
      {
        for (size_t i = 0; i < windows.GetSize();i++)
        {
          GUIWindow* win = windows.Get(i);
          if (win && strcmp(win->GetName(), name) == 0)
            return (Interface*)win;
        }// for

        printf("E: Couldn't find window: '%s'\n", name);
        return 0;
      }

      /// Get a CEGUI window by name.
      CEGUI::Window* GetCeguiWindow (const char* name);
      /// Creates the root window.
      bool CreateRootWindow ();
      /// Verifies if the guimanager has completed initialization.
	  bool IsInitialized();

      void SavePositions ();
      void Reload ();

      MenuManager * GetMenuManager() { return menuMgr; }
	  PT::GUI::Windows::DragDrop* GetDragDrop (){return dragdrop;}
	};
}
}
#endif // GUIMANAGER_H
