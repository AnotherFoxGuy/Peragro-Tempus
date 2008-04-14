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
 * @file stateevent.h
 *
 * @brief Helper classes for state events.
 */

#ifndef PT_STATE_EVENT_H
#define PT_STATE_EVENT_H

#include <cssysdef.h>

#include "client/event/event.h"

#include "client/reporter/reporter.h"

namespace PT
{
  namespace Events
  {
    /**
     * @ingroup events
     * Privides helper functions for state change events.
     */
    struct StateHelper
    {
      /**
       * Get the state value from an event.
       * @param event The event.
       * @return The value of the state parameter.
       */
      static unsigned int GetState(const iEvent* event)
      {
        unsigned int state = -1;
        if (event->Retrieve("state", state) != csEventErrNone)
          Report(PT::Error, "StateHelper::GetState failed!");

        return state;
      }

      /**
       * Check if a state event has an error.
       * @param event The event.
       * @return Whether the event error flag is true.
       */
      static bool GetError(const iEvent* event)
      {
        bool state = false;
        if (event->Retrieve("error", state) != csEventErrNone)
          Report(PT::Error, "StateHelper::GetError failed!");

        return state;
      }

      /**
       * Get the error message from a state event.
       * @param event The event.
       * @return The error message.
       */
      static std::string GetErrorMessage(const iEvent* event)
      {
        const char* textstr = "";
        if (event->Retrieve("errorMessage", textstr) != csEventErrNone)
          Report(PT::Error, "StateHelper::GetErrorMessage failed!");

        std::string text = textstr;
        return text;
      }

    };

  } // Events namespace
} // PT namespace

#endif // PT_STATE_EVENT_H
