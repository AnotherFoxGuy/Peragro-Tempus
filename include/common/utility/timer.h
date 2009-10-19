/*
    Copyright (C) 2009 Development Team of Peragro Tempus

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
 * @file timer.h
 *
 * @basic Wrapper class around OS specific time functions, for timing.
 */

#ifndef PT_UTILITY_TIMER_H
#define PT_UTILITY_TIMER_H

#include "boost/date_time/posix_time/posix_time_types.hpp"

namespace PT
{
  namespace Time
  {
    /**
     * Wrapper class around operating system specific time functions.
     */
    class Timer
    {
    public:
      /// Constructor, sets to the current time.
      Timer();
      /// Resets to the current time, optionally with a millisecond offset.
      void Reset(time_t offset = 0);
      /// Returns the milliseconds elapsed since the clock was initialized.
      time_t ElapsedMilliseconds() const;
      /// Returns the seconds elapsed since the clock was initialized.
      time_t ElapsedSeconds() const;

    private:
      boost::posix_time::ptime timeInit;
    };

  } // Time namespace
} // PT namespace

#endif // PT_UTILITY_TIMER_H
