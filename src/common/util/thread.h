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
 
#ifndef _THREAD_H_
#define _THREAD_H_

#ifdef WIN32
  #include <windows.h>
  #define WaitForThread(a, b); WaitForSingleObject(a, b);
  #define ThreadHandle HANDLE
  #define ThreadReturn DWORD WINAPI
  #define QuitThread(); return 0;
#else
  #include <pthread.h>
  #define WaitForThread(a, b); pthread_join(a, NULL);
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
    runThread = true;
  }

  virtual ~Thread() {}

  void begin()
  {
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
    WaitForThread(threadHandle, 1000);
  }

  inline bool isRunning()
  {
    return runThread;
  }
};

#endif // _THREAD_H_
