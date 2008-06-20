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
 * @file pttime.h
 *
 * @basic Wrapper class around operating system specific time functions.
 */

#ifndef PT_TIME_H
#define PT_TIME_H

#include "boost/date_time/posix_time/posix_time_types.hpp"

/**
 * Wrapper class around operating system specific time functions.
 * This uses boost::posix_time, which in turn uses gettimeofday on unix and
 * ftime on win32; to measure time at millisecond resolutions without drifting
 * from the local system clock, useful for measuring game time.
 */
class PTTime
{
public:
  /// Constructor, also initializes the time.
  PTTime() { Initialize(); }
  /// Destructor.
  ~PTTime() {}

  /// Stores the current time, with a millisecond offset.
  void Initialize(time_t offset = 0)
  {
    time_init = boost::posix_time::microsec_clock::local_time();
    if (offset) time_init += boost::posix_time::milliseconds(offset);
  }

  /// Returns the milliseconds elapsed since the clock was initialized.
  time_t GetElapsedMS() const
  {
    return (boost::posix_time::microsec_clock::local_time() - time_init)
      .total_milliseconds();
  }

  /// Returns the seconds elapsed since the clock was initialized.
  time_t GetElapsedS() const
  {
    return (boost::posix_time::microsec_clock::local_time() - time_init)
      .total_seconds();
  }

private:
  boost::posix_time::ptime time_init;

};

#endif // PT_TIME_H
