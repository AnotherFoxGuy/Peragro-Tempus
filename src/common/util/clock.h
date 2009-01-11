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
 * @file clock.h
 *
 * @basic Base class for game clocks.
 */

#ifndef PT_DATE_CLOCK_H
#define PT_DATE_CLOCK_H

#include <boost/shared_ptr.hpp>

#include "common/util/date.h"
#include "common/util/mutex.h"

namespace PT
{
  namespace Date
  {
    /**
     * Represents a the time and date, by combining the current date in
     *   different formats, and the calendar associated with them. This
     *   does not track the time and update, you must advance it.
     */
    class Clock
    {
    public:
      /// Default constructor.
      Clock();
      /// Constructor with custom calendar.
      Clock(const boost::shared_ptr<const Calendar>& cal);
      /// Destructor.
      virtual ~Clock();

      /// Get a pointer to the calendar used.
      boost::shared_ptr<const Calendar> GetCalendar() const;

      /// Change the calendar used. If you override this, be sure to call this
      /// function as well, with Clock::ChangeCalendar().
      virtual void ChangeCalendar(const boost::shared_ptr<const Calendar>& cal);

      /// Advance the clock, defaulting to 1 second.
      void Tick(LongType seconds = 1);

      /// Get the current date in integer format.
      IntegerDate GetIntegerDate() const;
      /// Get the current date in split format.
      SplitDate GetSplitDate() const;
      /// Get the time of the current day.
      DayTime GetDayTime() const;

      /// Set the date in the integer format.
      void SetDate(const IntegerDate& integerDate);
      /// Set the date in the split format.
      void SetDate(const SplitDate& date);

      /// Get a decimal representing the fraction of the current day.
      float GetFractionOfDay() const;

    private:
      /// Mutex to protect the members of the clock from getting out of sync.
      mutable Mutex dateMutex;
      /// The calendar.
      boost::shared_ptr<const Calendar> calendar;
      /// The current date in integer format.
      IntegerDate integerDate;
      /// The current date in split format.
      SplitDate splitDate;
    };

    inline Clock::Clock()
      : calendar(new Calendar()), integerDate(0),
      splitDate(calendar->ToSplitDate(integerDate))
    {}

    inline Clock::~Clock()
    {}

    inline Clock::Clock(const boost::shared_ptr<const Calendar>& cal)
      : calendar(cal)
    {}

    inline boost::shared_ptr<const Calendar> Clock::GetCalendar() const
    { return calendar; }

  } // Date namespace
} // PT namespace

#endif // PT_DATE_CLOCK_H
