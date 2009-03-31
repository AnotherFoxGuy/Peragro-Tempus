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

#ifndef STATSHUD_GUI_H
#define STATSHUD_GUI_H

#include "base-gui.h"

#define STATSHUDWINDOW "StatsHUD/Frame"

namespace PT
{
  namespace GUI
  {
    namespace Windows
    {
      class StatsHUDWindow : public GUIWindow
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

      public:
        StatsHUDWindow(GUIManager* guimanager);
        virtual ~StatsHUDWindow();
        bool Create();
        bool ReloadWindow();
        void SetHP (float hp, float maxHp); // set the Health points.
        void SetMP (float mp, float maxMp); // set the Mana points.
        void SetXP (float xp);              // set the Experance points.
        void SetSP (float sp, float maxSp); // set the Skill points.
        void SetName (const char* name);    // set the name;
        void SetText (const char* name, const char* test); // set the text;

        void HideWindow();
        void ShowWindow();

      };

    } // Windows namespace
  } // GUI namespace
} // PT namespace

#endif // STATSHUD_GUI_H
