/*
    Copyright (C) 2007 Development Team of Peragro Tempus

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

#include "client/reporter/reporter.h"
#include "client/pointer/pointer.h"
#include "client/event/eventmanager.h"
#include "client/event/actionevent.h"

#include <utility>

namespace PT 
{

  InputManager::InputManager()
  {
  }

  bool InputManager::Initialize()
  {
    iObjectRegistry* obj_reg = PointerLibrary::getInstance()->getObjectRegistry();
    if (!obj_reg) return false;

    csConfigAccess cfg (obj_reg, "/config/client.cfg");

    csRef<iConfigIterator> it = cfg->Enumerate("Key");

    while (it.IsValid() && it->Next())
    {
      const char* keystring = it->GetKey() + strlen(it->GetSubsection()) + 1;
      const char* action = it->GetStr();

      bool shift, alt, ctrl;
      int keycode = GetKeyCode (keystring, shift, alt, ctrl);

      // Check if valid key
      if (keycode == -1) 
      {
        Report(PT::Error, "Unknown key '%s' for action '%s'.", keystring, action);
        continue;
      }

      Report(PT::Debug, "Binding key '%s(%d)' to action '%s'.", keystring, keycode, action);
      functions.Put(keycode, action);

      if(keycode >= 'A' && keycode <= 'Z') {
        keycode += 'a' - 'A';
        Report(PT::Debug, "Binding key '%s(%d)' to action '%s'.", keystring, keycode, action);
        functions.Put(keycode, action);
      }
    }

    return true;
  }

  bool InputManager::ProcessEvent(iEvent &ev) 
  {
    if (csKeyEventHelper::GetAutoRepeat (&ev)) return false;

    csKeyEventType eventtype = csKeyEventHelper::GetEventType(&ev);
    bool down = (eventtype == csKeyEventTypeDown);

    utf32_char key = csKeyEventHelper::GetCookedCode(&ev);

    if (functions.Contains(key))
    {
      std::string action = functions.Get(key, "");
      Report(PT::Debug, "Pressed key '(%d)', firing action '%s'.", key, action.c_str());

      using namespace PT::Events;
      ActionEvent* actionEvent = new ActionEvent();
      actionEvent->action		= action;
      actionEvent->released		= !down;
      PointerLibrary::getInstance()->getEventManager()->AddEvent(actionEvent);
    }
    else
      Report(PT::Warning, "No action for key '%d'.", key);

    return false;
  }

  int InputManager::GetKeyCode (const char* keystring, bool& shift, bool& alt, bool& ctrl)
  {
    // The following code is based on crystalspace / bugplug.cpp rev 27481!
    // GNU Library General Public License
    // Copyright (C) 2001 by Jorrit Tyberghein

    // Modified version!

    shift = alt = ctrl = false;
    char const* dash = strchr (keystring, '-');
    while (dash)
    {
      if (!strncmp (keystring, "shift", int (dash-keystring))) shift = true;
      else if (!strncmp (keystring, "alt", int (dash-keystring))) alt = true;
      else if (!strncmp (keystring, "ctrl", int (dash-keystring))) ctrl = true;
      keystring = dash+1;
      dash = strchr (keystring, '-');
    }

    int keycode = -1;
    if (!strcmp (keystring, "Tab")) keycode = CSKEY_TAB;
    else if (!strcmp (keystring, "Space")) keycode = ' ';
    else if (!strcmp (keystring, "Esc")) keycode = CSKEY_ESC;
    else if (!strcmp (keystring, "Enter")) keycode = CSKEY_ENTER;
    else if (!strcmp (keystring, "Bs")) keycode = CSKEY_BACKSPACE;
    else if (!strcmp (keystring, "Up")) keycode = CSKEY_UP;
    else if (!strcmp (keystring, "Down")) keycode = CSKEY_DOWN;
    else if (!strcmp (keystring, "Right")) keycode = CSKEY_RIGHT;
    else if (!strcmp (keystring, "Left")) keycode = CSKEY_LEFT;
    else if (!strcmp (keystring, "PageUp")) keycode = CSKEY_PGUP;
    else if (!strcmp (keystring, "PageDown")) keycode = CSKEY_PGDN;
    else if (!strcmp (keystring, "Home")) keycode = CSKEY_HOME;
    else if (!strcmp (keystring, "End")) keycode = CSKEY_END;
    else if (!strcmp (keystring, "Ins")) keycode = CSKEY_INS;
    else if (!strcmp (keystring, "Del")) keycode = CSKEY_DEL;
    else if (!strcmp (keystring, "F1")) keycode = CSKEY_F1;
    else if (!strcmp (keystring, "F2")) keycode = CSKEY_F2;
    else if (!strcmp (keystring, "F3")) keycode = CSKEY_F3;
    else if (!strcmp (keystring, "F4")) keycode = CSKEY_F4;
    else if (!strcmp (keystring, "F5")) keycode = CSKEY_F5;
    else if (!strcmp (keystring, "F6")) keycode = CSKEY_F6;
    else if (!strcmp (keystring, "F7")) keycode = CSKEY_F7;
    else if (!strcmp (keystring, "F8")) keycode = CSKEY_F8;
    else if (!strcmp (keystring, "F9")) keycode = CSKEY_F9;
    else if (!strcmp (keystring, "F10")) keycode = CSKEY_F10;
    else if (!strcmp (keystring, "F11")) keycode = CSKEY_F11;
    else if (!strcmp (keystring, "F12")) keycode = CSKEY_F12;
    else if (*(keystring+1) != 0) return -1;
    else if ((*keystring >= 'A' && *keystring <= 'Z')
      || strchr ("!@#$%^&*()_+", *keystring))
    {
      shift = 1;
      keycode = *keystring;
    }
    else
      keycode = *keystring;

    return keycode;
  }

}
