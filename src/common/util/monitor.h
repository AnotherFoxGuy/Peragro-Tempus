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

  bool original;

public:
  ptMonitor() : monitor(0) 
  {
    original = false;
    monitor = 0;
  }

  ptMonitor(const ptMonitor<T>& other)
  {
    monitor = other.monitor;
    original = false;
  }

  virtual ~ptMonitor()
  {
    if (original)
    {
      *monitor = 0;
      delete monitor;
    }
  }

  void operator =(const ptMonitor<T>& other)
  {
    if (original)
    {
      *monitor = 0;
      delete monitor;
    }

    original = false;
    monitor = other.monitor;
  }

  void set(T* ref)
  {
    if (original)
    {
      *monitor = 0;
      delete monitor;
    }

    original = true;

    monitor = new const T*();
    *monitor = ref;
  }

  void clear()
  {
    // Destruct old
    if (original)
    {
      delete monitor;
    }
    original = false;
    monitor = 0;
  }

  const T* get() const
  {
    if (monitor == 0) return 0;
    return *monitor;
  }
};

#endif // _MONITOR_H_
