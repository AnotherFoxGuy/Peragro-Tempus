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

#ifndef INTERACTDIALOG_GUI_H
#define INTERACTDIALOG_GUI_H

#include "base-gui.h"

#define INTERACTDIALOGWINDOW "InteractDialog/Frame"

namespace PT
{
  namespace GUI
  {
    namespace Windows
    {
      class InteractDialogWindow : public GUIWindow
      {
      private:
        bool newDialog;
        unsigned int interactId;
        unsigned int itemId;

        float root_size;
        float button_size;

        void ClearActions();
        void AddAction(const char* action);
        void LayoutIcons();

        bool OnCancelButton(const CEGUI::EventArgs& args);
        bool OnAction(const CEGUI::EventArgs& args);

      public:
        InteractDialogWindow(GUIManager* guimanager);
        virtual ~InteractDialogWindow();
        bool Create();
        bool ReloadWindow();

        bool OnInteract (iEvent& ev);
      };

    } // Windows namespace
  } // GUI namespace
} // PT namespace

#endif // INTERACTDIALOG_GUI_H
