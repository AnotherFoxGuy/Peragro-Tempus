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

#include "client/gui/gui.h"
#include "client/gui/guimanager.h"
#include "client/gui/skinmanager.h"

#include <CEGUI.h>
#include <CEGUIWindowManager.h>
#include <CEGUILogger.h>
#include <ivaria/icegui.h>
#include <iutil/stringarray.h>

#include "common/reporter/reporter.h"
#include "common/event/eventmanager.h"
#include "common/event/interfaceevent.h"

namespace PT
{
  namespace GUI
  {
    namespace Windows
    {
      ControlOptionsWindow::ControlOptionsWindow(GUIManager* guiManager)
        : GUIWindow (guiManager), ACTION_INDEX(0), CONTROL1_INDEX(1),
          CONTROL2_INDEX(2)
      {
        windowName = CONTROLOPTIONSWINDOW;
      } // end ControlOptionsWindow()

      ControlOptionsWindow::~ControlOptionsWindow()
      {
      } // end ~ControlOptionsWindow()

      bool ControlOptionsWindow::Create()
      {
        ReloadWindow();

        return true;
      } // end Create()

      bool ControlOptionsWindow::ControlSet(iEvent& e)
      {
        LoadControlsList();

        return true;
      } // end ControlSet()

      bool ControlOptionsWindow::OnControlsListSelection(const CEGUI::EventArgs& e)
      {
        if (controlList->getSelectedCount() == 0)
          return true;

        CEGUI::ListboxItem* item = controlList->getFirstSelectedItem();
        if (!item->isSelected()) return true;

        CEGUI::MCLGridRef ref(controlList->getItemGridReference(item));
        if (ref.column == ACTION_INDEX) return true;

        CEGUI::String control(item->getText());

        ref.column = ACTION_INDEX;
        item = controlList->getItemAtGridReference(ref);
        if (!item) return true;
        CEGUI::String action(item->getText());

        using namespace PT::Events;
        EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
        csRef<iEvent> setRequest =
          evmgr->CreateEvent("interface.options.controls.change");
        setRequest->Add("action", action.c_str());
        setRequest->Add("control", control.c_str());

        evmgr->AddEvent(setRequest);

        Report(PT::Debug, "Change %s %s", action.c_str(), control.c_str());
        return true;
      } // end OnControlsListSelection()

      void ControlOptionsWindow::CreateControlsList()
      {
        // Setup columns.
        CEGUI::String strAction("Action");
        controlList->addColumn(strAction, ACTION_INDEX, CEGUI::UDim(0.45f,0));
        CEGUI::String strControl1("Control 1");
        controlList->addColumn(strControl1, CONTROL1_INDEX, CEGUI::UDim(0.25f,0));
        CEGUI::String strControl2("Control 2");
        controlList->addColumn(strControl2, CONTROL2_INDEX, CEGUI::UDim(0.25f,0));
        controlList->setSelectionMode(CEGUI::MultiColumnList::CellSingle);

        LoadControlsList();

      } // end CreateControlsList()

      void ControlOptionsWindow::LoadControlsList()
      {
        // Remove all items from the list.
        controlList->resetList();

        // Iterate over all controls in the configuration.
        csRef<iConfigIterator> keyItr(app_cfg->Enumerate("Peragro.Control."));
        while(keyItr->HasNext())
        {
          keyItr->Next();

          const char* action = keyItr->GetStr();
          const char* control = keyItr->GetKey(true);
          if (action != 0 && control != 0)
          {
            AddListItem(action, control);
          }
        }
      } // end LoadControlsList()

      void ControlOptionsWindow::AddListItem(const CEGUI::String& actionStr,
        const CEGUI::String& controlStr)
      {
        CEGUI::ListboxItem* action =
          controlList->findColumnItemWithText(actionStr, ACTION_INDEX, 0);
        if (action)
        {
          // Action found, add control.
          CEGUI::MCLGridRef ref(controlList->getItemRowIndex(action),
            CONTROL1_INDEX);
          CEGUI::ListboxItem* control = controlList->getItemAtGridReference(ref);

          if (!control)
          {
            controlList->setItem(new CEGUI::ListboxTextItem(controlStr), ref);
          }
          else if (control->getText().empty())
          {
            control->setText(controlStr);
          }
          else
          {
            // Control 1 set, check control 2.
            ref.column = CONTROL2_INDEX;
            control = controlList->getItemAtGridReference(ref);

            if (!control || control->getText().empty())
            {
              controlList->setItem(new CEGUI::ListboxTextItem(controlStr), ref);
            }
            else if (control->getText().empty())
            {
              control->setText(controlStr);
            }
            else
            {
              Report(PT::Notify, "More than 2 control shortcuts bound to %s, "
                "can't show in the options window.", actionStr.c_str());
            }
          }
        }
        else
        {
          // Action not found, add a new entry.
          unsigned row = controlList->addRow();
          controlList->setItem(new CEGUI::ListboxTextItem(actionStr),
            ACTION_INDEX, row);
          controlList->setItem(new CEGUI::ListboxTextItem(controlStr),
            CONTROL1_INDEX, row);
        }
      }

      bool ControlOptionsWindow::ReloadWindow()
      {
        // Load the layout.
        window = GUIWindow::LoadLayout("client/options/controls.xml");

        app_cfg = csQueryRegistry<iConfigManager>
          (PointerLibrary::getInstance()->getClient()->GetObjectRegistry());
        if (!app_cfg)
        {
          Report(PT::Error, "Can't find the config manager!");
          return false;
        }

        // Set up the controls list.
        btn = winMgr->getWindow("Options/Controls/List");
        controlList = static_cast<CEGUI::MultiColumnList*>(btn);
        if (!controlList) return false;

        CreateControlsList();
        controlList->subscribeEvent(CEGUI::MultiColumnList::EventSelectionChanged,
          CEGUI::Event::Subscriber(&ControlOptionsWindow::OnControlsListSelection, this));


        // Register listener for controls set.
        using namespace PT::Events;
        EventHandler<ControlOptionsWindow>* cbControlSet =
          new EventHandler<ControlOptionsWindow>
          (&ControlOptionsWindow::ControlSet, this);
        PointerLibrary::getInstance()->getEventManager()->
          AddListener("interface.options.controls.changed", cbControlSet);

        return true;
      } // end ReloadWindow()

    } // Windows namespace
  } // GUI namespace
} // PT namespace

