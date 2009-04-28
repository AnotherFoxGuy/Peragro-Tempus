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

#include <stdio.h>
#include <string>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include <iostream>
#include <fstream>
#include <iterator>
#include <sys/stat.h>
#include "ext/sqlite/sqlite3.h"

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

using namespace std;

// A helper function to simplify multi line option part.
template<class T>
ostream& operator<<(ostream& os, const vector<T>& v)
{
    copy(v.begin(), v.end(), ostream_iterator<T>(cout, " "));
    return os;
}

class App : public Application
{
private:
  unsigned int port;
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
  bool FileExists(const string& fileName);
};

App::App()
{
}

App::~App()
{
  printf("Server Shutdown initialised!\n");

  printf("- Shutdown Timer Engine:\t");
  if (timerEngine) timerEngine->Kill();
  printf("done\n");

  printf("- Shutdown Interaction Manager:     \t");
  if (interactionMgr) interactionMgr->shutdown();
  printf("done\n");

  printf("- Shutdown Network:     \t");
  if (network) network->shutdown();
  printf("done\n");

  printf("- Shutdown Database:     \t");
  if (db) db->shutdown();
  printf("done\n");

  // This has to be delete before the timerEngine!
  if (resourcesFactory) delete resourcesFactory;

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

  po::variables_map vm;
  // load server config file and cml options
  try
  {
    string cfgfile;
    string dbname;
    string sqlscript;

    // Declare the config file options.
    po::options_description config_options("Config file server options");
    config_options.add_options()
      ("port,p", po::value<unsigned int>(&port)->default_value(12345),
        "Set which network port number the server will use.")
      ("sqlitedb", po::value<string>(&dbname)->default_value("test_db.sqlite")
        ,"Name of SQLite database file.")
      ("sqlcreatedb", po::value<string>(&dbname)->default_value("createdb.sql")
        , "Name of SQL script to create\ndefault database.")
    ;

    // Declare the cmdline options.
    po::options_description cmdline_options("Command line server options");
    cmdline_options.add_options()
      ("help,h", "Help message")
      ("port,p", po::value<unsigned int>(&port)->default_value(12345),
       "Set which network port number the server will use.")
      ("cfg", po::value<string>(&cfgfile)->default_value("pt-server.cfg")
       ,"Name of server config file.")
    ;
    cmdline_options.add(config_options);

    // parse cmd line options
    po::store(po::parse_command_line(argc, argv, cmdline_options), vm);
    po::notify(vm);

    // parse cfg file options
    ifstream ifs( vm["cfg"].as<string>().c_str() );
    po::store(parse_config_file(ifs, config_options), vm);
    po::notify(vm);

    // clean up
    ifs.close();

    if (vm.count("help"))
    {
      cout << cmdline_options << endl;
      return 1;
    }

    // output server settings
    if (vm.count("port"))
    {
      cout << "port: "
        << vm["port"].as<unsigned int>() << endl;
    }

    if (vm.count("cfg"))
    {
       cout << "configure file: "
         << vm["cfg"].as<string>() << endl;
    }

    if (vm.count("sqlitedb"))
    {
       cout << "db name: "
         << vm["sqlitedb"].as<string>() << endl;
    }

    if (vm.count("sqlcreatedb"))
    {
       cout << "db create script: "
         << vm["sqlcreatedb"].as<string>() << endl;
    }
  }
  catch(exception& e)
  {
    cout << e.what() << endl;
    return 1;
  } // end load server options

  // Check sqlite database.
  //cout << vm["sqlitedb"].as<string>() << "\n";
  int rc;
  if (!FileExists(vm["sqlitedb"].as<string>()))
  { // no sqlite database file so create one
    sqlite3 *sqlitedb;
    rc = sqlite3_open(vm["sqlitedb"].as<string>().c_str(), &sqlitedb);
    if (rc != SQLITE_OK)
    {
      cout << "Error(" << rc << ") Creating sqlite database:"
        <<  vm["sqlitedb"].as<string>().c_str() << endl;
      return 1;
    }
    cout << "database '" << vm["sqlcreatedb"].as<string>()
      << "' created succesfully!" << endl;

    if (FileExists(vm["sqlcreatedb"].as<string>()) )
    { // database script exists, setup database tables and data using script
      fstream ifs_sql(vm["sqlcreatedb"].as<string>().c_str(),ios::in);
      string sql_line;
      char *errmsg = 0;

      // load and execute sql script.
      while(!ifs_sql.eof())
      {
        getline(ifs_sql, sql_line);
        rc = sqlite3_exec( sqlitedb, sql_line.c_str(), NULL, NULL, &errmsg );
        if (rc!=SQLITE_OK)
        { // Error executing a sql statment
          cout << "ERROR: failed executing sql statment\n"
            << "sql:\n" << sql_line.c_str() << "\n"
            << "error:\n" << errmsg << endl;
          // clean up and exit
          sqlite3_free(errmsg);
          ifs_sql.close();
          sqlite3_close(sqlitedb);
          remove (vm["sqlitedb"].as<string>().c_str()); // make sure incomplete db file goes away
          return 1;
        }
      } // end while sql
      ifs_sql.close();
      cout << "database script '" << vm["sqlcreatedb"].as<string>()
        << "' completed succesfully!" << endl;
    } // end if sqlcreatedb
    sqlite3_close(sqlitedb);
  } // end if !sqlitedb

  return 0;
}

void App::Run()
{
  //--[Initialize]--------------------------------------------------------
  server = new Server();

  db = new dbSQLite("test_db.sqlite");
  if (!db)
  {
    printf("Checking For DB init file\n");
    return;
  }

  tablemgr = new TableManager(db);
  tablemgr->Initialize();

  worldManager = new Common::World::WorldManager();

  server->setDatabase(db);
  server->SetTableManager(tablemgr);

  /// @TODO think this can be removed
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
  stringstream portStr; portStr << port;
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

bool App::FileExists(const string& fileName)
{
  struct stat fileInfo;

  // Attempt to get the file attributes
  const int statResult = stat(fileName.c_str(), &fileInfo);

  return (statResult == 0);
}

int main(int argc, char ** argv)
{
  return ApplicationRunner<App>::Run(argc, argv);
}

