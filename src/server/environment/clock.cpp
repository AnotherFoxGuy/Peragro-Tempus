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

#include "clock.h"
#include "server/network/network.h"
#include "server/network/networkhelper.h"
#include "server/entity/entity.h"

namespace PT
{
  namespace Server
  {
    namespace Environment
    {
      Clock::Clock()
        : PT::Time::Clock(), gamePerReal(6), broadcastInterval(60)
      {
        // Set interval to one second, to avoid remainders when trying to divide
        // 10 by different values of gamePerReal.
        this->setInterval(10);
        this->start();
      }

      Clock::~Clock()
      {
      }

      void Clock::InitTime(boost::shared_ptr<Entity> entity)
      {
        InitTimeMessage time_msg;

        time_msg.setSeconds(GetIntegerDate().seconds);

        boost::shared_ptr<const PT::Time::Calendar> cal(GetCalendar());
        time_msg.setEpoch(cal->epoch);
        time_msg.setSecondsPerMinute(cal->secondsPerMinute);
        time_msg.setMinutesPerHour(cal->minutesPerHour);
        time_msg.setHoursPerDay(cal->hoursPerDay);
        time_msg.setDaysPerWeek(cal->daysPerWeek);
        time_msg.setWeeksPerMonth(cal->weeksPerMonth);
        time_msg.setMonthsPerSeason(cal->monthsPerSeason);
        time_msg.setSeasonsPerYear(cal->seasonsPerYear);

        time_msg.setGamePerReal(gamePerReal);

        ByteStream bs;
        time_msg.serialise(&bs);
        NetworkHelper::localcast(bs, entity);
      }

      void Clock::BroadcastTime()
      {
        UpdateTimeMessage time_msg;
        time_msg.setSeconds(GetIntegerDate().seconds);

        ByteStream bs;
        time_msg.serialise(&bs);
        NetworkHelper::broadcast(bs);
      }

      void Clock::timeOut()
      {
        // Game seconds to advance, per real second.
        Tick(gamePerReal);

        static size_t counter = 0;
        ++counter;
        if (counter >= broadcastInterval)
        {
          counter = 0;
          BroadcastTime();
        }
      }

    } // Environment namespace
  } // Server namespace
} // PT namespace
