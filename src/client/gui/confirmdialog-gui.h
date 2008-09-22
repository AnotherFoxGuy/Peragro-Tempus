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

#ifndef CONFIRMDIALOG_GUI_H
#define CONFIRMDIALOG_GUI_H

#include "base-gui.h"

#define OKDIALOGWINDOW "OkDialog/Frame"
#define CONFIRMDIALOGWINDOW "ConfirmDialog/Frame"

namespace PT
{
  namespace GUI
  {
    namespace Windows
    {
      class OkDialogWindow : public GUIWindow
      {
      private:
        bool OnOkButton(const CEGUI::EventArgs& args);

      public:
        OkDialogWindow(GUIManager* guimanager);
        virtual ~OkDialogWindow();
        // Load the chat gui layout and register button events.
        bool Create();
        bool ReloadWindow();

        void SetText(const char* text);
        void SetOkEvent(CEGUI::Event::Subscriber subscriber,
                        bool destroywindow);
      };

      class ConfirmDialogWindow : public GUIWindow
      {
      private:
        bool OnYesButton(const CEGUI::EventArgs& args);
        bool OnNoButton(const CEGUI::EventArgs& args);

      public:
        ConfirmDialogWindow(GUIManager* guimanager);
        virtual ~ConfirmDialogWindow();
        bool Create();

        void SetText(const char* text);
        void SetYesEvent(CEGUI::Event::Subscriber subscriber,
                         bool destroywindow=true);
        void SetNoEvent(CEGUI::Event::Subscriber subscriber,
                        bool destroywindow=true);
      };

    } // Windows namespace
  } // GUI namespace
} // PT namespace

#endif // CONFIRMDIALOG_GUI_H
