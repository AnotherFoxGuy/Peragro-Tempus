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

#include "common/entity/entitymanager.h"
#include "common/entity/racemanager.h"
#include "common/entity/itemmanager.h"
#include "common/entity/statmanager.h"
#include "common/entity/skillmanager.h"
#include "common/util/sleep.h"
#include "common/util/timer.h"

#include "server/server.h"
#include "server/charactermanager.h"
#include "server/database/sqlite/sqlite.h"
#include "server/database/table-entities.h"
#include "server/usermanager.h"
#include "server/useraccountmanager.h"
#include "server/skillengine.h"
#include "server/spawner.h"
#include "server/network/network.h"

#include "common/util/wincrashdump.h"

#include <signal.h>

int running = 2;

void shutdown()
{
  if (running < 2) return;
  running = 1;

  printf("Server Shutdown initialised!\n");

  printf("- Shutdown Network:     \t");
  Server::getServer()->getNetwork()->shutdown();
  printf("done\n");

  printf("- Shutdown Database:     \t");
  Server::getServer()->getDatabase()->shutdown();
  printf("done\n");

  //printf("- Shutdown Spawner:     \t");
  //Server::getServer()->getSpawner()->kill();
  //printf("done\n");

  printf("- Shutdown Timer Engine:\t");
  Server::getServer()->getTimerEngine()->kill();
  printf("done\n");

  running = 0;
}

void sigfunc(int sig)
{
   if(sig == SIGINT)
   {
     shutdown();
   }
}

int main(int argc, char ** argv)
{
  signal(SIGINT, sigfunc);

  setWinCrashDump(argv[0]);

  Server server;

  Network network(&server);
  network.init(12345);

  dbSQLite db;
  server.setDatabase(&db);

  CharacterManager char_mgr(&server);
  server.setCharacterManager(&char_mgr);

  UserManager usr_mgr;
  server.setUserManager(&usr_mgr);

  UserAccountManager usr_acc_mgr(&server);
  server.setUserAccountManager(&usr_acc_mgr);

  EntityManager ent_mgr;
  server.setEntityManager(&ent_mgr);

  RaceManager race_mgr;
  server.setRaceManager(&race_mgr);

  ItemManager item_mgr;
  server.setItemManager(&item_mgr);

  StatManager stat_mgr;
  server.setStatManager(&stat_mgr);

  SkillManager skill_mgr;
  server.setSkillManager(&skill_mgr);

  TimerEngine timeEngine;
  timeEngine.begin();
  server.setTimerEngine(&timeEngine);

  ent_mgr.loadFromDB(db.getEntityTable());
  item_mgr.loadFromDB(db.getItemTable());
  stat_mgr.loadFromDB(db.getStatTable());
  skill_mgr.loadFromDB(db.getSkillTable());
  race_mgr.loadFromDB(db.getRaceTable());

  //SkillEngine skillengine;
  //server.setSkillEngine(&skillengine);
  //timeEngine.registerTimer(&skillengine);

  Spawner spawner;
  server.setSpawner(&spawner);
  // TODO: load that from DB rather than hardcoding
  ptString room("room", 4);
  spawner.addSpawnPoint( -114, 2, 22, room, 3, 60); //spawn smallplate every 10 second after picking
  spawner.addSpawnPoint( -97, 2, 88, room, 2, 20); //spawn ballpot every 10 second after picking
  spawner.addSpawnPoint( -73, 2.5, 105, room, 1, 10); //spawn apple every 10 second after picking
  spawner.addSpawnPoint( -23, 2.5, 110, room, 1, 45); //spawn apple every 45 second after picking
  spawner.addSpawnPoint( 17, 2.5, 117, room, 1, 75); //spawn apple every 75 second after picking
  spawner.addSpawnPoint( 85, 2.5, 108, room, 1, 15); //spawn apple every 15 second after picking
  spawner.start();
  //timeEngine.registerTimer(&spawner);

  unsigned int sentbyte = 0, recvbyte = 0, timestamp = 0;

  unsigned int delay_time = 10000; //10 sec = 10000 ms

  //Used for testing crash handler
  //throw 1;

  CharacterEntity* test_dummy = (CharacterEntity*)ent_mgr.findByName(ptString("test-dummy", 10));
  CharacterEntity* test_dragon = (CharacterEntity*)ent_mgr.findByName(ptString("Baby Dragonfly", 14));

  if (!test_dragon) printf("test_dragon not found! \n");

  while (running > 0)
  {
    pt_sleep(delay_time);
    network.getStats(sentbyte, recvbyte, timestamp);
    printf("Network Usage: %.2f\t Down: %.2f\n", sentbyte/(float)delay_time, recvbyte/(float)delay_time);

    //Moving test-dummy slowly
    float pos[3] = {rand()*10.0f/RAND_MAX-5.0f+29.0f, 0.15f, rand()*10.0f/RAND_MAX - 5.0f + 106.2f};
    server.moveEntity(test_dummy, pos, 3.0f);
    printf("Moving Test-Dummy to: <%.2f,%.2f,%.2f>\n", pos[0], pos[1], pos[2]);

    //Moving test_dragon slowly
    float pos1[3] = {rand()*10.0f/RAND_MAX-5.0f+41.0f, 4.0f, rand()*10.0f/RAND_MAX - 5.0f + 172.0f};
    server.moveEntity(test_dragon, pos1, 3.0f);
    printf("Moving test_dragon to: <%.2f,%.2f,%.2f>\n", pos1[0], pos1[1], pos1[2]);

  }

  printf("Time to quit now!\n");

  return 0;
}
