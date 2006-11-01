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
#  define WIN32_LEAN_AND_MEAN
#  include "windows.h"
#else
#  include <pthread.h>
#endif

class Mutex
{
private:

#ifdef WIN32
    CRITICAL_SECTION mutex;
#else
    pthread_mutex_t mutex;
#endif

public:
  Mutex()
  {
#ifdef WIN32
    InitializeCriticalSection(&mutex);
#else
    pthread_mutex_init(&mutex, 0);
#endif
  }

  ~Mutex()
  {
#ifdef WIN32
    DeleteCriticalSection(&mutex);
#else
    pthread_mutex_destroy(&mutex);
#endif
  }

  void lock()
  {
#ifdef WIN32
    EnterCriticalSection(&mutex);
#else
    pthread_mutex_trylock(&mutex);
#endif
  }

  void unlock()
  {
#ifdef WIN32
    LeaveCriticalSection(&mutex);
#else
    pthread_mutex_unlock(&mutex);
#endif
  }
};

#endif // _MUTEX_H_
