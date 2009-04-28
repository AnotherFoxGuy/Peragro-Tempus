/*
    Copyright (C) 2009 Development Team of Peragro Tempus

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

#ifndef CONSOLEAPP_H
#define CONSOLEAPP_H

#include <signal.h>

class Application
{
private:
  static bool running;
  static void ShutDown() { running = false; }

  static void sigfunc(int sig)
  {
    if (sig == SIGINT)
      ShutDown();
  }

#if (_MSC_VER >= 1300)
  static BOOL WINAPI TrapCtrlHandler( DWORD dwCtrlType )
  {
    switch( dwCtrlType )
    {
    case CTRL_C_EVENT:
    case CTRL_CLOSE_EVENT:
    case CTRL_BREAK_EVENT:
      ShutDown();
      break;
    default:
      return FALSE;  // unhandled.Some other in the call back list can process
    }
    return TRUE; // handled the events
  }
#endif

protected:
  virtual int Initialize(int argc, char* argv[]) = 0;
  virtual void Run() = 0;

public:
  Application()
  {
    signal(SIGINT, &sigfunc);
#if (_MSC_VER >= 1300)
    SetConsoleCtrlHandler( &TrapCtrlHandler, TRUE );
#endif
  }

  virtual ~Application()
  {
  }

  static bool IsRunning() { return running; }
};

bool Application::running = true;

template <class T>
class ApplicationRunner
{
public:
  /// Run the application.
  static int Run (int argc, char* argv[])
  {
    int result;
    {
      T app;
      result = app.Initialize (argc, argv);
      if (result) return result;
      //while (app.IsRunning());
      //{
        app.Run();
      //}
    }
    return result;
  }
};

#endif // CONSOLEAPP_H
