/*
  Copyright (C) 2005 - 2007 Development Team of Peragro Tempus

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

#include "client/gui/options/movement-gui.h"

#include <CEGUI.h>
#include <CEGUIWindowManager.h>
#include <CEGUILogger.h>
#include <ivaria/icegui.h>

#include <iutil/cfgmgr.h>

#include "common/reporter/reporter.h"
#include "common/eventcs/eventmanager.h"

#include "client/pointer/pointer.h"

#include "client/gui/skinmanager.h"

namespace PT
{
  namespace GUI
  {
    namespace Windows
    {
      MovementOptionsWindow::MovementOptionsWindow(GUIManager* guiManager)
        : GUIWindow (guiManager)
      {
        windowName = MOVEMENTOPTIONSWINDOW;
      } // end MovementOptionsWindow()

      MovementOptionsWindow::~MovementOptionsWindow()
      {
      } // end ~MovementOptionsWindow()

      void MovementOptionsWindow::SaveConfig()
      {
        app_cfg->Save();
      } // end SaveConfig()

      bool MovementOptionsWindow::Create()
      {
        ReloadWindow();

        return true;
      } // end Create()

      bool MovementOptionsWindow::ReloadWindow()
      {
        // Load the layout.
        window = GUIWindow::LoadLayout ("client/options/movement.layout");

        app_cfg = csQueryRegistry<iConfigManager>
          (PointerLibrary::getInstance()->getObjectRegistry());
        if (!app_cfg)
        {
          Report(PT::Error, "Can't find the config manager!");
          return false;
        }

        // Set up the button behaviour for the movement mode.
        CreateDropListMovement();
        btn = winMgr->getWindow("Options/Movement/Mode/DropList");
        btn->subscribeEvent(CEGUI::Combobox::EventListSelectionAccepted,
          CEGUI::Event::Subscriber(&MovementOptionsWindow::OnDropListMovement, this));

        // Set up the "backward reverse" checkbox.
        CreateReverseCheckBox();
        btn = winMgr->getWindow("Options/Movement/Reverse");
        btn->subscribeEvent(CEGUI::Checkbox::EventCheckStateChanged,
          CEGUI::Event::Subscriber(&MovementOptionsWindow::OnReverseCheckBox, this));

        // Set up the "invert Y axis" checkbox.
        CreateYAxisCheckBox();
        btn = winMgr->getWindow("Options/Movement/Invert_Y_Axis");
        btn->subscribeEvent(CEGUI::Checkbox::EventCheckStateChanged,
          CEGUI::Event::Subscriber(&MovementOptionsWindow::OnYAxisCheckBox, this));

        return true;
      } // end ReloadWindow()

      void MovementOptionsWindow::SendUpdateEvent()
      {
        PT::Events::EventManager* evmgr =
          PointerLibrary::getInstance()->getEventManager();
        evmgr->AddEvent(evmgr->CreateEvent("interface.options.movement"));
      } // end SendUpdateEvent()

      bool MovementOptionsWindow::OnDropListMovement(const CEGUI::EventArgs& e)
      {
        btn = winMgr->getWindow("Options/Movement/Mode/DropList");
        uint id = ((CEGUI::Combobox*)btn)->getSelectedItem()->getID();

        bool local = true;

        switch(id)
        {
        case 0: // Local
          local = true;
          break;
        case 1: // Server
          local = false;
          break;
        default: Report(PT::Error, "OnDropListMovement: failed %d", id);
        }

        app_cfg->SetBool("Peragro.Movement.Local", local);
        SendUpdateEvent();
        SaveConfig();
        return true;
      }

      void MovementOptionsWindow::CreateDropListMovement()
      {

        btn = winMgr->getWindow("Options/Movement/Mode/DropList");

        bool local = app_cfg->GetBool("Peragro.Movement.Local");

        if (local)
          ((CEGUI::Combobox*)btn)->setText("Client");
        else
          ((CEGUI::Combobox*)btn)->setText("Server");

        CEGUI::ListboxItem* charIdItem = new CEGUI::ListboxTextItem((CEGUI::utf8*)"Client", 0);
        ((CEGUI::Combobox*)btn)->addItem(charIdItem);

        charIdItem = new CEGUI::ListboxTextItem((CEGUI::utf8*)"Server", 1);
        ((CEGUI::Combobox*)btn)->addItem(charIdItem);

        ((CEGUI::Combobox*)btn)->setReadOnly(true);

      }

      bool MovementOptionsWindow::OnReverseCheckBox(const CEGUI::EventArgs& e)
      {
        btn = winMgr->getWindow("Options/Movement/Reverse");
        bool fs = ((CEGUI::Checkbox*)btn)->isSelected();

        app_cfg->SetBool("Peragro.Movement.BackwardReverse", fs);
        SendUpdateEvent();
        SaveConfig();
        return true;
      }

      void MovementOptionsWindow::CreateReverseCheckBox()
      {
        btn = winMgr->getWindow("Options/Movement/Reverse");

        bool fs = app_cfg->GetBool("Peragro.Movement.BackwardReverse");

        ((CEGUI::Checkbox*)btn)->setSelected(fs);
      }

      bool MovementOptionsWindow::OnYAxisCheckBox(const CEGUI::EventArgs& e)
      {
        btn = winMgr->getWindow("Options/Movement/Invert_Y_Axis");
        bool ya = ((CEGUI::Checkbox*)btn)->isSelected();

        app_cfg->SetBool("Peragro.Movement.InvertYAxis", ya);
        // Goes to a different plugin, using the other event name.
        PT::Events::EventManager* evmgr =
          PointerLibrary::getInstance()->getEventManager();
        evmgr->AddEvent(evmgr->CreateEvent("interface.options.view"));
        SaveConfig();
        return true;
      }

      void MovementOptionsWindow::CreateYAxisCheckBox()
      {
        btn = winMgr->getWindow("Options/Movement/Invert_Y_Axis");

        bool ya = app_cfg->GetBool("Peragro.Movement.InvertYAxis");

        ((CEGUI::Checkbox*)btn)->setSelected(ya);
      }

    } // Windows namespace
  } // GUI namespace
} // PT namespace

