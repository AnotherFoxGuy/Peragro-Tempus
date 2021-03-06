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
 * @file interfaceevent.h
 *
 * @brief Helper classes for interface events.
 */

#ifndef PT_INTERFACE_EVENT_H
#define PT_INTERFACE_EVENT_H

#include <cssysdef.h>

#include "event.h"

namespace PT
{
  namespace Events
  {
    /**
     * @ingroup events
     * Provides helper functions for interface events.
     */
    struct InterfaceHelper
    {
      /**
       * Get whether an event contains the action.
       * @param event The event.
       * @param action The action name.
       * @return True if the action is contained in the event.
       */
      static bool ContainsAction(const iEvent* event, const char* action)
      {
        const char* actionsstr = "";
        if (event->Retrieve("actions", actionsstr) != csEventErrNone)
          return Report(PT::Error, "InterfaceHelper::ContainsAction failed!");

        std::string actions = actionsstr;
        return (actions.find(action) != std::string::npos);
      }

    };

  } // Events namespace
} // PT namespace

#endif // PT_INTERFACE_EVENT_H
