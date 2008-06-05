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
/**
 * @file shortcutcombo.h
 *
 * @brief Provides handling of shortcut key combos.
 */

#ifndef PTSHORTCUTCOMBO_H
#define PTSHORTCUTCOMBO_H

#include <string>
#include <csutil/event.h>

namespace PT
{
  namespace Input
  {
    /**
     * It can extract combos from a CS configuration files, using
     * SetFromConfigString() method, or from a keyboard/mouse iEvent using
     * SetFromKeyEvent() and SetFromMouseEvent() methods.<br>
     * It can also generate a suitable string for writing it into configuration
     * file by using GetConfigKey().<br>
     * Base comparision operators are provided, mostly for providing interface
     * to std::map.
     * @brief Provides handling of shortcut key combos.
     * @author Branko Majic <branko.majic NO SPAM AT gmail.com>
     */
    class ShortcutCombo
    {
    private:
      /// Unique key code used to identify the keyboard/mouse key press.
      uint32 keyCode;

      /// Specifies if the SHIFT key is held down.
      bool shift;
      /// Specifies if the ALT key is held down.
      bool alt;
      /// Specifies if the CTRL key is held down.
      bool ctrl;

      /// A key.
      struct Key
      {
        /// The keycode.
        uint32 code;
        /// The key name.
        std::string name;
      };
      /// The static array of special keys.
      static Key SpecialKeys[];
      /// Number of special keys in our local Keys variable.
      static const size_t SPECIAL_KEY_COUNT;
      static const size_t NOT_SPECIAL_KEY;
    public:
      /// Initialize a ShortcutCombo to 0.
      ShortcutCombo();

      /**
       * Initialize a ShortcutCombo by extracting combo from an event.
       * @param ev Event used for extracting combo information.
       * @param keyboard Whether event is coming from keyboard or mouse.
       */
      ShortcutCombo(const iEvent& ev, bool keyboard=true);

      /**
       * Initialize a ShortcutCombo by extracting combo from a string provided by configuration.
       * @param keyStr Configuration string to be parsed for shortcut combo.
       */
      ShortcutCombo(const std::string& keyStr);

      /// Destructor.
      ~ShortcutCombo();

      /**
       * Generates a key for configuration file.
       * @return CS config-ready key.
       */
      std::string GetConfigKey() const;

      /**
       * Extracts a shortcut combo from a key string.
       * @param keyStr Configuration string to be parsed for shortcut combo.
       * @return True if all went well, false if an error occured.
       */
      bool SetFromConfigString(const std::string& keyStr);

      /**
       * Setup the shortcuts using mouse event.
       * @param ev Mouse event.
       */
      void SetFromMouseEvent(const iEvent &ev);

      /**
       * Setup the shortcuts using keyboard event.
       * @param ev Keyboard event.
       */
      void SetFromKeyEvent(const iEvent &ev);

      /**
       * Does a lookup of key code based on key name.
       * @param name Name of the key (CtrlL, F1, letter, or printable sign).
       * @return Index of the found code-name pair in Keys array, or NOT_SPECIAL_KEY.
       */
      static size_t Lookup(const std::string& name);

      /**
       * Does a lookup of key name based on key code.
       * @param code Code of the key.
       * @return Index of the found name-code in Keys array, or NOT_SPECIAL_KEY.
       */
      static size_t Lookup(const uint32& code);

      /// Equality operator.
      bool operator==(const ShortcutCombo& c2) const;
      /// Compares in following order: keyCode, shift, alt, ctrl.
      bool operator<(const ShortcutCombo& c2) const;
    };

  } // Input namespace
} // PT namespace

#endif // PTSHORTCUTCOMBO_H
