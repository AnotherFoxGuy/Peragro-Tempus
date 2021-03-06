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

#include "client/gui/options/controls-gui.h"

#include <CEGUI.h>
#include <CEGUIWindowManager.h>
#include <CEGUILogger.h>
#include <ivaria/icegui.h>

#include <iutil/cfgmgr.h>
#include <iutil/stringarray.h>

#include "common/eventcs/eventmanager.h"
#include "common/eventcs/inputevent.h"

#include "common/reporter/reporter.h"
#include "client/pointer/pointer.h"

#include "client/gui/skinmanager.h"

namespace PT
{
  namespace GUI
  {
    namespace Windows
    {
      ControlOptionsWindow::ControlOptionsWindow(GUIManager* guiManager)
        : GUIWindow (guiManager), changeColumn(CONTROL_NONE)
      {
        windowName = CONTROLOPTIONSWINDOW;
      } // end ControlOptionsWindow()

      ControlOptionsWindow::~ControlOptionsWindow()
      {
      } // end ~ControlOptionsWindow()

      bool ControlOptionsWindow::Create()
      {
        app_cfg = csQueryRegistry<iConfigManager>
          (PointerLibrary::getInstance()->getObjectRegistry());
        if (!app_cfg)
        {
          Report(PT::Error, "Can't find the config manager!");
          return false;
        }

        ReloadWindow();

        // Register listener for controls set.
        using namespace PT::Events;
        EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();

        PT_REGISTER_LISTENER(ControlOptionsWindow, ControlUpdate, "input.options.controlupdate");
        PT_REGISTER_LISTENER(ControlOptionsWindow, ControlSet, "input.options.controlset");

        return true;
      } // end Create()

      bool ControlOptionsWindow::OnControlsListSelection(const CEGUI::EventArgs& e)
      {
        if (controlList->getSelectedCount() == 0)
          return true;

        CEGUI::ListboxItem* item = controlList->getFirstSelectedItem();
        if (!item || !item->isSelected()) return true;

        CEGUI::MCLGridRef ref(controlList->getItemGridReference(item));
        if (ref.column == ACTION_INDEX) return true;

        changeColumn = static_cast<CONTROL_INDEX>(ref.column);
        CEGUI::String control(item->getText());

        ref.column = ACTION_INDEX;
        item = controlList->getItemAtGridReference(ref);
        if (!item) return true;
        CEGUI::String action(item->getText());

        using namespace PT::Events;
        EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
        csRef<iEvent> setRequest =
          evmgr->CreateEvent("input.options.changecontrol");
        setRequest->Add("action", action.c_str());
        setRequest->Add("control", control.c_str());

        evmgr->AddEvent(setRequest);

        return true;
      } // end OnControlsListSelection()

      void ControlOptionsWindow::CreateControlsList()
      {
        // Setup columns.
        CEGUI::String strAction("Action");
        controlList->addColumn(strAction, ACTION_INDEX,
          CEGUI::UDim(0.45f,0));
        CEGUI::String strControl1("Control 1");
        controlList->addColumn(strControl1, CONTROL1_INDEX,
          CEGUI::UDim(0.25f,0));
        CEGUI::String strControl2("Control 2");
        controlList->addColumn(strControl2, CONTROL2_INDEX,
          CEGUI::UDim(0.25f,0));
        controlList->setSelectionMode(CEGUI::MultiColumnList::CellSingle);

        ReloadControlsList();

      } // end CreateControlsList()

