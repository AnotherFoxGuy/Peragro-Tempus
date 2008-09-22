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
#include <assert.h>

#include "monitor.h"
#include "mutex.h"


template<class T>
class ptMonitorable
{
private:
  Mutex mutex;
  ptMonitor<T> ref;

  bool isLocked;
  PT::Thread::ThreadID threadID;

protected:
  ptMonitorable() { ref.set((T*)this); isLocked = false; threadID = -1; }
  virtual ~ptMonitorable() {};

  T* getLock() const
  {
    T* const me = (T*) this;
    me->mutex.lock();
    me->isLocked = true;
    me->threadID = PT::Thread::CurrentThreadID();
    return me;
  }

  void freeLock()
  {
    threadID = -1;
    isLocked = false;
    T* const me = (T*) this;
    me->mutex.unlock();
  }

  template<class U> friend class ptScopedMonitorable;

public:
  const ptMonitor<T>& getRef() const { return ref; }

  bool IsLocked() const { return isLocked; }
  PT::Thread::ThreadID GetThreadID() const { return threadID; }
};

template<class T>
class ptScopedMonitorable
{
private:
  T* lockObj;
  T& operator=(const T &c);

public:
  explicit ptScopedMonitorable(const T* c)
  {
    //printf("I: Getting lock (Currently %s., %d/%d)\n", c->IsLocked()?"locked":"unlocked", PT::Thread::CurrentThreadID(), c->GetThreadID());
    //assert( !(c->IsLocked() && c->GetThreadID() != PT::Thread::CurrentThreadID()) );
    //if ( c->IsLocked() && (c->GetThreadID() != PT::Thread::CurrentThreadID()) )
      //printf("E: Already locked!!!\n");
    //printf("I: Getting lock (%s)\n", c->IsLocked()?"Locked":"Unlocked");
    lockObj = c->getLock();
  }

  ~ptScopedMonitorable()
  {
    //printf("I: Releasing lock\n");
    lockObj->freeLock();
    lockObj = 0;
  }

  inline T* operator->() const
  {
    return lockObj;
  }

  inline bool operator!() const
  {
    return lockObj == 0;
  }

  inline operator T*() const
  {
    return lockObj;
  }

  inline operator const T*() const
  {
    return lockObj;
  }
};

#endif // MONITORABLE_H
