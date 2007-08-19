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

namespace PT {

  const char* InputManager::actionStr[] = { "ACTION_FORWARD", 
                                         "ACTION_BACKWARD",
                                         "ACTION_LEFT",
                                         "ACTION_RIGHT",
                                         "ACTION_TOGGLEWALK",
                                         "ACTION_PANUP",
                                         "ACTION_PANDOWN",
                                         "ACTION_TOGGLECAMERA",
                                         "ACTION_TOGGLEDISTCLIP",
                                         "ACTION_HIT",
                                         "ACTION_ACTIVATESKILL",
                                         "ACTION_ACTIVATEWEAPON",
                                         "ACTION_QUIT",
                                         };

  InputManager::InputManager() {
    actionCount = sizeof(actionStr)/sizeof(const char*);

    functionList = new tFunction[actionCount];
    for (int i=0; i < actionCount; i++) {
      functionList[i] = 0;
    }
    for (int i=0; i < actionCount; i++) {
      actionList[i] = -1; // No valid action bind to this key yet.
    }
  }

  bool InputManager::ProcessEvent(iEvent &ev) const {
    if (csKeyEventHelper::GetAutoRepeat (&ev)) return false;

    csKeyEventType eventtype = csKeyEventHelper::GetEventType(&ev);
    bool down = (eventtype == csKeyEventTypeDown);
    
    int action = GetActionType(ev);
    if (action < 0 || action >= sizeof(actionStr)/sizeof(const char*)) {
      return false;
    }
    if (functionList[action]) {
        return (client->*functionList[action])(down, ev);
    } else {
      printf ("No key bindings for this key.\n");
    }
  }

  bool InputManager::Initialize(iObjectRegistry *registry) {
    csConfigAccess cfg (registry, "/config/client.cfg");
    const char * str;
    str = cfg->GetStr ("Key.Up");
    actionList[PTKEY_UP] = GetActionID(str);
    str = cfg->GetStr ("Key.Down");
    actionList[PTKEY_DOWN] = GetActionID(str);
    str = cfg->GetStr ("Key.Left");
    actionList[PTKEY_LEFT] = GetActionID(str);
    str = cfg->GetStr ("Key.Right");
    actionList[PTKEY_RIGHT] = GetActionID(str);
    str = cfg->GetStr ("Key.Space");
    actionList[PTKEY_SPACE] = GetActionID(str);
    str = cfg->GetStr ("Key.PageUp");
    actionList[PTKEY_PAGEUP] = GetActionID(str);
    str = cfg->GetStr ("Key.PageDown");
    actionList[PTKEY_PAGEDOWN] = GetActionID(str);
    str = cfg->GetStr ("Key.C");
    actionList[PTKEY_C] = GetActionID(str);
    str = cfg->GetStr ("Key.D");
    actionList[PTKEY_D] = GetActionID(str);
    str = cfg->GetStr ("Key.H");
    actionList[PTKEY_H] = GetActionID(str);
    str = cfg->GetStr ("Key.J");
    actionList[PTKEY_J] = GetActionID(str);
    str = cfg->GetStr ("Key.K");
    actionList[PTKEY_K] = GetActionID(str);
    str = cfg->GetStr ("Key.Esc");
    actionList[PTKEY_ESC] = GetActionID(str);
    
  }

  int InputManager::GetActionID(const char *key) {
    for (int i = 0; i < actionCount; i++) {
      if (strcmp (key, actionStr[i]) == 0 ) {
        return i;
      }
    }
    return -1;
  }

  void InputManager::SetCallback (Client  *client, const char *actionType,
                               tFunction pFunctionPointer) {

    this->client = client;
    int actionID = GetActionID(actionType);
    if (actionID != -1) {
      functionList[actionID] = pFunctionPointer;
    }
  }

  int InputManager::GetActionType(iEvent &ev) const {
    utf32_char code = csKeyEventHelper::GetCookedCode(&ev);

    switch (code) {
      case CSKEY_UP:
        return actionList[PTKEY_UP];
        break;
      case CSKEY_DOWN:
        return actionList[PTKEY_DOWN];
        break;
      case CSKEY_RIGHT:
        return actionList[PTKEY_RIGHT];
        break;
      case CSKEY_LEFT:
        return actionList[PTKEY_LEFT];
        break;
      case CSKEY_SPACE:
        return actionList[PTKEY_SPACE];
        break;
      case CSKEY_PGUP:
        return actionList[PTKEY_PAGEUP];
        break;
      case CSKEY_PGDN:
        return actionList[PTKEY_PAGEDOWN];
        break;
      case CSKEY_ESC:
        return actionList[PTKEY_ESC];
        break;
      case 'c':
        return actionList[PTKEY_C];
        break;
      case 'd':
        return actionList[PTKEY_D];
        break;
      case 'h':
        return actionList[PTKEY_H];
        break;
      case 'j':
        return actionList[PTKEY_J];
        break;
      case 'k':
        return actionList[PTKEY_K];
        break;
    }
    return sizeof(actionStr)/sizeof(const char*) + 1; // Error
  }
}
