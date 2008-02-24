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
 
#ifndef THREAD_H
#define THREAD_H

#ifdef WIN32
  #define WIN32_LEAN_AND_MEAN
  #include "windows.h"
  #define ThreadHandle HANDLE
  #define ThreadReturn DWORD WINAPI
  #define QuitThread(); return 0;
#else
  #include <pthread.h>
  #define ThreadHandle pthread_t
  #define ThreadReturn void*
  #define QuitThread(); pthread_exit(NULL);
#endif

class Thread
{
private:
  ThreadHandle threadHandle;

  static ThreadReturn ThreadStart (void *pArg)
  {
    Thread* This = (Thread*)pArg;
    while (This->runThread) This->Run();
    QuitThread();
  };

protected:
  bool runThread;
  virtual void Run() = 0;

public:
  Thread()
  {
    runThread = false;
  }

  virtual ~Thread() {}

  void begin()
  {
    runThread = true;
#ifdef WIN32
    threadHandle = CreateThread(0,0,(LPTHREAD_START_ROUTINE)ThreadStart, this, 0, 0);
#else
    pthread_create(&threadHandle, 0, ThreadStart, this);
#endif
  }

  void end()
  {
    runThread = false;
  }

  void kill()
  {
    runThread = false;
#ifdef WIN32
    WaitForSingleObject(threadHandle, 1000);
#else
    timespec timeout;
    timeout.tv_sec = 1000;
    timeout.tv_nsec = 0;
    pthread_timedjoin_np(threadHandle, NULL, &timeout);
#endif
  }

  inline bool isRunning()
  {
    return runThread;
  }
};

#endif // THREAD_H
