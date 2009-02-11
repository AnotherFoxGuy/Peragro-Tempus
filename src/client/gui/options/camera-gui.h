/*
  Copyright (C) 2005 - 2007 Development Team of Peragro Tempus

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

#ifndef PT_GUI_CAMERAOPTIONSWINDOW_H
#define PT_GUI_CAMERAOPTIONSWINDOW_H

#include "client/gui/base-gui.h"

#define CAMERAOPTIONSWINDOW "Options/Camera"

namespace PT
{
  namespace GUI
  {
    namespace Windows
    {
      class CameraOptionsWindow : public GUIWindow
      {
      private:
        csRef<iConfigManager> app_cfg;

        void SendUpdateEvent();
        bool ToggleDistClip(iEvent& e);

      private:
        void CreateAdaptiveClippingCheckBox();
        bool OnAdaptiveClippingCheckBox(const CEGUI::EventArgs& e);

      public:
        CameraOptionsWindow(GUIManager* guiManager);
        virtual ~CameraOptionsWindow();
        bool Create();
        bool ReloadWindow();

      };

    } // Windows namespace
  } // GUI namespace
} // PT namespace

#endif // PT_GUI_CAMERAOPTIONSWINDOW_H
