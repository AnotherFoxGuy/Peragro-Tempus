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

#ifndef PT_GUI_CONTROLOPTIONSWINDOW_H
#define PT_GUI_CONTROLOPTIONSWINDOW_H

#include "client/gui/base-gui.h"

#define CONTROLOPTIONSWINDOW "Options/Controls"

namespace PT
{
  namespace GUI
  {
    namespace Windows
    {
      class ControlOptionsWindow : public GUIWindow
      {
      private:
        csRef<iConfigManager> app_cfg;

        enum COLUMN_INDEX
        {
          ACTION_INDEX = 0,
          CONTROL1_INDEX = 1,
          CONTROL2_INDEX = 2
        };
        enum CONTROL_INDEX
        {
          CONTROL_NONE = -1,
          CONTROL1 = CONTROL1_INDEX,
          CONTROL2 = CONTROL2_INDEX
        };

        CEGUI::MultiColumnList* controlList;
        CONTROL_INDEX changeColumn;

      private:
        bool OnControlsListSelection(const CEGUI::EventArgs& e);
        void CreateControlsList();
        void ReloadControlsList();
        bool SetControlAtRef(CEGUI::MCLGridRef& ref,
          const CEGUI::String& controlStr, bool overwrite);
        void AddControl(const CEGUI::String& actionStr,
          const CEGUI::String& controlStr);
        CEGUI::MCLGridRef GetOrAddAction(const CEGUI::String& actionStr);

        bool ControlUpdate(iEvent& e);
        bool ControlSet(iEvent& e);

      public:
        ControlOptionsWindow(GUIManager* guiManager);
        virtual ~ControlOptionsWindow();
        bool Create();
        bool ReloadWindow ();

      };

    } // Windows namespace
  } // GUI namespace
} // PT namespace

#endif // PT_GUI_CONTROLOPTIONSWINDOW_H
