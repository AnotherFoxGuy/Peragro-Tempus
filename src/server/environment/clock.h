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

#ifndef PT_SERVER_CLOCK_H
#define PT_SERVER_CLOCK_H

#include "common/util/timer.h"
#include "common/util/sleep.h"
#include "common/util/clock.h"

class Entity;

namespace PT
{
  namespace Server
  {
    namespace Environment
    {
      /**
       * Controls the current game time; broadcasts to all clients at the
       *   specified interval.
       */
      class Clock : public Timer, public PT::Date::Clock
      {
      public:
        /// Constructor.
        Clock();
        /// Destructor.
        ~Clock();

        /**
         * Send the initial time details to an entity.
         * @param entity The entity.
         */
        void InitTime(boost::shared_ptr<Entity> entity);

        /// Broadcast the current game time to all clients.
        void BroadcastTime();

      private:
        /// Timer implementation.
        void timeOut();

        /// Game seconds per real second.
        unsigned int gamePerReal;
        /// Real seconds between update broadcasts.
        size_t broadcastInterval;
      };

    } // Environment namespace
  } // Server namespace
} // PT namespace

#endif // PT_SERVER_CLOCK_H
