/*
  Copyright (C) 2005 - 2008 Development Team of Peragro Tempus

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

#ifndef PT_GUI_MOVEMENTOPTIONSWINDOW_H
#define PT_GUI_MOVEMENTOPTIONSWINDOW_H

#include "client/gui/base-gui.h"

#define MOVEMENTOPTIONSWINDOW "Options/Movement"

namespace PT
{
  namespace GUI
  {
    namespace Windows
    {
      class MovementOptionsWindow : public GUIWindow
      {
      private:
        csRef<iConfigManager> app_cfg;

        void SaveConfig();

        void SendUpdateEvent();

      private:
        bool OnDropListMovement(const CEGUI::EventArgs& e);
        void CreateDropListMovement();

        void CreateReverseCheckBox();
        bool OnReverseCheckBox(const CEGUI::EventArgs& e);

        void CreateYAxisCheckBox();
        bool OnYAxisCheckBox(const CEGUI::EventArgs& e);

      public:
        MovementOptionsWindow(GUIManager* guiManager);
        virtual ~MovementOptionsWindow();
        bool Create();
      };

    } // Windows namespace
  } // GUI namespace
} // PT namespace

#endif // PT_GUI_MOVEMENTOPTIONSWINDOW_H
