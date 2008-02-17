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

#ifndef PTSHORTCUTCOMBO_H
#define PTSHORTCUTCOMBO_H

#include <string>
#include <csutil/event.h>

namespace PT
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
    ///Unique key code used to identfy the keyboard/mouse key press.
    uint32 keyCode;

    ///Specifies if SHIFT key was pressed.
    bool shift;
    ///Specifies if ALT key was pressed.
    bool alt;
    ///Specifies if CTRL key was pressed.
    bool ctrl;

    //Friend operators
    friend bool operator==(const ShortcutCombo& c1, const ShortcutCombo& c2);
    friend bool operator<(const ShortcutCombo& c1, const ShortcutCombo& c2);
  public:
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
     * @param ev
     */
    void SetFromMouseEvent(const iEvent &ev);
    /**
     * Setup the shortcuts using keyboard event.
     * @param ev Keyboard event.
     */
    void SetFromKeyEvent(const iEvent &ev);
  };

  ///Equality operator.
  bool operator==(const ShortcutCombo& c1, const ShortcutCombo& c2);
  ///Compares in following order: keyCode, shift, alt, ctrl.
  bool operator<(const ShortcutCombo& c1, const ShortcutCombo& c2);
}

#endif // PTSHORTCUTCOMBO_H
