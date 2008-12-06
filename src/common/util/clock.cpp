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

#include "common/util/clock.h"

namespace PT
{
  namespace Date
  {
    void Clock::ChangeCalendar(const boost::shared_ptr<const Calendar>& cal)
    {
      ptScopedMutex lock(dateMutex);
      calendar = cal;
      splitDate = cal->ToSplitDate(integerDate);
    }

    void Clock::Tick(LongType seconds)
    {
      ptScopedMutex lock(dateMutex);
      integerDate.seconds += seconds;
      calendar->Advance(splitDate, seconds);
    }

    IntegerDate Clock::GetIntegerDate() const
    {
      ptScopedMutex lock(dateMutex);
      return integerDate;
    }

    SplitDate Clock::GetSplitDate() const
    {
      ptScopedMutex lock(dateMutex);
      return splitDate;
    }

    void Clock::SetDate(LongType date)
    {
      ptScopedMutex lock(dateMutex);
      integerDate.seconds = date;
      splitDate = calendar->ToSplitDate(integerDate);
    }

    void Clock::SetDate(SplitDate date)
    {
      ptScopedMutex lock(dateMutex);
      integerDate = calendar->ToIntegerDate(date);
      splitDate = date;
    }

    DayTime Clock::GetDayTime() const
    {
      ptScopedMutex lock(dateMutex);
      return DayTime(splitDate.second, splitDate.minute, splitDate.hour);
    }

    float Clock::GetFractionOfDay() const
    {
      ptScopedMutex lock(dateMutex);
      return calendar->FractionOfDay(integerDate);
    }

  } // Date namespace
} // PT namespace

