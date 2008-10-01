/*
    Copyright (C) 2007-2008 Development Team of Peragro Tempus

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

#include "inputmanager.h"

#include "common/reporter/reporter.h"
#include "client/pointer/pointer.h"
#include "common/event/eventmanager.h"
#include "common/event/inputevent.h"
#include "include/clipboard.h"

#include "CEGUI.h"
#include "CEGUIWindowManager.h"
#include "CEGUILogger.h"
#include "client/gui/guimanager.h"

#include <utility>

namespace PT
{
  namespace Input
  {
    InputManager::InputManager()
      : csBaseEventHandler()
    {
    } // end InputManager()

    bool InputManager::Initialize()
    {
      csRef<iObjectRegistry> obj_reg =
        PointerLibrary::getInstance()->getObjectRegistry();
      if (!obj_reg) return false;

      // Register for input events.
      csBaseEventHandler::Initialize(obj_reg);
      RegisterQueue(obj_reg, csevInput(obj_reg));

      SETUP_HANDLER
      REGISTER_LISTENER(InputManager, ChangeControl, "input.options.changecontrol")

      // Register listeners for Clipboard Events.
      csTheClipboard = csQueryRegistry<iClipboard> (obj_reg);
      if (csTheClipboard)
      {
        REGISTER_LISTENER(InputManager, ClipboardCopy, "input.CopyText")
        REGISTER_LISTENER(InputManager, ClipboardPaste, "input.PasteText")
        REGISTER_LISTENER(InputManager, ClipboardCut, "input.CutText")

        csString ostype("");
        csTheClipboard->GetOS(ostype);
        Report(PT::Debug, "Clipboard plugin is using the '%s' implementation.",
          ostype.GetData());
      }
      else
      {
        Report(PT::Warning, "Clipboard object unavailable. Disabling clipboard.");
      }

      if (!LoadControls()) return false;

      return true;
    } // end Initialize()

    bool InputManager::LoadControls()
    {
      csRef<iObjectRegistry> obj_reg =
        PointerLibrary::getInstance()->getObjectRegistry();
      if (!obj_reg) return false;
      // Open configuration file.
      csRef<iConfigManager> app_cfg = csQueryRegistry<iConfigManager>(obj_reg);
      if (!app_cfg) return Report(PT::Error, "Can't find the config manager!");

      csRef<iConfigIterator> cfgItr = app_cfg->Enumerate("Peragro.Control.");

      // Load the controls.
      while (cfgItr.IsValid() && cfgItr->Next())
      {
        ControlCombo combo;

        if (!combo.SetFromConfigString(cfgItr->GetKey(true)))
        {
          Report(PT::Error, "Invalid control combo '%s' with action '%s'.",
            cfgItr->GetKey(true), cfgItr->GetStr());
          continue;
        }

        // Assign a function to control.
        if (controls.insert(std::make_pair(combo, cfgItr->GetStr()))
          .second == false)
        {
          Report(PT::Error, "Duplicate control combo '%s' ignored (action '%s').",
            cfgItr->GetKey(true), cfgItr->GetStr());
        }
      } // end while

      return true;
    } // end LoadControls()

    bool InputManager::HandleControlEvents(const iEvent &ev, const bool keyboard)
    {
      // Create a control combo from the event for lookup.
      const ControlCombo eventCombo(ev, keyboard);

      // Check if the key is down or up.
      bool down = false;
      if (keyboard)
        down = (csKeyEventHelper::GetEventType(&ev) == csKeyEventTypeDown);
      else
        down = (csMouseEventHelper::GetEventType(&ev) == csMouseEventTypeDown);

      if (!changeControl.get())
      {
        // Search the control map.
        ControlMap::const_iterator itr = controls.find(eventCombo);
        if (itr == controls.end())
        {
          return Report(PT::Debug, "No action for key '%s' (%s).",
            eventCombo.ConfigString().c_str(), down?"down":"up");
        }

        Report(PT::Debug, "Sending event '%s' combo '%s'.",
          itr->second.c_str(), eventCombo.ConfigString().c_str()); // TODO remove

        // Setup the event, and fire it.
        PT::Events::EventManager* evmgr =
          PointerLibrary::getInstance()->getEventManager();
        std::string eventName = "input."; eventName += itr->second;
        csRef<iEvent> inputEvent = evmgr->CreateEvent(eventName);
        inputEvent->Add("action", itr->second.c_str());
        inputEvent->Add("buttonState", down);
        evmgr->AddEvent(inputEvent);
      }
      else
      {
        // Change the control mapping.
        using namespace PT::Events;
        EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();

        if (down)
        {
          // Control press, update.
          csRef<iEvent> updateEvent =
            evmgr->CreateEvent("input.options.controlupdate");
          updateEvent->Add("action", changeControl->action.c_str());
          updateEvent->Add("control", eventCombo.ConfigString().c_str());
          evmgr->AddEvent(updateEvent);
        }
        else
        {
          // Control release, set.
          csRef<iEvent> setEvent =
            evmgr->CreateEvent("input.options.controlset");

          // Try insert pressed control and action being set.
          std::pair<ControlMap::iterator, bool> boundControl(
            controls.insert(std::make_pair(eventCombo, changeControl->action)));

          // True if the control was not already in the map.
          if (boundControl.second == true)
          {
            // Successful, remove any previous binding.
            if (changeControl->itr != controls.end())
            {
              controls.erase(changeControl->itr);
              Report(PT::Debug, "Removing control combo '%s %s'.",
                changeControl->action.c_str(),
                changeControl->itr->first.ConfigString().c_str());
            }

            Report(PT::Debug, "Control combo %s bound to %s.",
              boundControl.first->first.ConfigString().c_str(),
              boundControl.first->second.c_str());

            setEvent->Add("action", boundControl.first->second.c_str());
            setEvent->Add("control",
              boundControl.first->first.ConfigString().c_str());
          }
          else
          {
            // Unsuccessful, set to previous binding, report error only if it's
            // different.
            if (eventCombo != changeControl->itr->first)
            {
              Report(PT::Error, "Control combo %s is already bound to %s.",
                boundControl.first->first.ConfigString().c_str(),
                boundControl.first->second.c_str());
            }

            setEvent->Add("action", changeControl->action.c_str());
            setEvent->Add("control", changeControl->itr->first.
              ConfigString().c_str());
          }

          evmgr->AddEvent(setEvent);
          // Reset the control change data.
          changeControl.reset();
        }
      }

      return true;
    } // end HandleControlEvents()

    bool InputManager::OnKeyboard(iEvent &ev)
    {
      // If the key is being held, we don't send any new actions.
      if (csKeyEventHelper::GetAutoRepeat(&ev)) return false;

      return HandleControlEvents(ev, true);
    } // end OnKeyboard()

    bool InputManager::OnMouseDown(iEvent& ev)
    {
      return HandleControlEvents(ev, false);
    } // end OnMouseDown()

    bool InputManager::OnMouseUp(iEvent& ev)
    {
      return HandleControlEvents(ev, false);
    } // end OnMouseUp()

    bool InputManager::ChangeControl(iEvent& ev)
    {
      using namespace PT::Events;
      const char* control = InputHelper::GetControl(&ev);
      const char* action = InputHelper::GetAction(&ev);
      if (action == 0 || control == 0 || action[0] == 0)
      {
        return Report(PT::Debug, "Got invalid control change request.");
      }

      const ControlCombo controlCombo(control);

      if (!controlCombo.IsValid())
      {
        // Setup new control for this action.
        changeControl.reset(new ControlChangeData(controls.end(), action));
        Report(PT::Debug, "Listening for control setup. '%s, %s'",
          action, control);
      }
      else
      {
        // Change existing control.
        ControlMap::iterator itr(controls.find(controlCombo));
        if (itr == controls.end())
        {
          Report(PT::Debug, "Control in request not found '%s:%s' "
            "(request converts to %s)", action, control,
            controlCombo.ConfigString().c_str());
        }
        else if (itr->second == action)
        {
          changeControl.reset(new ControlChangeData(itr, action));
          Report(PT::Debug, "Listening for control change. '%s:%s'",
            action, control);
        }
        else
        {
          Report(PT::Debug, "Incorrect action for control change '%s:%s' "
            "(action found: %s)", action, control, itr->second.c_str());
        }
      }

      return true;
    }

    bool InputManager::ClipboardCut(iEvent& ev)
    {
      using namespace PT::Events;

      if (!InputHelper::GetButtonDown(&ev)) return false;

      DoCopy(true);
      return true;
    } // end ClipboardCut()

    bool InputManager::ClipboardCopy(iEvent& ev)
    {
      using namespace PT::Events;

      if (!InputHelper::GetButtonDown(&ev)) return false;

      DoCopy(false);
      return true;
    } // end ClipboardCopy()

    bool InputManager::DoCopy(bool cutText)
    {
      Report(PT::Debug, "copy");
      iCEGUI* cegui = PointerLibrary::getInstance()->getGUIManager()->GetCEGUI();
      CEGUI::Window* activeChildWindow =
        cegui->GetWindowManagerPtr()->getWindow("Root")->getActiveChild();
      if (activeChildWindow)
      {
        CEGUI::String wintype = activeChildWindow->getType();
        if (wintype == "Peragro/Editbox")
        {
          // Get the selected text.
          CEGUI::Editbox* ceguiEditBox =
            static_cast<CEGUI::Editbox*>(activeChildWindow);
          CEGUI::String::size_type index = ceguiEditBox->getSelectionStartIndex();
          CEGUI::String::size_type length = ceguiEditBox->getSelectionLength();
          csString selectedText =
            ceguiEditBox->getText().substr(index, length).c_str();

          // Cut text support.
          if (cutText && !ceguiEditBox->isReadOnly())
          {
            CEGUI::String text = ceguiEditBox->getText();
            ceguiEditBox->setText(text.erase(index, length));
            ceguiEditBox->setCaratIndex(index);
          }

          // Copy text to clipboard.
          csTheClipboard->SetData(selectedText, 0);

        }
        else
        {
          // Report what type it is for future use!
          Report(PT::Debug, "Attempting to copy text from %s!", wintype.c_str());
        }
      }
      return true;
    } // end DoCopy()

    bool InputManager::ClipboardPaste(iEvent& ev)
    {
      using namespace PT::Events;

      if (!InputHelper::GetButtonDown(&ev)) return false;

      Report(PT::Debug, "paste");
      csString text;
      csTheClipboard->GetData(text, 0);
      if (text.Length() > 0)
      {
        CEGUI::String newText = text.GetData();
        for (size_t length = newText.length(), count = 0; count < length; ++count)
          CEGUI::System::getSingleton().injectChar(static_cast<CEGUI::utf32>(newText[count]));
      }
      return true;
    } // end ClipboardPaste()

  } // Input namespace
} // PT namespace
