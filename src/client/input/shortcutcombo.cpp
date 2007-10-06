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

#include "shortcutcombo.h"

//We use '-' for our own perverted purposes of separating the keys
#define CSKEY_DASH '-'

//Number of special keys in our local Keys variable
//Keep this up-to-date or gnomes will come and eat you!
#define SPECIAL_KEY_COUNT 41
#define NOT_SPECIAL_KEY SPECIAL_KEY_COUNT+1

struct Key
{
  uint32 code;
  std::string name;
};

struct Key Keys[SPECIAL_KEY_COUNT] =
{
  { CSKEY_ALT_LEFT    , "AltL"      },
  { CSKEY_ALT_RIGHT   , "AltR"      },
  { CSKEY_CTRL_LEFT   , "CtrlL"     },
  { CSKEY_CTRL_RIGHT  , "CtrlR"     },
  { CSKEY_SHIFT_LEFT  , "ShiftL"    },
  { CSKEY_SHIFT_RIGHT , "ShiftR"    },
  { CSKEY_TAB         , "Tab"       },
  { CSKEY_SPACE       , "Space"     },
  { CSKEY_ESC         , "Esc"       },
  { CSKEY_ENTER       , "Enter"     },
  { CSKEY_BACKSPACE   , "Bs"        },
  { CSKEY_UP          , "Up"        },
  { CSKEY_DOWN        , "Down"      },
  { CSKEY_RIGHT       , "Right"     },
  { CSKEY_LEFT        , "Left"      },
  { CSKEY_PGUP        , "PageUp"    },
  { CSKEY_PGDN        , "PageDown"  },
  { CSKEY_HOME        , "Home"      },
  { CSKEY_END         , "End"       },
  { CSKEY_INS         , "Ins"       },
  { CSKEY_DEL         , "Del"       },
  { CSKEY_F1          , "F1"        },
  { CSKEY_F2          , "F2"        },
  { CSKEY_F3          , "F3"        },
  { CSKEY_F4          , "F4"        },
  { CSKEY_F5          , "F5"        },
  { CSKEY_F6          , "F6"        },
  { CSKEY_F7          , "F7"        },
  { CSKEY_F8          , "F8"        },
  { CSKEY_F9          , "F9"        },
  { CSKEY_F10         , "F10"       },
  { CSKEY_F11         , "F11"       },
  { CSKEY_F12         , "F12"       },
  { CSKEY_DASH        , "Dash"      },
  { csmbLeft          , "LMB"       },
  { csmbRight         , "RMB"       },
  { csmbMiddle        , "MMB"       },
  { csmbWheelUp       , "WLUP"      },
  { csmbWheelDown     , "WLDOWN"    },
  { csmbExtra1        , "MBX1"      },
  { csmbExtra2        , "MBX2"      }
};

/**
 * Does a lookup of key code based on key name.
 * @param name Name of the key (CtrlL, F1, letter, or printable sign).
 * @return Index of the found code-name pair in Keys array, or NOT_SPECIAL_KEY.
 */
int Lookup(const std::string& name)
{
  int i;
  for (i=0; i<SPECIAL_KEY_COUNT; i++)
    if (Keys[i].name == name) return i;
  return NOT_SPECIAL_KEY;
}

/**
 * Does a lookup of key name based on key code.
 * @param code Code of the key.
 * @return Index of the found name-code in Keys array, or NOT_SPECIAL_KEY.
 */
int Lookup(const uint32& code)
{
  int i;
  for (i=0; i<SPECIAL_KEY_COUNT; i++)
    if (Keys[i].code == code) return i;
  return NOT_SPECIAL_KEY;
}

namespace PT
{
  ShortcutCombo::ShortcutCombo()
  {
  keyCode = 0;
  shift   = false;
  alt     = false;
  ctrl    = false;
  }

  ShortcutCombo::ShortcutCombo(const iEvent& ev, bool keyboard)
  {
    if (keyboard) SetFromKeyEvent(ev);
    else SetFromMouseEvent(ev);
  }