      void ControlOptionsWindow::ReloadControlsList()
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
            AddControl(action, control);
          }
        }
      } // end LoadControlsList()

      CEGUI::ListboxItem* ControlOptionsWindow::CreateControlItem(
        const CEGUI::String& controlStr)
      {
        CEGUI::ListboxItem* newItem = new CEGUI::ListboxTextItem(controlStr);
        // This would be nice to setup in the xmls, but I don't know if it's
        // possible.
        newItem->setSelectionBrushImage((CEGUI::utf8*)"Peragro",
          (CEGUI::utf8*)"TextSelectionBrush");
        CEGUI::colour left(1.0f, 0.0f, 0.0f, 0.3f);
        CEGUI::colour right(0.7f, 0.1f, 0.1f, 0.7f);
        newItem->setSelectionColours(left, right, left, right);

        return newItem;
      } // end CreateControlItem()

      bool ControlOptionsWindow::SetControlAtRef(CEGUI::MCLGridRef& ref,
        const CEGUI::String& controlStr, bool overwrite)
      {
        if (ref.column != CONTROL1 &&
          ref.column != CONTROL2)
          return false;

        CEGUI::ListboxItem* control = controlList->getItemAtGridReference(ref);
        if (!control)
        {
          controlList->setItem(CreateControlItem(controlStr), ref);
          return true;
        }
        else if (overwrite || control->getText().empty())
        {
          control->setText(controlStr);
          return true;
        }

        return false;
      } // end SetControlAtRef()

      void ControlOptionsWindow::AddControl(const CEGUI::String& actionStr,
        const CEGUI::String& controlStr)
      {
        CEGUI::MCLGridRef ref = GetOrAddAction(actionStr);

        ref.column = CONTROL1;
        if (SetControlAtRef(ref, controlStr, false) == false)
        {
          ref.column = CONTROL2;
          if (SetControlAtRef(ref, controlStr, false) == false)
          {
            Report(PT::Notify, "More than 2 control shortcuts bound to %s, "
              "can't show in the options window.", actionStr.c_str());
          }
        }
      } // end AddControl()

      CEGUI::MCLGridRef ControlOptionsWindow::GetOrAddAction(
        const CEGUI::String& actionStr)
      {
        CEGUI::ListboxItem* action = controlList->
          findColumnItemWithText(actionStr, ACTION_INDEX, 0);
        if (!action)
        {
          // Action not found, add a new entry.
          unsigned row = controlList->addRow();
          controlList->setItem(new CEGUI::ListboxTextItem(actionStr),
            ACTION_INDEX, row);
          controlList->setItem(CreateControlItem(""), CONTROL1_INDEX, row);
          controlList->setItem(CreateControlItem(""), CONTROL2_INDEX, row);

          return CEGUI::MCLGridRef(row, ACTION_INDEX);
        }
        return controlList->getItemGridReference(action);
      } // end AddAction()

      bool ControlOptionsWindow::ControlUpdate(iEvent& ev)
      {
        using namespace PT::Events;
        const char* action = InputHelper::GetAction(&ev);
        const char* newControl = InputHelper::GetControl(&ev);

        if (action == 0 || action[0] == 0 || newControl == 0 ||
          changeColumn == CONTROL_NONE)
        {
          // Got an invalid event, reload the list just in case.
          ReloadControlsList();
          return true;
        }

        CEGUI::MCLGridRef ref = GetOrAddAction(action);

        ref.column = changeColumn;
        SetControlAtRef(ref, newControl, true);

        return true;
      } // end ControlUpdate()

      bool ControlOptionsWindow::ControlSet(iEvent& ev)
      {
        ControlUpdate(ev);

        // Finish changing control.
        changeColumn = CONTROL_NONE;
        controlList->clearAllSelections();

        app_cfg->Save();

        return true;
      } // end ControlSet()

      bool ControlOptionsWindow::ReloadWindow()
      {
        // Load the layout.
        window = GUIWindow::LoadLayout("client/options/controls.layout");

        // Set up the controls list.
        btn = winMgr->getWindow("Options/Controls/List");
        controlList = static_cast<CEGUI::MultiColumnList*>(btn);
        if (!controlList) return false;

        CreateControlsList();
        controlList->subscribeEvent(CEGUI::Window::EventMouseButtonUp,
          CEGUI::Event::Subscriber(&ControlOptionsWindow::OnControlsListSelection, this));

        return true;
      } // end ReloadWindow()

    } // Windows namespace
  } // GUI namespace
} // PT namespace

