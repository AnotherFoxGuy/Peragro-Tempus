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

Clock::Clock()
{
  hour = 10;
  minute = 0;
  minutesPerHour = 60;
  hoursPerDay = 24;

  // Real time in tenths of seconds per game minute.
  realPerGame = 1;
  // Broadcast updates once a game hour.
  broadcastInterval = 60;

  this->setInterval(realPerGame);
  this->start();
}

void Clock::InitTime(const Entity* entity)
{
  InitTimeMessage time_msg;
  time_msg.setMinute(minute);
  time_msg.setHour(hour);
  time_msg.setMinutesPerHour(minutesPerHour);
  time_msg.setHoursPerDay(hoursPerDay);
  time_msg.setRealPerGame(realPerGame);

  ByteStream bs;
  time_msg.serialise(&bs);
  NetworkHelper::localcast(bs, entity);
}

void Clock::BroadcastTime()
{
  UpdateTimeMessage time_msg;
  time_msg.setMinute(minute);
  time_msg.setHour(hour);

  ByteStream bs;
  time_msg.serialise(&bs);
  NetworkHelper::broadcast(bs);
}

void Clock::timeOut()
{
  ++minute;
  while (minute >= minutesPerHour && minutesPerHour != 0)
  {
    minute -= minutesPerHour;
    ++hour;
  }
  while (hour >= hoursPerDay && hoursPerDay != 0)
  {
    hour -= hoursPerDay;
  }

  ++counter;
  if (counter >= broadcastInterval)
  {
    counter = 0;
    BroadcastTime();
  }
}

