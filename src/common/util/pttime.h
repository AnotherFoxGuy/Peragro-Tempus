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

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <time.h>
#else // not WIN32
#include <sys/time.h>
#endif // WIN32

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

  /// Stores the current time, with a millisecond offset.
  void Initialize(time_t offset = 0)
  {
#ifdef WIN32
    time_init = clock();
    time_init += (offset * CLOCKS_PER_SEC) / 1000;

#else // not WIN32
    gettimeofday(&time_init, 0);
    // The quotient of the conversion is the seconds value.
    time_init.tv_sec += offset / 1000;
    // Convert the remainder to microseconds.
    time_init.tv_usec += (offset % 1000) * 1000;

#endif // WIN32
  }

  /// Returns the milliseconds elapsed since the clock was initialized.
  time_t GetElapsedMS()
  {
#ifdef WIN32
    return (clock() - time_init) * 1000 / CLOCKS_PER_SEC;

#else // not WIN32
    timeval time_now;
    gettimeofday(&time_now, 0);
    // Truncate to milliseconds.
    return (((time_now.tv_sec - time_init.tv_sec) * 1000) +
      ((time_now.tv_usec - time_init.tv_usec) / 1000));

#endif // WIN32
  }

  /// Returns the seconds elapsed since the clock was initialized.
  time_t GetElapsedS()
  {
#ifdef WIN32
    return (clock() - time_init) / CLOCKS_PER_SEC;

#else // not WIN32
    timeval time_now;
    gettimeofday(&time_now, 0);
    return (time_now.tv_sec - time_init.tv_sec);

#endif // WIN32
  }

private:
#ifdef WIN32
  time_t time_init;

#else // not WIN32
  timeval time_init;

#endif // WIN32
};

#endif // PT_TIME_H
