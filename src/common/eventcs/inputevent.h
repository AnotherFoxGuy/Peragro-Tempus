/*
    Copyright (C) 2005 Development Team of Peragro Tempus

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
 * @file inputevent.h
 *
 * @brief Helper classes for input events.
 */

#ifndef PT_INPUT_EVENT_H
#define PT_INPUT_EVENT_H

#include <cssysdef.h>

#include "event.h"

#include "common/reporter/reporter.h"

namespace PT
{
  namespace Events
  {
    /**
     * @ingroup events
     * Provides helper functions for input events.
     */
    struct InputHelper
    {
      /**
       * Get the action name from an event.
       * @param event The event.
       * @return The action name.
       */
      static const char* GetAction(const iEvent* event)
      {
        const char* action = "";
        if (event->Retrieve("action", action) != csEventErrNone)
          Report(PT::Error, "InputHelper::GetAction failed!");
        return action;
      }

      /**
       * Get the button state from an event.
       * @param event The event.
       * @return True if the button is down, false if up.
       */
      static bool GetButtonDown(const iEvent* event)
      {
        bool state = false;
        if (event->Retrieve("buttonState", state) != csEventErrNone)
          Report(PT::Error, "InputHelper::GetButtonState failed!");
        return state;
      }

      /**
       * Get the name of the control from an event.
       * @param event The event.
       * @return The control name.
       */
      static const char* GetControl(const iEvent* event)
      {
        const char* control = "";
        if (event->Retrieve("control", control) != csEventErrNone)
          Report(PT::Error, "InputHelper::GetControl failed!");
        return control;
      }

    };
  } // Events namespace
} // PT namespace

#endif // PT_INPUT_EVENT_H
