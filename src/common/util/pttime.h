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

#ifdef WIN32

private:
  struct timeval 
  {
    long tv_sec;
    long tv_usec;
  };

  inline int gettimeofday(struct timeval *tp, void *tzp)
  {
    union 
    {
      long long ns100;
      FILETIME ft;
    } _now;

    GetSystemTimeAsFileTime(&_now.ft);
    tv->tv_usec = (long)(_now.ns100 % 1000000UL);
    tv->tv_sec = (long)(_now.ns100 / 1000000UL);
    return 0;
  }

#endif

  timeval time_init;

public:
  /// Stores the current time, with a millisecond offset.
  void Initialize(time_t offset = 0)
  {
    gettimeofday(&time_init, 0);
    // The quotient of the conversion is the seconds value.
    time_init.tv_sec += (long) offset / 1000;
    // Convert the remainder to microseconds.
    time_init.tv_usec += (long) (offset % 1000) * 1000;
  }

  /// Returns the milliseconds elapsed since the clock was initialized.
  time_t GetElapsedMS()
  {
    timeval time_now;
    gettimeofday(&time_now, 0);
    // Truncate to milliseconds.
    return (((time_now.tv_sec - time_init.tv_sec) * 1000) +
      ((time_now.tv_usec - time_init.tv_usec) / 1000));
  }

  /// Returns the seconds elapsed since the clock was initialized.
  time_t GetElapsedS()
  {
    timeval time_now;
    gettimeofday(&time_now, 0);
    return (time_now.tv_sec - time_init.tv_sec);
  }
};

#endif // PT_TIME_H
