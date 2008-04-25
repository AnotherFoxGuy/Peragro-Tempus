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
 * @file enviromentevent.h
 *
 * @brief Helper classes for enviroment events.
 */

#ifndef PTENVIRONMENT_EVENT_H
#define PTENVIRONMENT_EVENT_H

#include <cssysdef.h>

#include "client/event/event.h"

#include "client/reporter/reporter.h"

namespace PT
{
  namespace Events
  {
    /**
     * @ingroup events
     * Helper functions for enviroment events.
     */
    struct EnvironmentHelper
    {
      /**
        * Get the hour of the day from an event.
        * @param event The event.
        * @return The hour as a number.
        */
       static unsigned int GetTimeHour(const iEvent* event)
       {
         unsigned int hour = 0;
         if (event->Retrieve("hour", hour) != csEventErrNone)
           Report(PT::Error, "EnvironmentHelper::GetTimeHour failed!");
 
         return hour;
       }

       /**
        * Get the minute of the hour from an event.
        * @param event The event.
        * @return The minute as a number.
        */
       static unsigned int GetTimeMinute(const iEvent* event)
       {
         unsigned int minute = 0;
         if (event->Retrieve("minute", minute) != csEventErrNone)
           Report(PT::Error, "EnvironmentHelper::GetTimeMinute failed!");
 
         return minute;
       }

    };
  } // Events namespace
} // PT namespace

#endif // PTENVIRONMENT_EVENT_H
