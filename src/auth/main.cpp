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

#include "auth/network/network.h"
#include "auth/database/tables.h"

#include "common/database/sqlite/sqlite.h"

#include "common/util/sleep.h"
#include "common/util/wincrashdump.h"

#include "common/events/engine.h"

#include <signal.h>

int running = 2;

void shutdown()
{
  if (running < 2) return;
  running = 1;

  printf("Server Shutdown initialised!\n");

  printf("- Shutdown Network:     \t");
  //Server::getServer()->getNetwork()->shutdown();
  printf("done\n");

  running = 0;
}

void sigfunc(int sig)
{
   if (sig == SIGINT)
   {
     shutdown();
   }
}

int main(int argc, char ** argv)
{
  signal(SIGINT, sigfunc);

  setWinCrashDump(argv[0]);

  Tables tables;
  dbSQLite db("auth_db.sqlite");
  tables.init(&db);

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
  Network network;
  network.init(port);

  printf("Server up and running!\n");

  size_t sentbyte = 0, recvbyte = 0, timestamp = 0;
  size_t delay_time = 10000; //10 sec = 10000 ms

  while (running > 0)
  {
    pt_sleep(delay_time);
    network.getStats(sentbyte, recvbyte, timestamp);
    float uptraffic = sentbyte/(float)delay_time;
    float downtraffic = recvbyte/(float)delay_time;
    if (uptraffic > 0.001 || downtraffic > 0.001)
    {
      printf("Network Usage: Up: %.2f kB/s\t Down: %.2f kB/s\n", uptraffic, downtraffic);
    }
  }

  //cd.kill();

  StringStore::destroy();

  printf("Time to quit now!\n");

  return 0;
}
