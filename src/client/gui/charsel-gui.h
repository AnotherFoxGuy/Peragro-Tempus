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

#ifndef CHARSEL_GUI_H
#define CHARSEL_GUI_H

#include "base-gui.h"

#define SELECTCHARWINDOW "CharSelect/Frame"

namespace PT
{
  namespace GUI
  {
    namespace Windows
    {
      class SelectCharWindow : public GUIWindow
      {
      private:
        /// Connects with the current character.
        bool SelectChar(const CEGUI::EventArgs& e);
        /// Shows the currently selected character.
        bool OnSelection(const CEGUI::EventArgs& e);
        /// Makes a new character.
        bool NewChar(const CEGUI::EventArgs& e);
        /// Deletes the selected character.
        bool DelChar(const CEGUI::EventArgs& e);
        /// Returns the character name.
        CEGUI::String GetNewCharName();
        /// Clears the character name textbox.
        void ClearNewCharName();
        /// Scroll left throught the characterlist.
        void ScrollLeft();
        /// Scroll right throught the characterlist.
        void ScrollRight();

        /// Opens the Create character menu.
        bool CreateButton(const CEGUI::EventArgs& e);
        /// Returns to the character menu.
        bool CancelButton(const CEGUI::EventArgs& e);
        /// Opens the server setup window.
        bool AdminButton(const CEGUI::EventArgs& e);
        void ToggleNewWindow(bool visible);

      public:
        void ShowAdminButton();
        SelectCharWindow(GUIManager* guimanager);
        virtual ~SelectCharWindow();
        bool Create();
        bool ReloadWindow();
        void AddCharacter(unsigned int charId, const char* name,
                          unsigned char* skincolour, unsigned char* hairncolour,
                          unsigned char* decalcolour);
        void EmptyCharList();
        void ShowWindow();
      };

    } // Windows namespace
  } // GUI namespace
} // PT namespace

#endif // CHARSEL_GUI_H
