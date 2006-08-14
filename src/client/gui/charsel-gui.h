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

/*======================//
// GUISelectCharWindow  //
//======================*/
class SelectCharWindow : public GUIWindow
{
private:
  bool SelectChar(const CEGUI::EventArgs& e);  // Connects with the current character.
  bool OnSelection(const CEGUI::EventArgs& e); // Shows the currently selected character.
  bool NewChar(const CEGUI::EventArgs& e);     // Makes a new character.
  bool DelChar(const CEGUI::EventArgs& e);     // Deletes the selected character.
  CEGUI::String GetNewCharName();              // Returns the character name.
  void ScrollLeft();                           // Scroll left throught the characterlist.
  void ScrollRight();                          // Scroll right throught the characterlist.

  bool CreateButton(const CEGUI::EventArgs& e);   // Opens the Create character menu.
  bool CancelButton(const CEGUI::EventArgs& e);   // returns to the character menu.
  void ToggleNewWindow(bool visible); 

public:
  SelectCharWindow(GUIManager* guimanager);
  virtual ~SelectCharWindow();
  void CreateGUIWindow();    // load the Login guilayout and register button events.
  void AddCharacter(unsigned int charId, const char* name);
};

#endif // CHARSEL_GUI_H
