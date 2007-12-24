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

#include "server/entity/entitymanager.h"
#include "server/entity/character.h"
#include "server/entity/entity.h"
#include "server/entity/itementity.h"
#include "server/entity/pcentity.h"
#include "server/entity/npcentity.h"
#include "server/entity/doorentity.h"
#include "server/entity/doormanager.h"
#include "server/entity/racemanager.h"
#include "server/entity/itemmanager.h"
#include "server/entity/statmanager.h"
#include "server/entity/skillmanager.h"
#include "server/entity/sectormanager.h"
#include "common/quest/npcdialog.h"
#include "common/quest/npcdialoganswer.h"
#include "common/quest/npcdialogmanager.h"
#include "common/util/sleep.h"
#include "common/util/timer.h"
#include "common/items/itemsid.h"

#include "server/server.h"
#include "server/entity/charactermanager.h"
#include "server/database/sqlite/sqlite.h"
#include "server/database/table-entities.h"
#include "server/loader/fileloader.h"
#include "server/loader/file-doors.h"
#include "server/loader/file-items.h"
#include "server/loader/file-sectors.h"
#include "server/entity/usermanager.h"
#include "server/useraccountmanager.h"
#include "server/environment.h"
#include "server/spawner.h"
#include "server/network/network.h"

#include "common/util/wincrashdump.h"

#include <signal.h>

int running = 2;
EntityManager* ent_mgr;

void shutdown()
{
  if (running < 2) return;
  running = 1;

  printf("Server Shutdown initialised!\n");

  delete ent_mgr;

  printf("- Shutdown Network:     \t");
  Server::getServer()->getNetwork()->shutdown();
  printf("done\n");

  printf("- Shutdown Database:     \t");
  Server::getServer()->getDatabase()->shutdown();
  printf("done\n");

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

  dbSQLite db;
  server.setDatabase(&db);

  FileLoader fileloader;

  CharacterManager char_mgr(&server);
  server.setCharacterManager(&char_mgr);

  UserManager usr_mgr;
  server.setUserManager(&usr_mgr);

  UserAccountManager usr_acc_mgr(&server);
  server.setUserAccountManager(&usr_acc_mgr);

  ent_mgr = new EntityManager();
  server.setEntityManager(ent_mgr);

  RaceManager race_mgr;
  server.setRaceManager(&race_mgr);

  ItemManager item_mgr;
  server.setItemManager(&item_mgr);

  StatManager stat_mgr;
  server.setStatManager(&stat_mgr);

  DoorManager door_mgr;
  server.setDoorManager(&door_mgr);

  SkillManager skill_mgr;
  server.setSkillManager(&skill_mgr);

  SectorManager sector_mgr;
  server.setSectorManager(&sector_mgr);

  TimerEngine timeEngine;
  timeEngine.begin();
  server.setTimerEngine(&timeEngine);

  // Initialising time broadcasting
  Environment environment;

  sector_mgr.loadFromDB(db.getSectorsTable());
  // Loading sectors from file (to be removed when server setup through the client is working)
  fileloader.getSectorsFile()->load();

  item_mgr.loadFromDB(db.getItemTable());
  // Loading items from file (to be removed when server setup through the client is working)
  fileloader.getItemsFile()->load();

  ent_mgr->loadFromDB(db.getEntityTable());

  //item_mgr.loadFromDB(db.getItemTable());
  door_mgr.loadFromDB(db.getDoorsTable());
  stat_mgr.loadFromDB(db.getStatTable());
  skill_mgr.loadFromDB(db.getSkillTable());
  race_mgr.loadFromDB(db.getRaceTable());

  // Loading doors from file (to be removed when server setup through the client is working)
  //fileloader.getDoorsFile()->load();

  Spawner spawner;
  server.setSpawner(&spawner);
  // TODO: load that from DB rather than hardcoding
  ptString room("room", 4);
  spawner.addSpawnPoint( -114, 2, 22, room, SMALLPLATEID, 60); //spawn smallplate every 10 second after picking
  spawner.addSpawnPoint( -97, 2, 88, room, BALLPOTID, 20); //spawn ballpot every 10 second after picking
  spawner.addSpawnPoint( -73, 2.5, 105, room, APPLEID, 10); //spawn apple every 10 second after picking
  spawner.addSpawnPoint( -23, 2.5, 110, room, APPLEID, 45); //spawn apple every 45 second after picking
  spawner.addSpawnPoint( 17, 2.5, 117, room, APPLEID, 75); //spawn apple every 75 second after picking
  spawner.addSpawnPoint( 85, 2.5, 108, room, APPLEID, 15); //spawn apple every 15 second after picking
  spawner.addSpawnPoint( 87.8f, 2.0f, 11.2f, room, 4, 15); //spawn what?
  spawner.start();
  //timeEngine.registerTimer(&spawner);

  NPCDialogManager dialog_mgr;
  dialog_mgr.load();

  // Finally initialising the network!
  Network network(&server);
  network.init(12345);

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

  StringStore::destroy();

  printf("Time to quit now!\n");

  return 0;
}
