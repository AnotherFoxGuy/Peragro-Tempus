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

#include "stdio.h"

#include <boost/scoped_ptr.hpp>

#include "auth/network/network.h"
#include "auth/database/tables.h"

#include "common/database/sqlite/sqlite.h"

#include "common/util/sleep.h"
#include "common/util/wincrashdump.h"
#include "common/util/consoleapp.h"

#include "common/thread/threadloop.h"

#include "common/events/engine.h"

class App : public Application
{
private:
  PT::Thread::ThreadLoop<Database, PT::Thread::OwnedStorage> dbThread;
  boost::scoped_ptr<Network> network;

  int argc;
  char** argv;

public:
  App();
  virtual ~App();

  virtual int Initialize(int argc, char* argv[]);
  virtual void Run();
};

App::App()
  : dbThread(&Database::Run)
{
}

App::~App()
{
  printf("Server Shutdown initialised!\n");

  printf("- Shutdown Network:     \t");
  network->shutdown();
  printf("done\n");

  printf("- Shutdown Database:     \t");
  dbThread.Stop();
  printf("done\n");

  StringStore::destroy();

  printf("Time to quit now!\n");
}

int App::Initialize(int argc, char* argv[])
{
  App::argc = argc;
  App::argv = argv;

  setWinCrashDump(argv[0]);

  return 0;
}


void App::Run()
{
  dbThread.Set(new DbSQLite("auth_db.sqlite"));
  Tables tables;
  tables.init(dbThread.Get());
  dbThread.Start();

  EventEngine eng;

  // Register Event
  //eng.registerHandler(&dh, Events::Dummy);

  for (size_t i = 0; i<5; i++)
  {
    eng.addWorker();
  }
  eng.start();

  unsigned int port = 12345;

  // Finally initialising the network!
  network.reset(new Network);
  network->init(port);

  printf("Server up and running!\n");

  size_t sentbyte = 0, recvbyte = 0, timestamp = 0;
  size_t delay_time = 10000; //10 sec = 10000 ms

  while (IsRunning())
  {
    pt_sleep(delay_time);
    network->getStats(sentbyte, recvbyte, timestamp);
    float uptraffic = sentbyte/(float)delay_time;
    float downtraffic = recvbyte/(float)delay_time;
    if (uptraffic > 0.001 || downtraffic > 0.001)
    {
      printf("Network Usage: Up: %.2f kB/s\t Down: %.2f kB/s\n", uptraffic, downtraffic);
    }
  }
}

int main(int argc, char ** argv)
{
  return ApplicationRunner<App>::Run(argc, argv);
}

