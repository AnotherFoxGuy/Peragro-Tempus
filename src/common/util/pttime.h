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

/**
 * Wrapper class around operating system specific time functions.
 */
class PTTime
{
public:
  /// Constructor, also initializes the time.
  PTTime() { Initialize(); }
  /// Destructor.
  ~PTTime() {}

  boost::xtime time_init;

public:
  /// Stores the current time, with a millisecond offset.
  void Initialize(time_t offset = 0)
  {
    boost::xtime_get(&time_init, boost::TIME_UTC);
    // The quotient of the conversion is the seconds value.
    time_init.sec += (long) offset / 1000;
    // Convert the remainder to microseconds.
    time_init.nsec += (long) (offset % 1000) * 1000000;
  }

  /// Returns the milliseconds elapsed since the clock was initialized.
  time_t GetElapsedMS()
  {
    boost::xtime time_now;
    boost::xtime_get(&time_now, boost::TIME_UTC);
    // Truncate to milliseconds.
    return (((time_now.sec - time_init.sec) * 1000) +
      ((time_now.nsec - time_init.nsec) / 1000000));
  }

  /// Returns the seconds elapsed since the clock was initialized.
  time_t GetElapsedS()
  {
    boost::xtime time_now;
    boost::xtime_get(&time_now, boost::TIME_UTC);
    return (time_now.sec - time_init.sec);
  }
};

#endif // PT_TIME_H
