/*
    Copyright (C) 2005 Development Team of Peragro Tempus

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "client/gui/gui.h"

#include "CEGUI.h"
#include "CEGUIWindowManager.h"
#include "CEGUILogger.h"

#include "client/network/network.h"
#include "client/gui/guimanager.h"

#include "client/data/skill/skill.h"
#include "client/data/skill/skilldatamanager.h"

#include "client/pointer/pointer.h"
#include "common/reporter/reporter.h"

#include "client/combat/combatmanager.h"

namespace PT
{
  namespace GUI
  {
    namespace Windows
    {
      StatsHUDWindow::StatsHUDWindow (GUIManager* guimanager)
        : GUIWindow (guimanager)
      {
        windowName = STATSHUDWINDOW;
      }

      StatsHUDWindow::~StatsHUDWindow ()
      {
      }

      void StatsHUDWindow::HideWindow()
      {
        winMgr->getWindow("StatsHUD/Frame")->setVisible(false);
      }

      void StatsHUDWindow::ShowWindow()
      {
        winMgr->getWindow("StatsHUD/Frame")->setVisible(true);
      }

      /**
       *
       * Function that will update the HP bar in the gui.
       *
       * @param hp The current health.
       * @param maxHP The maximum health.
       *
       */
      void StatsHUDWindow::SetHP(int hp, int maxHp)
      {
        char buf[30];
        float progress;
        if (maxHp == 0) {
          progress = 0.0f;
        } else {
          progress = hp/(float)maxHp;
        }
        hpbar->setProgress(progress);
        snprintf(buf, 29, "           %d / %d", hp, maxHp);
        buf[29] = '\0';

        SetText("StatsHUD/HPValue", buf);
      }

      void StatsHUDWindow::SetMP (float mp)
      {
        mpbar->setProgress(mp);
      }

      void StatsHUDWindow::SetXP (float xp)
      {
        xpbar->setProgress(xp);
      }

      void StatsHUDWindow::SetSP (float sp)
      {
        spbar->setProgress(sp);
      }

      void StatsHUDWindow::SetName (const char* name)
      {
        try
        {
          CEGUI::DefaultWindow* namewin   = (CEGUI::DefaultWindow*) winMgr->getWindow("StatsHUD/Name");
          namewin->setText((CEGUI::String)name);
        }
        catch(CEGUI::Exception /*&e*/)
        {
        }
      } // end SetName()

      void StatsHUDWindow::SetText (const char* name, const char* text)
      {
        CEGUI::String cname(name);
        CEGUI::String ctext(text);
        try
        {
          CEGUI::DefaultWindow* namewin   = (CEGUI::DefaultWindow*) winMgr->getWindow(cname);
          namewin->setText(ctext);
        }
        catch(CEGUI::Exception /*&e*/)
        {
        }
      } // end SetText()


      bool StatsHUDWindow::Create()
      {
        ReloadWindow();
        return true;
      }

      bool StatsHUDWindow::ReloadWindow()
      {
        window = GUIWindow::LoadLayout ("client/statshud.layout");
        GUIWindow::AddToRoot(window);

        winMgr = cegui->GetWindowManagerPtr ();

        // Set up the progressmeters
        hpbar   = (CEGUI::ProgressBar*) winMgr->getWindow("StatsHUD/HP");
        mpbar   = (CEGUI::ProgressBar*) winMgr->getWindow("StatsHUD/MP");
        xpbar   = (CEGUI::ProgressBar*) winMgr->getWindow("StatsHUD/XP");
        spbar   = (CEGUI::ProgressBar*) winMgr->getWindow("StatsHUD/SP");

        hpbar->setProgress(1.0f);
        mpbar->setProgress(1.0f);
        xpbar->setProgress(1.0f);
        spbar->setProgress(1.0f);

        return true;
      }
    }
  }
}
