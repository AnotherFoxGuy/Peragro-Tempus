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

#define HUDWINDOW "PlayerHUD/Frame"

namespace PT
{
  namespace GUI
  {
    namespace Windows
    {
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
        CEGUI::ProgressBar* spbar;

        int counter;
        CEGUI::Window* CreateSkillSlot(CEGUI::Window* parent,
                                       const CEGUI::UVector2& position, int id);
        CEGUI::Window* CreateSkillIcon(int skillid);
        bool HandleSkillSelected(const CEGUI::EventArgs& args);
        bool HandleDragDropped(const CEGUI::EventArgs& args);
        bool HandleDragDroppedOnSkill(const CEGUI::EventArgs& args);
        bool OnRootKeyDown(const CEGUI::EventArgs& e);
        void SetActiveSkill(CEGUI::Window* window);

      public:
        HUDWindow(GUIManager* guimanager);
        virtual ~HUDWindow();
        bool Create();
        bool ReloadWindow();
        void SetHP (int hp, int maxHp); // set the Health points.
        void SetMP (float mp);          // set the Mana points.
        void SetXP (float xp);          // set the Experance points.
        void SetSP (float sp);          // set the Skill points.
        void SetName (const char* name); // set the name;
        void SetText (const char* name, const char* test); // set the text;
        bool AddSkill(int skillid);
        int GetActiveSkillId();

        void HideWindow();
        void ShowWindow();

      };

    } // Windows namespace
  } // GUI namespace
} // PT namespace

#endif // HUD_GUI_H
