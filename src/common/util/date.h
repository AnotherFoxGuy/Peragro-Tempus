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
 * @file date.h
 *
 * @basic Classes for dealing with game dates and times.
 */

#ifndef PT_DATE_DATE_H
#define PT_DATE_DATE_H

#include <vector>
#include <string>

namespace PT
{
  namespace Date
  {
    typedef unsigned long LongType;
    typedef long DifferenceType;
    typedef unsigned short ShortType;
    typedef std::vector<std::string> NameListType;

    struct Calendar;

    /**
     * The date split into the different units like hours and days.
     *   All units are zero based, even if we normally use them starting from
     *   one, like days and months.
     */
    struct SplitDate
    {
      /// Default constructor.
      SplitDate()
        : second(0), minute(0), hour(0), day(0), month(0), year(0)
      {}
      /// Constructor.
      SplitDate(ShortType second, ShortType minute, ShortType hour,
        ShortType day, ShortType month, ShortType year)
        : second(second), minute(minute), hour(hour), day(day), month(month),
        year(year)
      {}

      /**
       * Advance the date by the specified amount of seconds, using the
       *   calendar for reference.
       * @param cal The calendar.
       * @param seconds Seconds to advance.
       */
      void Advance(const Calendar& cal, ShortType seconds = 1);

      /// Second of minute.
      ShortType second;
      /// Minute of hour.
      ShortType minute;
      /// Hour of day.
      ShortType hour;
      /// Day of month, starting with zero.
      ShortType day;
      /// Month of year, starting with zero.
      ShortType month;
      /// Year, not since epoch.
      ShortType year;
    };

    /**
     * The date as a single value, representing seconds since the start of the
     *   current epoch.
     */
    struct IntegerDate
    {
      /// Default constructor.
      IntegerDate(LongType date = 0)
        : seconds(date)
      {}

      /// Seconds since epoch.
      LongType seconds;
    };

    /**
     * Container to return the time of the current day.
     */
    struct DayTime
    {
      /// Default constructor.
      DayTime()
        : second(0), minute(0), hour(0)
      {}
      /// Constructor.
      DayTime(ShortType s, ShortType m, ShortType h)
        : second(s), minute(m), hour(h)
      {}

      /// Second of minute.
      ShortType second;
      /// Minute of hour.
      ShortType minute;
      /// Hour of day.
      ShortType hour;
    };

    /**
     * A calendar system, used as a reference when working with date and time
     *   measurement. The members of this class are const, because the
     *   relationships between some are fixed at construction; if you want to
     *   change the calendar make a new one.
     */
    struct Calendar
    {
    public:
      /// Default constructor.
      Calendar(ShortType epoch = 256, ShortType sPM = 60, ShortType mPH = 60,
        ShortType hPD = 24, ShortType dPW = 6, ShortType wPM = 7,
        ShortType mPS = 3, ShortType sPY = 4);

      /// The year of the current epoch.
      const ShortType epoch;

      const ShortType secondsPerMinute;
      const ShortType minutesPerHour;
      const ShortType hoursPerDay;
      const ShortType daysPerWeek;
      const ShortType weeksPerMonth;
      const ShortType monthsPerSeason;
      const ShortType seasonsPerYear;

      const ShortType daysPerMonth;
      const ShortType monthsPerYear;

      const LongType secondsPerHour;
      const LongType secondsPerDay;
      const LongType secondsPerWeek;
      const LongType secondsPerMonth;
      const LongType secondsPerSeason;
      const LongType secondsPerYear;

      /// The names of the days of a week.
      const NameListType dayNames;
      /// The names of the months of a year.
      const NameListType monthNames;
      /// The names of the seasons of a year.
      const NameListType seasonNames;

      /// Split an integer date into the different units.
      SplitDate ToSplitDate(const IntegerDate& date) const;

      /// Combine a split date into a unified integer date.
      IntegerDate ToIntegerDate(const SplitDate& date) const;

      /// Get the time of the current day, from an integer date.
      DayTime ToDayTime(const IntegerDate& date) const;

      /// Get a decimal fraction representing the time of the current day.
      float FractionOfDay(const SplitDate& date) const;
      /// Get a decimal fraction representing the time of the current day.
      float FractionOfDay(const IntegerDate& date) const;

      /**
       * Advance a date by the specified seconds. Parameter seconds plus
       *   secondsPerMinute must fit into a ShortType; otherwise it will
       *   overflow during calculation, making this unsuitable for large
       *   changes; work with an integer date in that case.
       * @param date The split date to modify.
       * @param seconds The seconds to advance.
       */
      void Advance(SplitDate& date, ShortType seconds = 1) const;

      /// Calculate the day of the week.
      ShortType GetDayOfWeek(const SplitDate& date) const;
      /// Calculate the week of the month.
      ShortType GetWeekOfMonth(const SplitDate& date) const;
      /// Calculate the season of the year.
      ShortType GetSeason(const SplitDate& date) const;
    };

    bool operator<(const SplitDate& l, const SplitDate& r);

    bool operator==(const SplitDate& l, const SplitDate& r);

    inline bool operator!=(const SplitDate& l, const SplitDate& r)
    { return !(l == r); }

    inline bool operator>(const SplitDate& l, const SplitDate& r)
    { return (r < l); }

    inline bool operator<=(const SplitDate& l, const SplitDate& r)
    { return !(r < l); }

    inline bool operator>=(const SplitDate& l, const SplitDate& r)
    { return !(l < r); }

    /// Output a date to a stream. This will convert units to start with one,
    /// if that is the normal earth way, like day and month.
    std::ostream& operator<<(std::ostream& os, const SplitDate& d);
    /// Input values into a date, converted from a stream.
    std::istream& operator>>(std::istream& is, SplitDate& d);

    /// Output the time of day to a stream.
    std::ostream& operator<<(std::ostream& os, const DayTime& d);

    /// Output the calendar parameters to a stream.
    std::ostream& operator<<(std::ostream& os, const Calendar& c);

    inline void SplitDate::Advance(const Calendar& cal, ShortType seconds)
    { cal.Advance(*this, seconds); }

    inline ShortType Calendar::GetDayOfWeek(const SplitDate& date) const
    { return date.day % daysPerWeek; }

    inline ShortType Calendar::GetWeekOfMonth(const SplitDate& date) const
    { return date.day / daysPerWeek; }

    inline ShortType Calendar::GetSeason(const SplitDate& date) const
    { return date.month / monthsPerSeason; }

  } // Date namespace
} // PT namespace

#endif // PT_DATE_DATE_H
