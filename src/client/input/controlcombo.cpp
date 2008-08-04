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

#include "controlcombo.h"

// We use '-' for our own perverted purposes of separating the keys.
#define CSKEY_DASH '-'

namespace PT
{
  namespace Input
  {
    ControlCombo::Key ControlCombo::SpecialKeys[] =
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
      { csmbWheelUp       , "MWLUP"     },
      { csmbWheelDown     , "MWLDOWN"   },
      { csmbExtra1        , "MBX1"      },
      { csmbExtra2        , "MBX2"      }
    };

    const size_t ControlCombo::SPECIAL_KEY_COUNT =
      (sizeof(SpecialKeys)/sizeof(Key));
    const size_t ControlCombo::NOT_SPECIAL_KEY = SPECIAL_KEY_COUNT + 1;

    ControlCombo::ControlCombo()
      : keyCode(0), shift(false), alt(false), ctrl(false)
    {
    } // end ControlCombo()

    ControlCombo::ControlCombo(const iEvent& ev, const bool keyboard)
      : keyCode(0), shift(false), alt(false), ctrl(false)
    {
      SetFromEvent(ev, keyboard);
    } // end ControlCombo()

    ControlCombo::ControlCombo(const std::string& cfgStr)
      : keyCode(0), shift(false), alt(false), ctrl(false)
    {
      SetFromConfigString(cfgStr);
    } // end ControlCombo()

    ControlCombo::~ControlCombo()
    {
    } // end ~ControlCombo()

    std::string ControlCombo::GetAsConfigString() const
    {
      std::string combo;
      // First write the modifiers.
      if (shift) combo += "Shift-";
      if (alt) combo += "Alt-";
      if (ctrl) combo += "Ctrl-";

      // Lookup the index of keyCode in special characters.
      const size_t i = Lookup(keyCode);

      // Convert back to uppercase.
      if (i == NOT_SPECIAL_KEY) combo += toupper(keyCode);
      else combo += SpecialKeys[i].name;

      return combo;
    } // end GetConfigKey()

    bool ControlCombo::SetFromConfigString(const std::string& cfgStr)
    {
      // First check for existance of modifiers.
      if (cfgStr.find("Shift-") == std::string::npos) shift = false;
      else shift = true;
      if (cfgStr.find("Alt-") == std::string::npos) alt = false;
      else alt = true;
      if (cfgStr.find("Ctrl-") == std::string::npos) ctrl = false;
      else ctrl = true;

      // Our actual key code resides after the last dash, if any.
      size_t i = cfgStr.find_last_of('-');
      std::string key;
      if (i == std::string::npos) key = cfgStr;
      else key = cfgStr.substr(i + 1ul);

      if (key.length() == 1)
      {
        // Key is printable. Convert it to lowercase.
        // TODO: Do the same with !@# etc characters?
        keyCode = tolower(key[0]);
      }
      else if ((i = Lookup(key)) != NOT_SPECIAL_KEY)
      {
        // Key is special.
        keyCode = SpecialKeys[i].code;
      }
      else
      {
        // Failed to recognize the key.
        return false;
      }

      return true;
    } // end SetFromConfigString()

    void ControlCombo::SetFromEvent(const iEvent &ev, const bool keyboard)
    {
      // Get the raw code.
      if (keyboard)
      {
        keyCode = csKeyEventHelper::GetRawCode(&ev);
      }
      else
      {
        keyCode = csMouseEventHelper::GetButton(&ev);
      }

      csKeyModifiers m;
      csKeyEventHelper::GetModifiers(&ev,m);

      // Setup shift/alt/ctrl modifiers.
      shift = (m.modifiers[csKeyModifierTypeShift] != 0);
      alt = (m.modifiers[csKeyModifierTypeAlt] != 0);
      ctrl = (m.modifiers[csKeyModifierTypeCtrl] != 0);
    } // end SetFromEvent()

    size_t ControlCombo::Lookup(const std::string& name)
    {
      for (size_t i = 0; i < SPECIAL_KEY_COUNT; ++i)
      {
        if (SpecialKeys[i].name == name)
          return i;
      }
      return NOT_SPECIAL_KEY;
    } // end Lookup()

    size_t ControlCombo::Lookup(const uint32& code)
    {
      for (size_t i = 0; i < SPECIAL_KEY_COUNT; ++i)
      {
        if (SpecialKeys[i].code == code)
          return i;
      }
      return NOT_SPECIAL_KEY;
    } // end Lookup()

    // Operators needed for the std::map.

    bool ControlCombo::operator==(const ControlCombo& c2) const
    {
      return (keyCode == c2.keyCode && shift == c2.shift
              && ctrl == c2.ctrl && alt == c2.alt);
    }

    bool ControlCombo::operator<(const ControlCombo& c2) const
    {
      if (keyCode<c2.keyCode) return true;

      else if (keyCode > c2.keyCode) return false;
      else if (shift < c2.shift) return true;
      else if (alt < c2.alt) return true;
      else if (ctrl < c2.ctrl) return true;
      return false;
    }

  } // Input namespace
} // PT namespace
