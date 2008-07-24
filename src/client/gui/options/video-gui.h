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

#ifndef PT_GUI_VIDEOWINDOW_H
#define PT_GUI_VIDEOWINDOW_H

#include "client/gui/base-gui.h"

#include <csutil/array.h>

#define VIDEOWINDOW "VideoWindow"

namespace PT
{
  namespace GUI
  {
    namespace Windows
    {
      class VideoWindow : public GUIWindow
      {
      private:
        csRef<iConfigManager> app_cfg;

        void SaveConfig();

        void SendUpdateEvent();
        bool ToggleDistClip(iEvent& e);

      private:
        bool OnDropListReflections(const CEGUI::EventArgs& e);
        void CreateDropListReflections();

        bool OnDropListTexture(const CEGUI::EventArgs& e);
        void CreateDropListTexture();

        void CreateFullScreenCheckBox();
        bool OnFullScreenCheckBox(const CEGUI::EventArgs& e);

        void CreateAdaptiveClippingCheckBox();
        bool OnAdaptiveClippingCheckBox(const CEGUI::EventArgs& e);

        void CreateAdaptiveSpinners();
        bool OnMinFPSSpinnerChanged(const CEGUI::EventArgs &e);
        bool OnMaxFPSSpinnerChanged(const CEGUI::EventArgs &e);
        bool OnMinDistanceSpinnerChanged(const CEGUI::EventArgs &e);

      public:
        VideoWindow (GUIManager* guiManager);
        virtual ~VideoWindow ();
        bool Create ();
        bool ReloadWindow ();

      };

    } // Windows namespace
  } // GUI namespace
} // PT namespace

#endif // PT_GUI_VIDEOWINDOW_H
