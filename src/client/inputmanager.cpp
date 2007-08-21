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
    return false;
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
    str = cfg->GetStr ("Key.A");
    actionList[PTKEY_A] = GetActionID(str);
    str = cfg->GetStr ("Key.B");
    actionList[PTKEY_B] = GetActionID(str);
    str = cfg->GetStr ("Key.C");
    actionList[PTKEY_C] = GetActionID(str);
    str = cfg->GetStr ("Key.D");
    actionList[PTKEY_D] = GetActionID(str);
    str = cfg->GetStr ("Key.E");
    actionList[PTKEY_E] = GetActionID(str);
    str = cfg->GetStr ("Key.F");
    actionList[PTKEY_F] = GetActionID(str);
    str = cfg->GetStr ("Key.G");
    actionList[PTKEY_G] = GetActionID(str);
    str = cfg->GetStr ("Key.H");
    actionList[PTKEY_H] = GetActionID(str);
    str = cfg->GetStr ("Key.I");
    actionList[PTKEY_I] = GetActionID(str);
    str = cfg->GetStr ("Key.K");
    actionList[PTKEY_K] = GetActionID(str);
    str = cfg->GetStr ("Key.L");
    actionList[PTKEY_L] = GetActionID(str);
    str = cfg->GetStr ("Key.M");
    actionList[PTKEY_M] = GetActionID(str);
    str = cfg->GetStr ("Key.N");
    actionList[PTKEY_N] = GetActionID(str);
    str = cfg->GetStr ("Key.O");
    actionList[PTKEY_O] = GetActionID(str);
    str = cfg->GetStr ("Key.P");
    actionList[PTKEY_P] = GetActionID(str);
    str = cfg->GetStr ("Key.Q");
    actionList[PTKEY_Q] = GetActionID(str);
    str = cfg->GetStr ("Key.R");
    actionList[PTKEY_R] = GetActionID(str);
    str = cfg->GetStr ("Key.S");
    actionList[PTKEY_S] = GetActionID(str);
    str = cfg->GetStr ("Key.T");
    actionList[PTKEY_T] = GetActionID(str);
    str = cfg->GetStr ("Key.U");
    actionList[PTKEY_U] = GetActionID(str);
    str = cfg->GetStr ("Key.V");
    actionList[PTKEY_V] = GetActionID(str);
    str = cfg->GetStr ("Key.W");
    actionList[PTKEY_W] = GetActionID(str);
    str = cfg->GetStr ("Key.X");
    actionList[PTKEY_X] = GetActionID(str);
    str = cfg->GetStr ("Key.Y");
    actionList[PTKEY_Y] = GetActionID(str);
    str = cfg->GetStr ("Key.Z");
    actionList[PTKEY_Z] = GetActionID(str);
    str = cfg->GetStr ("Key.Esc");
    actionList[PTKEY_ESC] = GetActionID(str);

    return true;
    
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
      case 'a':
        return actionList[PTKEY_A];
        break;
      case 'b':
        return actionList[PTKEY_B];
        break;
      case 'c':
        return actionList[PTKEY_C];
        break;
      case 'd':
        return actionList[PTKEY_D];
        break;
      case 'e':
        return actionList[PTKEY_E];
        break;
      case 'f':
        return actionList[PTKEY_F];
        break;
      case 'g':
        return actionList[PTKEY_G];
        break;
      case 'h':
        return actionList[PTKEY_H];
        break;
      case 'i':
        return actionList[PTKEY_I];
        break;
      case 'k':
        return actionList[PTKEY_K];
        break;
      case 'l':
        return actionList[PTKEY_L];
        break;
      case 'm':
        return actionList[PTKEY_M];
        break;
      case 'n':
        return actionList[PTKEY_N];
        break;
      case 'o':
        return actionList[PTKEY_O];
        break;
      case 'p':
        return actionList[PTKEY_P];
        break;
      case 'q':
        return actionList[PTKEY_Q];
        break;
      case 'r':
        return actionList[PTKEY_R];
        break;
      case 's':
        return actionList[PTKEY_S];
        break;
      case 't':
        return actionList[PTKEY_T];
        break;
      case 'u':
        return actionList[PTKEY_U];
        break;
      case 'v':
        return actionList[PTKEY_V];
        break;
      case 'w':
        return actionList[PTKEY_W];
        break;
      case 'x':
        return actionList[PTKEY_X];
        break;
      case 'y':
        return actionList[PTKEY_Y];
        break;
      case 'z':
        return actionList[PTKEY_Z];
        break;
    }
    return sizeof(actionStr)/sizeof(const char*) + 1; // Error
  }
}
