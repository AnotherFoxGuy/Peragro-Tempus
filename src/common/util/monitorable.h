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

#ifndef MONITORABLE_H
#define MONITORABLE_H

#include <stdio.h>

#include "monitor.h"
#include "mutex.h"

template<class T>
class ptMonitorable
{
private: 
  Mutex mutex;
  ptMonitor<T> ref;

protected:
  ptMonitorable() { ref.set((T*)this); }
  virtual ~ptMonitorable() {};

public:
  const ptMonitor<T>& getRef() const { return ref; }

  T* getLock() const
  {
    T* const me = (T*) this;
    me->mutex.lock();
    return me;
  }

  void freeLock()
  {
    T* const me = (T*) this;
    me->mutex.unlock();
  }
};

template<class T>
class ptScopedMonitorable
{
private: 
  T* lockObj;
  T& operator=(const T &c);

public:
  ptScopedMonitorable(const T* c) 
  { 
    printf("I: Getting lock\n");
    lockObj = c->getLock(); 
  }

  ~ptScopedMonitorable() 
  { 
    printf("I: Releasing lock\n");
    lockObj->freeLock(); 
    lockObj = 0;
  }

  T* operator->()
  {
    return lockObj;
  }

  bool operator!()
  {
    return lockObj == 0;
  }

  operator T*() 
  {
    return lockObj; 
  }

};

#endif // MONITORABLE_H
