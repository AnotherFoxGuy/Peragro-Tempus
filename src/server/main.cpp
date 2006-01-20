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
#include "common/entity/itemmanager.h"
#include "common/util/sleep.h"

#include "server/server.h"
#include "server/charactermanager.h"
#include "server/database/sqlite/sqlite.h"
#include "server/database/table-entities.h"
#include "server/usermanager.h"
#include "server/useraccountmanager.h"
#include "server/network/network.h"

#include "common/util/wincrashdump.h"

int main(int argc, char ** argv)
{
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

  ItemManager item_mgr;
  server.setItemManager(&item_mgr);

  ent_mgr.loadFromDB(db.getEntityTable());
  
  item_mgr.loadFromDB(db.getItemTable());

  unsigned int sentbyte = 0, recvbyte = 0, timestamp = 0;

  unsigned int delay_time = 10000; //10 sec = 10000 ms

  //Used for testing crash handler
  //int test;  (*(CharacterManager*)0).createCharacter("test",0, test);

  while (true)
  {
    sleep(delay_time);
    network.getStats(sentbyte, recvbyte, timestamp);
    printf("Network Usage: Up: %.2f\t Down: %.2f\n", sentbyte/(float)delay_time, recvbyte/(float)delay_time);
  }

  return 0;
}
