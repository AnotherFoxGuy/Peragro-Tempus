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

#ifndef SLEEP_H
#define SLEEP_H

#include <boost/thread/thread.hpp>
#include <boost/thread/xtime.hpp>

inline void pt_sleep(size_t ms)
{
  boost::xtime::xtime_nsec_t total_ns(ms * 1000000);
  boost::xtime::xtime_sec_t seconds(total_ns / 1000000000);
  boost::xtime::xtime_nsec_t remainder_ns(total_ns % 1000000000);

  boost::xtime xt;
  boost::xtime_get(&xt, boost::TIME_UTC);
  xt.sec += seconds;
  xt.nsec += remainder_ns;
  boost::thread::sleep(xt);
}

#endif // SLEEP_H