  ShortcutCombo::ShortcutCombo(const std::string& keyStr)
  {
    SetFromConfigString(keyStr);
  }

  ShortcutCombo::~ShortcutCombo()
  {
  }

  std::string ShortcutCombo::GetConfigKey() const
  {
    std::string shortcut;
    int i;

    //First write the modifiers
    if (shift) shortcut += ("shift-");
    if (alt)   shortcut += ("alt-");
    if (ctrl)  shortcut += ("ctrl-");

    //Lookup the index of keyCode in special characters
    i = Lookup(keyCode);

    if (i == NOT_SPECIAL_KEY) 
      shortcut += keyCode;
    else 
      shortcut += Keys[i].name;

    return shortcut;
  }

  bool ShortcutCombo::SetFromConfigString(const std::string& keyStr)
  {
    size_t i;
    std::string key;

    //First check for existance of modifiers
    if (keyStr.find("shift-") == std::string::npos) shift=false;
    else shift = true;
    if (keyStr.find("alt-") == std::string::npos) alt = false;
    else alt = true;
    if (keyStr.find("ctrl-") == std::string::npos) ctrl = false;
    else ctrl = true;

    //Our actual keyCode resides after the last dash, if any
    i=keyStr.find_last_of('-');
    if (i == std::string::npos) key = keyStr;
    else key = keyStr.substr(i+1);

    //Is it a printable key?
    if (key.length() == 1)
      {
      keyCode = key[0];
      //We will treat letters only in lower-case, if user was an idiot, repair it
      //TODO: Do the same with !@# etc characters?
      if (keyCode>='A' && keyCode) keyCode+='a'-'A';
      }
    else if ((i = Lookup(key)) != NOT_SPECIAL_KEY) keyCode = Keys[i].code; //Was our key a special key
    else return false; //Failed to recognize the key

    return true; //The key was properly recognized
  }

  void ShortcutCombo::SetFromKeyEvent(const iEvent &ev)
  {
    csKeyModifiers m;

    //Get raw code
    keyCode = csKeyEventHelper::GetRawCode(&ev);
    csKeyEventHelper::GetModifiers(&ev,m);

    //Setup shift/alt/ctrl modifiers
    shift = m.modifiers[csKeyModifierTypeShift] != 0;
    alt   = m.modifiers[csKeyModifierTypeAlt] != 0;
    ctrl  = m.modifiers[csKeyModifierTypeCtrl] != 0;
  }

  void ShortcutCombo::SetFromMouseEvent(const iEvent &ev)
  {
    //This code should work, but it doesn't. Modifier retrival
    //seems to be broken.
//     csKeyModifiers m;
//
//     keyCode=csMouseEventHelper::GetButton(&ev);
//     csKeyEventHelper::GetModifiers(&ev,m);
//
//     shift=m.modifiers[csKeyModifierTypeShift];
//     alt=m.modifiers[csKeyModifierTypeAlt];
//     ctrl=m.modifiers[csKeyModifierTypeCtrl];
    int modifiers;

    if (ev.Retrieve("keyModifiers",modifiers) != csEventErrNone) return;

    keyCode = csMouseEventHelper::GetButton(&ev);

    shift = (modifiers & CSMASK_SHIFT) != 0;
    alt   = (modifiers & CSMASK_ALT) != 0;
    ctrl  = (modifiers & CSMASK_CTRL) != 0;
  }

//
//Operators needed for the std::map
//
  bool operator==(const ShortcutCombo& c1, const ShortcutCombo& c2)
  {
    return (c1.keyCode == c2.keyCode && c1.shift == c2.shift 
            && c1.ctrl == c2.ctrl && c1.alt == c2.alt);
  }

  bool operator<(const ShortcutCombo& c1, const ShortcutCombo& c2)
  {
    if (c1.keyCode<c2.keyCode) return true;

    else if (c1.keyCode > c2.keyCode) return false;
    else if (c1.shift < c2.shift) return true;
    else if (c1.alt < c2.alt) return true;
    else if (c1.ctrl < c2.ctrl) return true;
    return false;
  }
}
