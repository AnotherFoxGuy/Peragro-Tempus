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
#include "server/entity/character/movementmanager.h"
#include "server/entity/character/character.h"
#include "server/entity/entity.h"
#include "server/entity/itementity.h"
#include "server/entity/itemtemplatesmanager.h"
#include "server/entity/pcentity.h"
#include "server/entity/npcentity.h"
#include "server/entity/doorentity.h"
#include "server/group/chatmanager.h"
#include "server/zone/zonemanager.h"
#include "server/quest/npcdialog.h"
#include "server/quest/npcdialoganswer.h"
#include "server/quest/npcdialogmanager.h"
#include "common/util/sleep.h"
#include "common/util/timer.h"

#include "server/server.h"
#include "common/database/sqlite/sqlite.h"
#include "server/database/tablemanager.h"
#include "server/database/table-entities.h"
#include "server/database/table-spawnpoints.h"
#include "server/database/table-config.h"
#include "server/database/table-zones.h"
#include "server/database/table-zonenodes.h"
#include "server/database/table-reputations.h"

#include "server/entity/usermanager.h"
#include "server/environment/environmentmanager.h"
#include "server/spawn/spawner.h"
#include "server/network/network.h"
#include "server/colldet/bullet.h"

#include "server/species/speciesmanager.h"

#include "common/util/wincrashdump.h"
#include "common/util/consoleapp.h"

#include "common/world/world.h"

#include "server/combat/interactionmanager.h"


class App : public Application
{
private:
  Database* db;
  TableManager* tablemgr;
  Common::World::WorldManager* worldManager;
  EntityManager* ent_mgr;
  InteractionManager* interactionMgr;
  TimerEngine * timerEngine;
  Network* network;
  Server* server;

  ResourcesFactory* resourcesFactory;

  int argc;
  char** argv;

public:
  App();
  virtual ~App();

  virtual int Initialize(int argc, char* argv[]);
  virtual void Run();
};

App::App()
{
}

App::~App()
{
  printf("Server Shutdown initialised!\n");

  printf("- Shutdown Timer Engine:\t");
  timerEngine->Kill();
  printf("done\n");

  printf("- Shutdown Interaction Manager:     \t");
  interactionMgr->shutdown();
  printf("done\n");

  printf("- Shutdown Network:     \t");
  network->shutdown();
  printf("done\n");

  printf("- Shutdown Database:     \t");
  db->shutdown();
  printf("done\n");

  // This has to be delete before the timerEngine!
  delete resourcesFactory;

  delete db;
  delete tablemgr;
  delete ent_mgr;
  delete interactionMgr;
  delete timerEngine;
  delete network;
  delete server;
  delete worldManager;

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
  //--[Initialize]--------------------------------------------------------
  server = new Server();

  db = new dbSQLite("test_db.sqlite");
  tablemgr = new TableManager(db);
  tablemgr->Initialize();

  worldManager = new Common::World::WorldManager();

  server->setDatabase(db);
  server->SetTableManager(tablemgr);

  unsigned int port = 0;
  for(int i = 1; i < argc; i++)
  {
    if (!strcmp(argv[i], "-port"))
    {
      i++;
      if (i < argc)
      {
        port = atoi(argv[i]);
      }
    }
    else
    {
      printf("Invalid argument: %s\n", argv[i]);
      printf("Valid arguments are:\n");
      printf("-port [port number] - Set which network port number the server will use.\n");
      return;
    }
  }

  if (port == 0)
  {
    ConfigTableVOp p(tablemgr->Get<ConfigTable>()->GetSingle("port"));
    if (p)
    {
      port = atoi(p->value.c_str());
    }
    else
    {
      port = 12345;
    }
  }

  // Save the port.
  std::stringstream portStr; portStr << port;
  tablemgr->Get<ConfigTable>()->Insert("port", portStr.str());

  timerEngine = new TimerEngine();
  timerEngine->Begin();
  server->setTimerEngine(timerEngine);

  interactionMgr = new InteractionManager();
  server->setInteractionManager(interactionMgr);

  UserManager usr_mgr;
  server->setUserManager(&usr_mgr);

  //Character stuff.
  MovementManager movementManager;
  server->SetMovementManager(&movementManager);

  EquipmentFactory equipmentFactory(tablemgr);
  server->SetEquipmentFactory(&equipmentFactory);

  SkillsFactory skillsFactory(tablemgr);
  server->SetSkillsFactory(&skillsFactory);

  AbilitiesFactory abilitiesFactory(tablemgr);
  server->SetAbilitiesFactory(&abilitiesFactory);

  VulnerabilitiesFactory vulnerabilitiesFactory(tablemgr);
  server->SetVulnerabilitiesFactory(&vulnerabilitiesFactory);

  ReputationsFactory reputationsFactory(tablemgr);
  server->SetReputationsFactory(&reputationsFactory);

  AttributesFactory attributesFactory(tablemgr);
  server->SetAttributesFactory(&attributesFactory);

  resourcesFactory = new ResourcesFactory(tablemgr);
  server->SetResourcesFactory(resourcesFactory);

  ItemTemplatesManager itemTemplatesManager;
  server->SetItemTemplatesManager(&itemTemplatesManager);

  SpeciesManager speciesManager;
  server->SetSpeciesManager(&speciesManager);

  ent_mgr = new EntityManager();
  server->setEntityManager(ent_mgr);
  ent_mgr->LoadFromDB(tablemgr->Get<EntityTable>());

  ChatManager::getChatManager();

  ZoneManager zone_mgr;
  server->setZoneManager(&zone_mgr);
  zone_mgr.LoadFromDB();

  EnvironmentManager environment_mgr;
  server->setEnvironmentManager(&environment_mgr);
  environment_mgr.Initialize();

  printf("Initialising collision detection... ");
  BulletCD cd;
  server->setCollisionDetection(&cd);
  cd.setup();
  printf("done\n");
  //cd.begin();

  Spawner spawner;
  server->setSpawner(&spawner);

  NPCDialogManager dialog_mgr;
  dialog_mgr.LoadFromDB();

  // Finally initialising the network!
  network = new Network(server);
  network->init(port);
  //----------------------------------------------------------------------

  printf("Server up and running!\n");

  size_t sentbyte = 0, recvbyte = 0, timestamp = 0;
  size_t delay_time = 1000; //1 sec = 1000 ms

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
