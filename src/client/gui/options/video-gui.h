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

#ifndef PT_GUI_VIDEOOPTIONSWINDOW_H
#define PT_GUI_VIDEOOPTIONSWINDOW_H

#include "client/gui/base-gui.h"

#define VIDEOOPTIONSWINDOW "Options/Video"

namespace PT
{
  namespace GUI
  {
    namespace Windows
    {
      class VideoOptionsWindow : public GUIWindow
      {
      private:
        csRef<iConfigManager> app_cfg;

        void SendUpdateEvent();
        void SaveConfig();

      private:
        bool OnDropListResolution(const CEGUI::EventArgs& e);
        void CreateDropListResolution();

        void CreateFullScreenCheckBox();
        bool OnFullScreenCheckBox(const CEGUI::EventArgs& e);

        bool OnDropListTextureQuality(const CEGUI::EventArgs& e);
        void CreateDropListTextureQuality();

        bool OnDropListTextureFiltering(const CEGUI::EventArgs& e);
        void CreateDropListTextureFiltering();

        bool OnMultisampleSliderChanged(const CEGUI::EventArgs &e);
        void CreateMultisampleSlider();

        bool OnAnisotropySliderChanged(const CEGUI::EventArgs &e);
        void CreateAnisotropySlider();

        bool OnDropListReflectionsQuality(const CEGUI::EventArgs& e);
        void CreateDropListReflectionsQuality();

        bool OnReflectionUpdateIntervalSliderChanged(const CEGUI::EventArgs &e);
        void CreateReflectionUpdateIntervalSlider();

        bool OnDropListTerrainViewDistance(const CEGUI::EventArgs& e);
        void CreateDropListTerrainViewDistance();

      public:
        VideoOptionsWindow(GUIManager* guiManager);
        virtual ~VideoOptionsWindow();
        bool Create();
        bool ReloadWindow();

      };

    } // Windows namespace
  } // GUI namespace
} // PT namespace

#endif // PT_GUI_VIDEOOPTIONSWINDOW_H
