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
 * @file controlcombo.h
 *
 * @brief Provides handling of control combinations.
 */

#ifndef PT_CONTROLCOMBO_H
#define PT_CONTROLCOMBO_H

#include <string>
#include <csutil/event.h>

namespace PT
{
  namespace Input
  {
    /**
     * A control combination, of keys and mouse buttons.
     * It can extract combos from a CS configuration files, using
     * SetFromConfigString() method, or from a keyboard / mouse event using
     * SetFromKeyEvent() and SetFromMouseEvent() methods.<br>
     * It can also generate a suitable string for writing into a configuration
     * file by using GetAsConfigString().<br>
     * Base comparision operators are provided, mostly for providing interface
     * to std::map.
     */
    class ControlCombo
    {
    private:
      /// Unique key code used to identify the keyboard/mouse key press.
      uint32 keyCode;

      /// Specifies if the Shift key is held down.
      bool shift;
      /// Specifies if the Alt key is held down.
      bool alt;
      /// Specifies if the Ctrl key is held down.
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
      /// Initialize a ControlCombo to 0.
      ControlCombo();

      /**
       * Initialize a ControlCombo by extracting from an event.
       * @param ev Event used for extracting combo information.
       * @param keyboard Whether event is coming from keyboard or mouse.
       */
      ControlCombo(const iEvent& ev, const bool keyboard = true);

      /**
       * Initialize a ControlCombo by extracting from a string provided by configuration.
       * @param cfgStr Configuration string to be parsed for control combo.
       */
      ControlCombo(const std::string& cfgStr);

      /// Destructor.
      ~ControlCombo();

      /**
       * Generates a string for the configuration file.
       * @return A control combo string.
       */
      std::string GetAsConfigString() const;

      /**
       * Extracts a control combo from a configuration string.
       * @param cfgStr Configuration string to be parsed for control combo.
       * @return True for success.
       */
      bool SetFromConfigString(const std::string& cfgStr);

      /**
       * Setup the control using an event.
       * @param ev The event.
       */
      void SetFromEvent(const iEvent &ev, const bool keyboard);

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
      bool operator==(const ControlCombo& c2) const;
      /// Inequality operator.
      inline bool operator!=(const ControlCombo& c2) const
      { return !(*this==c2); }
      /// Compares in following order: keyCode, shift, alt, ctrl.
      bool operator<(const ControlCombo& c2) const;
    };

  } // Input namespace
} // PT namespace

#endif // PTSHORTCUTCOMBO_H
