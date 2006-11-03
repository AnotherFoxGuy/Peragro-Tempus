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

#ifndef _MONITOR_H_
#define _MONITOR_H_

template<class T> 
class ptMonitor
{
private:
  const T** monitor;

  /// Reference Counter.
  unsigned int* refcount;

  bool original;

public:
  ptMonitor() : monitor(0) 
  {
    original = true;
    monitor = new const T*(); 
    refcount = new unsigned int;
    *refcount = 1;
  }

  ptMonitor(const ptMonitor<T>& other)
  {
    refcount = other.refcount;
    monitor = other.monitor;
    original = false;
    (*refcount)++;
  }

  ~ptMonitor()
  {
    if (original) *monitor = 0;
    (*refcount)--;
    if (*refcount == 0)
    {
      delete refcount;
      delete monitor;
    }
  }

  void operator =(const ptMonitor<T>& other)
  {
    // Destruct old
    if (original) *monitor = 0;
    (*refcount)--;
    if (*refcount == 0)
    {
      delete refcount;
      delete monitor;
    }

    // Copy new
    refcount = other.refcount;
    monitor = other.monitor;
    original = false;
    (*refcount)++;
  }

  void set(T* ref)
  {
    *monitor = ref;
  }

  void clear()
  {
    // Destruct old
    if (original) *monitor = 0;
    (*refcount)--;
    if (*refcount == 0)
    {
      delete refcount;
      delete monitor;
    }
    monitor = 0;
  }

  const T* get() const
  {
    if (monitor == 0) return 0;
    return *monitor;
  }
};

#endif // _MONITOR_H_
