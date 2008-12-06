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

#include "common/util/date.h"
#include <iostream>
#include <iomanip>

namespace PT
{
  namespace Date
  {
    bool operator<(const SplitDate& l, const SplitDate& r)
    {
      return (l.year < r.year) ||
        ((l.year == r.year) && (l.month < r.month) ||
          ((l.month == r.month)&& (l.day < r.day) ||
            ((l.day == r.day) && (l.hour < r.hour) ||
              ((l.hour == r.hour) && (l.minute < r.minute) ||
                ((l.minute == r.minute) && (l.second < r.second)
        )))));
    }

    bool operator==(const SplitDate& l, const SplitDate& r)
    {
      return ((l.second == r.second) &&
        (l.minute == r.minute) &&
        (l.hour == r.hour) &&
        (l.day == r.day) &&
        (l.month == r.month) &&
        (l.year == r.year));
    }

    std::ostream& operator<<(std::ostream& os, const SplitDate& d)
    {
      os.width(2); os.fill('0'); os << d.hour << ':';
      os.width(2); os.fill('0'); os << d.minute << ':';
      os.width(2); os.fill('0'); os << d.second << ' ';
      // Convert day and month from zero based.
      os << (d.day + 1) << '/' << (d.month + 1) << '/' << d.year;
      return os;
    }

    std::istream& operator>>(std::istream& is, SplitDate& d)
    {
      is >> d.hour; is.ignore(256, ':');
      is >> d.minute; is.ignore(256, ':');
      is >> d.second; is.ignore(256, ' ');
      // Convert day and month to zero based.
      is >> d.day; if (d.day > 0) d.day--; is.ignore(256, '/');
      is >> d.month; if (d.month > 0) d.month--; is.ignore(256, '/');
      is >> d.year;
      return is;
    }

    std::ostream& operator<<(std::ostream& os, const DayTime& d)
    {
      os.width(2); os.fill('0'); os << d.hour << ':';
      os.width(2); os.fill('0'); os << d.minute << ':';
      os.width(2); os.fill('0'); os << d.second;
      return os;
    }

    Calendar::Calendar(ShortType epoch, ShortType sPM, ShortType mPH,
      ShortType hPD, ShortType dPW, ShortType wPM, ShortType mPS, ShortType sPY)
      : epoch(epoch),
      secondsPerMinute(sPM),
      minutesPerHour(mPH),
      hoursPerDay(hPD),
      daysPerWeek(dPW),
      weeksPerMonth(wPM),
      monthsPerSeason(mPS),
      seasonsPerYear(sPY),
      daysPerMonth(daysPerWeek * weeksPerMonth),
      monthsPerYear(monthsPerSeason * seasonsPerYear),
      secondsPerHour(secondsPerMinute * minutesPerHour),
      secondsPerDay(secondsPerHour * hoursPerDay),
      secondsPerWeek(secondsPerDay * daysPerWeek),
      secondsPerMonth(secondsPerDay * daysPerMonth),
      secondsPerSeason(secondsPerMonth * monthsPerSeason),
      secondsPerYear(secondsPerMonth * monthsPerYear)
    {}

    SplitDate Calendar::ToSplitDate(const IntegerDate& integerDate) const
    {
      SplitDate date;
      LongType remainder = integerDate.seconds;

      date.year = (remainder / secondsPerYear) + epoch;
      remainder = remainder % secondsPerYear;
      date.month = remainder / secondsPerMonth;
      remainder = remainder % secondsPerMonth;
      date.day = remainder / secondsPerDay;
      remainder = remainder % secondsPerDay;
      date.hour = remainder / secondsPerHour;
      remainder = remainder % secondsPerHour;
      date.minute = remainder / secondsPerMinute;
      date.second = remainder % secondsPerMinute;

      return date;
    }

    DayTime Calendar::ToDayTime(const IntegerDate& integerDate) const
    {
      LongType remainder = integerDate.seconds % secondsPerDay;
      ShortType hour = remainder / secondsPerHour;
      remainder = remainder % secondsPerHour;

      return DayTime(remainder % secondsPerMinute,
        remainder / secondsPerMinute, hour);
    }

    IntegerDate Calendar::ToIntegerDate(const SplitDate& date) const
    {
      if (date.year < epoch) return IntegerDate();
      else return IntegerDate
        (date.second +
          (secondsPerMinute * (date.minute +
            (minutesPerHour * (date.hour +
              (hoursPerDay * (date.day +
                (daysPerMonth * (date.month +
                  (monthsPerYear * (date.year - epoch)
        ))))))))));
    }

    float Calendar::FractionOfDay(const SplitDate& date) const
    {
      return (static_cast<float>(date.second + (secondsPerMinute *
        (date.minute + (minutesPerHour * date.hour))))
        / static_cast<float>(secondsPerDay));
    }

    float Calendar::FractionOfDay(const IntegerDate& date) const
    {
      return (static_cast<float>(date.seconds % secondsPerDay) /
        static_cast<float>(secondsPerDay));
    }

    void Calendar::Advance(SplitDate& date, ShortType seconds) const
    {
      date.second += seconds;
      LongType overflow = date.second / secondsPerMinute;
      if (overflow == 0) return;

      date.second %= secondsPerMinute;
      date.minute += overflow;
      overflow = date.minute / minutesPerHour;
      if (overflow == 0) return;

      date.minute %= minutesPerHour;
      date.hour += overflow;
      overflow = date.hour / hoursPerDay;
      if (overflow == 0) return;

      date.hour %= hoursPerDay;
      date.day += overflow;
      overflow = date.day / daysPerMonth;
      if (overflow == 0) return;

      date.day %= daysPerMonth;
      date.month += overflow;
      overflow = date.month / monthsPerYear;
      if (overflow == 0) return;

      date.month %= monthsPerYear;
      date.year += overflow;
    }

    std::ostream& operator<<(std::ostream& os, const Calendar& c)
    {
      using namespace std;
      const int w = 18;

      os << setw(w) << left << "Epoch:" << c.epoch << endl;
      os << setw(w) << left << "Seconds / Minute:" << c.secondsPerMinute << endl;
      os << setw(w) << left << "Minutes / Hour:" << c.minutesPerHour << endl;
      os << setw(w) << left << "Hours / Day:" << c.hoursPerDay << endl;
      os << setw(w) << left << "Days / Week:" << c.daysPerWeek << endl;
      os << setw(w) << left << "Weeks / Month:" << c.weeksPerMonth << endl;
      os << setw(w) << left << "Months / Season:" << c.monthsPerSeason << endl;
      os << setw(w) << left << "Seasons / Year:" << c.seasonsPerYear << endl;

      return os;
    }

  } // Date namespace
} // PT namespace

