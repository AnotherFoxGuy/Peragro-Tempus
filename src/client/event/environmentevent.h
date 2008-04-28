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
 * @file environmentevent.h
 *
 * @brief Helper classes for environment events.
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
     * Helper functions for environment events.
     */
    struct EnvironmentHelper
    {
      /**
       * Get the hour of the day from an event.
       * @param event The event.
       * @return The hour as a number.
       */
      static size_t GetTimeHour(const iEvent* event)
      {
        unsigned char hour = 0;
        if (event->Retrieve("hour", hour) != csEventErrNone)
          Report(PT::Error, "EnvironmentHelper::GetTimeHour failed!");

        return static_cast<size_t>(hour);
      }

      /**
       * Get the minute of the hour from an event.
       * @param event The event.
       * @return The minute as a number.
       */
      static size_t GetTimeMinute(const iEvent* event)
      {
        unsigned char minute = 0;
        if (event->Retrieve("minute", minute) != csEventErrNone)
          Report(PT::Error, "EnvironmentHelper::GetTimeMinute failed!");

        return static_cast<size_t>(minute);
      }

      /**
       * Get the number of minutes per hour of game time.
       * @param event The event.
       * @return The minutes per hour.
       */
      static size_t GetMinutesPerHour(const iEvent* event)
      {
        size_t minutesPerHour = 0;
        if (event->Retrieve("minutesperhour", minutesPerHour) != csEventErrNone)
          Report(PT::Error, "EnvironmentHelper::GetMinutesPerHour failed!");

        return minutesPerHour;
      }

      /**
       * Get the number of hours per day of game time.
       * @param event The event.
       * @return The hours per day.
       */
      static size_t GetHoursPerDay(const iEvent* event)
      {
        size_t hoursPerDay = 0;
        if (event->Retrieve("hoursperday", hoursPerDay) != csEventErrNone)
          Report(PT::Error, "EnvironmentHelper::GetHoursPerDay failed!");

        return hoursPerDay;
      }

      /**
       * Get the real time in tenths of seconds per minute of game time.
       * @param event The event.
       * @return The real time in tenths of seconds.
       */
      static size_t GetRealPerGame(const iEvent* event)
      {
        size_t realPerGame = 0;
        if (event->Retrieve("realpergame", realPerGame) != csEventErrNone)
          Report(PT::Error, "EnvironmentHelper::GetRealPerGame failed!");

        return realPerGame;
      }

    };
  } // Events namespace
} // PT namespace

#endif // PTENVIRONMENT_EVENT_H
