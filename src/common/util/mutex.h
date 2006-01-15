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

#ifndef _MUTEX_H_
#define _MUTEX_H_

#ifdef WIN32
    #include <windows.h>
	#define MUTEX HANDLE
	#define LockMutex(a);   WaitForSingleObject(a, INFINITE);
	#define UnLockMutex(a); ReleaseMutex(a);
#else
    #include <pthread.h>
	#define MUTEX pthread_mutex_t
	#define LockMutex(a); pthread_mutex_lock(&a);
	#define UnLockMutex(a); pthread_mutex_unlock(&a);
#endif

class Mutex
{
private:
  MUTEX mutex;

public:
  Mutex()
  {
#ifdef WIN32
    mutex = 0;
#else
    MUTEX tmp = PTHREAD_MUTEX_INITIALIZER;
    mutex = tmp;
#endif
  }

  void lock()
  {
    LockMutex(mutex);
  }

  void unlock()
  {
    UnLockMutex(mutex)
  }
};

#endif // _MUTEX_H_
