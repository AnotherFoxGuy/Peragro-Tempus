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
 * @brief Controls the game time.
 */

#ifndef CLOCK_H
#define CLOCK_H

#include "common/util/timer.h"
#include "common/util/sleep.h"

class Entity;

/**
 * Controls the current game time.
 * Broadcasts the current game time to all clients, at specified intervals.
 */
class Clock : public Timer
{
public:
  /// Constructor.
  Clock();

  /**
   * Send the initial time details to an entity.
   * @param entity The entity.
   */
  void InitTime(const Entity* entity);

private:
  /// Broadcast the current game time to all clients.
  void BroadcastTime();

  /// Timer implementation.
  void timeOut();

  /// The hour of the day.
  size_t timeHour;
  /// The minute of the hour.
  size_t timeMinute;
  /// Minutes in an hour.
  size_t minutesPerHour;
  /// Hours in a day.
  size_t hoursPerDay;

  /// Real time in tenths of seconds per game minute.
  size_t realPerGame;
  /// Game minutes between update broadcasts.
  size_t broadcastInterval;

};

#endif // CLOCK_H
