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

#ifndef PT_TIME_H
#define PT_TIME_H

#include <time.h>

/// Returns the ms passed since program start
inline size_t pt_time_ms()
{
  return clock() * 1000 / CLOCKS_PER_SEC;
}

/// Returns the seconds passed since program start
inline size_t pt_time_s()
{
  return clock() / CLOCKS_PER_SEC;
}

#endif // PT_TIME_H
