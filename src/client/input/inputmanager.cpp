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
    {
    } // end InputManager()

    bool InputManager::Initialize()
    {
      iObjectRegistry* obj_reg =
        PointerLibrary::getInstance()->getObjectRegistry();
      if (!obj_reg) return false;

      // Register for input events.
      csBaseEventHandler::Initialize (obj_reg);
      RegisterQueue (obj_reg, csevInput(obj_reg));

      // Open configuration file, and prepare iterator for easy access.
      csRef<iConfigManager> app_cfg = csQueryRegistry<iConfigManager> (obj_reg);
      if (!app_cfg) return Report(PT::Error, "Can't find the config manager!");
      csRef<iConfigIterator> it = app_cfg->Enumerate("Peragro.Control.");

      Report(PT::Debug, "");
      Report(PT::Debug, "==Loading keybindings==========================");
      uint numberOfKeys = 0; // Number of keys we've read.

      // Load the shortcuts.
      while (it.IsValid() && it->Next())
      {
        ShortcutCombo combo;

        if (!combo.SetFromConfigString(it->GetKey(true)))
        {
          Report(PT::Error, "Unknown key combo '%s' for action '%s'.",
            it->GetKey(true), it->GetStr());
          continue;
        }

        // Assign a function to combo.
        functions[combo] = it->GetStr();
        Report(PT::Debug, "%-10s %s", it->GetKey(true), it->GetStr());
        numberOfKeys++;
      } // end while
      Report(PT::Debug, "================================ %d keybinding(s)\n",
        numberOfKeys);

      // Register listeners for Clipboard Events.
      csTheClipboard = csQueryRegistry<iClipboard> (obj_reg);
      if (csTheClipboard)
      {
        using namespace PT::Events;

        EventHandler<InputManager>* cbClipboardCopy =
          new EventHandler<InputManager>(&InputManager::ClipboardCopy, this);
        PointerLibrary::getInstance()->getEventManager()->
          AddListener("input.CopyText", cbClipboardCopy);

        EventHandler<InputManager>* cbClipboardPaste =
          new EventHandler<InputManager>(&InputManager::ClipboardPaste, this);
        PointerLibrary::getInstance()->getEventManager()->
          AddListener("input.PasteText", cbClipboardPaste);

        EventHandler<InputManager>* cbClipboardCut =
          new EventHandler<InputManager>(&InputManager::ClipboardCut, this);
        PointerLibrary::getInstance()->getEventManager()->
          AddListener("input.CutText", cbClipboardCut);

        csString ostype = "";
        csTheClipboard->GetOS(ostype);
        Report(PT::Debug, "Clipboard plugin is using the '%s' implementation.",
          ostype.GetData());
      }
      else
      {
        Report(PT::Warning, "Clipboard object unavailable. Disabling clipboard.");
      }

      return true;
    } // end Initialize()

    bool InputManager::OnKeyboard(iEvent &ev)
    {
      // If the key is being held, we don't send any new actions.
      if (csKeyEventHelper::GetAutoRepeat(&ev)) return false;

      // Check if the key was pressed or not.
      bool down = (csKeyEventHelper::GetEventType(&ev) == csKeyEventTypeDown);

      // Create an event shortcut for lookup.
      ShortcutCombo eventShortcut;
      eventShortcut.SetFromKeyEvent(ev);

      std::map<ShortcutCombo,std::string>::const_iterator it =
        functions.find(eventShortcut);

      // Did we find an action for key binding?
      if (it == functions.end())
      {
        Report(PT::Debug, "No action for key '%s'(%s).",
          eventShortcut.GetConfigKey().c_str(), down?"down":"up");
        return false;
      }

      //Report(PT::Debug, "Pressed key combo '(%s)', firing action '%s'",
      //  it->first.GetConfigKey().c_str(), it->second.c_str());

      // Setup the event, and fire it.
      PT::Events::EventManager* evmgr =
        PointerLibrary::getInstance()->getEventManager();
      std::string eventName = "input."; eventName += it->second;
      csRef<iEvent> inputEvent = evmgr->CreateEvent(eventName);
      inputEvent->Add("action", it->second.c_str());
      inputEvent->Add("buttonState", down);
      evmgr->AddEvent(inputEvent);

      return true;
    } // end OnKeyboard()

    bool InputManager::OnMouse(iEvent& ev)
    {
      bool down = (csMouseEventHelper::GetEventType(&ev) == csMouseEventTypeDown);

      ShortcutCombo eventShortcut;
      eventShortcut.SetFromMouseEvent(ev);

      std::map<ShortcutCombo,std::string>::const_iterator it =
        functions.find(eventShortcut);

      if (it == functions.end())
      {
        Report(PT::Debug, "No action for button '%s'(%s).",
          eventShortcut.GetConfigKey().c_str(), down?"down":"up");
        return false;
      }

      //Report(PT::Debug, "%s button '(%s)', firing action '%s'.",
      //  down ? "Pressed":"Released", it->first.GetConfigKey().c_str(), it->second.c_str());

      PT::Events::EventManager* evmgr =
        PointerLibrary::getInstance()->getEventManager();
      std::string eventName = "input."; eventName += it->second;
      csRef<iEvent> inputEvent = evmgr->CreateEvent(eventName);
      inputEvent->Add("action", it->second.c_str());
      inputEvent->Add("buttonState", down);
      evmgr->AddEvent(inputEvent);

      return true;
    } // end OnMouse()

    bool InputManager::OnMouseDown(iEvent& ev)
    {
      return OnMouse(ev);
    } // end OnMouseDown()

    bool InputManager::OnMouseUp(iEvent& ev)
    {
      return OnMouse(ev);
    } // end OnMouseUp()

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

    bool InputManager::DoCopy(bool cuttext)
    {
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
          if (cuttext && !ceguiEditBox->isReadOnly())
          {
            CEGUI::String text = ceguiEditBox->getText();
            ceguiEditBox->setText( text.erase( index, length ) );
            ceguiEditBox->setCaratIndex(index);
          }

          // Copy text to clipboard.
          csTheClipboard->SetData(selectedText,0);

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
