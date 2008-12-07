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

#ifndef STATUS_GUI_H
#define STATUS_GUI_H

#include "base-gui.h"

#define STATUSWINDOW "Status/Frame"

namespace PT
{
  namespace GUI
  {
    namespace Windows
    {
      class StatusWindow : public GUIWindow
      {
      private:
        bool handleCloseButton(const CEGUI::EventArgs& args);
        CEGUI::Window* createDragDropSlot(CEGUI::Window* parent,
                                          const CEGUI::UVector2& position);
        CEGUI::Window* createItemIcon(CEGUI::String itemname, int itemtype,
                                      bool stackable);
        int counter;

      public:
        StatusWindow(GUIManager* guimanager);
        virtual ~StatusWindow();
        bool Create();
        bool ReloadWindow();
        bool AddItem(CEGUI::String itemname, int itemtype, bool stackable);
        void AddSkill(const char* skillname, unsigned int skillvalue);
      };

    } // Windows namespace
  } // GUI namespace
} // PT namespace

#endif // STATUS_GUI_H
