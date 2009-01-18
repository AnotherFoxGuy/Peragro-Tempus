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

#include "event.h"

#include <boost/shared_ptr.hpp>

#include "common/util/date.h"
#include "common/reporter/reporter.h"

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
       * Get the second since the epoch from an event.
       * @param event The event.
       * @return The hour as a number.
       */
      static size_t GetSeconds(const iEvent* event)
      {
        size_t seconds = 0;
        const void* unused;
        if (event->Retrieve("seconds", unused, seconds) != csEventErrNone)
          Report(PT::Error, "EnvironmentHelper::GetSeconds failed!");

        return seconds;
      }

      /**
       * Get the calendar parameters from an event.
       * @param event The event.
       * @return The minute as a number.
       */
      static boost::shared_ptr<PT::Date::Calendar> GetCalendar(
        const iEvent* event)
      {
        using namespace PT::Date;
        unsigned short epoch = 0;
        unsigned char secondsPerMinute = 1;
        unsigned char minutesPerHour = 1;
        unsigned char hoursPerDay = 1;
        unsigned char daysPerWeek = 1;
        unsigned char weeksPerMonth = 1;
        unsigned char monthsPerSeason = 1;
        unsigned char seasonsPerYear = 1;

        if (event->Retrieve("epoch", epoch) != csEventErrNone)
          Report(PT::Error, "EnvironmentHelper::GetCalendar epoch failed!");
        if (event->Retrieve("secondsPerMinute", secondsPerMinute) != csEventErrNone)
          Report(PT::Error, "EnvironmentHelper::GetCalendar secondsPerMinute failed!");
        if (event->Retrieve("minutesPerHour", minutesPerHour) != csEventErrNone)
          Report(PT::Error, "EnvironmentHelper::GetCalendar minutesPerHour failed!");
        if (event->Retrieve("hoursPerDay", hoursPerDay) != csEventErrNone)
          Report(PT::Error, "EnvironmentHelper::GetCalendar hoursPerDay failed!");
        if (event->Retrieve("daysPerWeek", daysPerWeek) != csEventErrNone)
          Report(PT::Error, "EnvironmentHelper::GetCalendar daysPerWeek failed!");
        if (event->Retrieve("weeksPerMonth", weeksPerMonth) != csEventErrNone)
          Report(PT::Error, "EnvironmentHelper::GetCalendar weeksPerMonth failed!");
        if (event->Retrieve("monthsPerSeason", monthsPerSeason) != csEventErrNone)
          Report(PT::Error, "EnvironmentHelper::GetCalendar monthsPerSeason failed!");
        if (event->Retrieve("seasonsPerYear", seasonsPerYear) != csEventErrNone)
          Report(PT::Error, "EnvironmentHelper::GetCalendar seasonsPerYear failed!");

        return boost::shared_ptr<PT::Date::Calendar>(new Calendar(
          epoch, secondsPerMinute, minutesPerHour, hoursPerDay, daysPerWeek,
          weeksPerMonth, monthsPerSeason, seasonsPerYear));
      }

      /**
       * Get the amount of game seconds per real second.
       * @param event The event.
       * @return The real time in tenths of seconds.
       */
      static size_t GetGamePerReal(const iEvent* event)
      {
        size_t gamePerReal = 1;
        const void* unused;
        if (event->Retrieve("gamePerReal", unused, gamePerReal) != csEventErrNone)
          Report(PT::Error, "EnvironmentHelper::GetGamePerReal failed!");

        return gamePerReal;
      }

    };
  } // Events namespace
} // PT namespace

#endif // PTENVIRONMENT_EVENT_H
