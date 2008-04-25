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
 * @file enviroment.h
 *
 * @brief Controls the game time.
 */

#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "common/util/timer.h"
#include "common/util/sleep.h"

/**
 * Controls the current game time.
 * Broadcasts the current game time to all clients, at specified intervals.
 */
class Environment : public Timer
{
public:
  /// Constructor.
  Environment();

private:
  /// Timer implementation.
  void timeOut()
  {
    BroadcastTime();
  }

  /// Broadcast the current game time to all clients.
  void BroadcastTime();

  /// The hour of the day.
  size_t timeHour;
  /// The minute of the hour.
  size_t timeMinute;
  /// Minutes in an hour.
  size_t minutesPerHour;
  /// Hours in a day.
  size_t hoursPerDay;
  /// Game time between update broadcasts in minutes.
  size_t minutesPerUpdate;
  /// Real time between update broadcasts in tenths of seconds.
  size_t updateInterval;

};

#endif // ENVIRONMENT_H
