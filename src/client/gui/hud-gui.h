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

#ifndef HUD_GUI_H
#define HUD_GUI_H

#include "base-gui.h"

/*================//
// GUIChatWindow  //
//================*/
class HUDWindow : public GUIWindow
{
private:

  struct SelectedSkill
  {
    int SkillId;
    CEGUI::Window* SkillWindow;
  };
  SelectedSkill* selectedskill;

  CEGUI::ProgressBar* hpbar;
  CEGUI::ProgressBar* mpbar;
  CEGUI::ProgressBar* xpbar;

  int counter;
  CEGUI::Window* CreateSkillSlot(CEGUI::Window* parent, const CEGUI::Point& position, int id);
  CEGUI::Window* CreateSkillIcon(CEGUI::String skillname, int skillid);
  bool AddSkill(CEGUI::String skillname, int skillid);
  bool HandleSkillSelected(const CEGUI::EventArgs& args);
  bool HandleDragDropped(const CEGUI::EventArgs& args);
  bool HandleDragDroppedOnSkill(const CEGUI::EventArgs& args);
  bool OnRootKeyDown(const CEGUI::EventArgs& e);
  void SetActiveSkill(CEGUI::Window* window);

public:
  HUDWindow(GUIManager* guimanager);
  virtual ~HUDWindow();
  void CreateGUIWindow();    // load the chat guilayout and register button events.
  void SetHP (float hp);       // set the hp. 
  void SetMP (float mp);       // set the mp. 
  void SetName (const char* name); // set the name;
  int GetActiveSkillId();

};


#endif // HUD_GUI_H
