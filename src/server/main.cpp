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

#include <string>
#include <iostream>
#include <fstream>
#include <iterator>

#include <boost/make_shared.hpp>
#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include <sys/stat.h>

#include "ext/sqlite/sqlite3.h"

#include "common/util/sleep.h"
#include "common/util/timer.h"
#include "common/util/wincrashdump.h"
#include "common/util/consoleapp.h"

#include "common/thread/threadloop.h"

#include "server/server.h"
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
#include "server/zone/locationmanager.h"
#include "server/quest/npcdialog.h"
#include "server/quest/npcdialoganswer.h"
#include "server/quest/npcdialogmanager.h"
#include "server/entity/usermanager.h"
#include "server/environment/environmentmanager.h"
#include "server/spawn/spawner.h"
#include "server/network/network.h"
#include "server/colldet/bullet.h"
#include "server/species/speciesmanager.h"
#include "server/combat/interactionmanager.h"

#include "common/database/sqlite/sqlite.h"
#include "server/database/tablemanager.h"
#include "server/database/table-entities.h"
#include "server/database/table-spawnpoints.h"
#include "server/database/table-config.h"
#include "server/database/table-zones.h"
#include "server/database/table-zonenodes.h"
#include "server/database/table-reputations.h"

#include "common/world/world.h"

#include "common/event/eventmanager.h"

using namespace std;
using namespace PT::Event;
using namespace PT::Thread;

class App : public Application
{
public:
  App();
  virtual ~App();

  virtual int Initialize(int argc, char* argv[]);
  virtual void Run();
  bool FileExists(const string& fileName);

private:
  unsigned int port;
  string dbName;
  int argc;
  char** argv;

  Server server;

  struct StringStoreDestructor
  {
    ~StringStoreDestructor();
  } stringStoreDestructor;

  ThreadLoop<PT::Event::iEventManager, OwnedStorage> eventThread;
  ThreadLoop<Database, OwnedStorage> dbThread;
  ThreadLoop<TimerEngine, OwnedStorage> timerThread;
  ThreadLoop<InteractionManager, OwnedStorage> interactionThread;

  boost::scoped_ptr<Network> network;
};

App::App()
  : port(0), dbName(""), argc(0), argv(0),
  eventThread(&PT::Event::iEventManager::SendEvents), dbThread(&Database::Run),
  timerThread(&TimerEngine::Run), interactionThread(&InteractionManager::Run)
{
}

App::~App()
{
  cout << "Server Shutdown initialised!" << endl;

  cout << "- Shutdown Timer Engine:          ";
  timerThread.Stop();
  cout << "done" << endl;

  cout << "- Shutdown Interaction Manager:   ";
  interactionThread.Stop();
  cout << "done" << endl;

  cout << "- Shutdown Network:               ";
  if (network) network->shutdown();
  cout << "done" << endl;

  cout << "- Shutdown Database:              ";
  dbThread.Stop();
  cout << "done" << endl;

  cout << "- Shutdown Event Manager:         ";
  eventThread.Stop();
  cout << "done" << endl;

  cout << "Time to quit now!" << endl;
}

App::StringStoreDestructor::~StringStoreDestructor()
{
  StringStore::destroy();
}

int App::Initialize(int argc, char* argv[])
{
  App::argc = argc;
  App::argv = argv;

  setWinCrashDump(argv[0]);

  po::variables_map vm;
  // Load server config file and command line options.
  try
  {
    string cfgfile;
    string sqlscript;

    // Declare the config file options.
    po::options_description config_options("Config file server options");
    config_options.add_options()
      ("port,p", po::value<unsigned int>(&port)->default_value(12345),
        "Set which network port number the server will use.")
      ("sqlite.dbfile", po::value<string>(&dbName)->default_value("test_db.sqlite"),
        "Name of SQLite database file.")
      ("sqlite.dbcreatesql", po::value<string>(&dbName)->default_value("createdb.sql"),
        "Name of SQL script to create database.")
    ;

    // Declare the cmdline options.
    po::options_description cmdline_options("Command line server options");
    cmdline_options.add_options()
      ("help,h", "Help message")
      ("port,p", po::value<unsigned int>(&port)->default_value(12345),
        "Set which network port number the server will use.")
      ("cfg", po::value<string>(&cfgfile)->default_value("pt-server.cfg"),
        "Name of server config file.")
    ;
    cmdline_options.add(config_options);

    // Parse command line options.
    po::store(po::parse_command_line(argc, argv, cmdline_options), vm);
    po::notify(vm);

    // Parse config file options.
    ifstream ifs(vm["cfg"].as<string>().c_str());
    po::store(parse_config_file(ifs, config_options), vm);
    po::notify(vm);

    // Clean up.
    ifs.close();

    if (vm.count("help"))
    {
      cout << cmdline_options << endl;
      return 1;
    }

    // Output server settings.
    if (vm.count("port"))
    {
      port = vm["port"].as<unsigned int>();
      cout << "Port: " << port << endl;
    }

    if (vm.count("cfg"))
    {
      cout << "Configure file: " << vm["cfg"].as<string>() << endl;
    }

    if (vm.count("sqlite.dbfile"))
    {
      cout << "DB name: " << vm["sqlite.dbfile"].as<string>() << endl;
    }

    if (vm.count("sqlite.dbcreatesql"))
    {
      cout << "DB create script: " << vm["sqlite.dbcreatesql"].as<string>() << endl;
    }
  }
  catch(exception& e)
  {
    cout << e.what() << endl;
    return 1;
  } // end load server options

  // Check sqlite database.
  const string& dbName(vm["sqlite.dbfile"].as<string>());
  if (!FileExists(dbName))
  {
    // No sqlite database file so create one.
    sqlite3* sqliteDB;
    const int rc = sqlite3_open(dbName.c_str(), &sqliteDB);
    if (rc != SQLITE_OK)
    {
      cout << "Error (" << rc << ") creating sqlite database '" << dbName
        << "'" << endl;
      return 1;
    }

    cout << "database '" << dbName << "' created succesfully!" << endl;

    const string& sqlCreateDBName(vm["sqlite.dbcreatesql"].as<string>());
    if (FileExists(sqlCreateDBName) )
    {
      // Database script exists; use it to setup database tables and data.
      fstream ifsSql(sqlCreateDBName.c_str(), ios::in);
      string sqlLine;
      char* errorMsg = 0;

      // Load and execute sql script.
      while (!ifsSql.eof())
      {
        getline(ifsSql, sqlLine);
        const int rc = sqlite3_exec(sqliteDB, sqlLine.c_str(), 0, 0, &errorMsg);
        if (rc != SQLITE_OK)
        {
          // Error executing a sql statement.
          cout << "Error (" << rc << ") executing sql statment" << endl
            << "SQL: " << sqlLine << endl << "Error: " << errorMsg << endl;

          // Clean up and exit.
          sqlite3_free(errorMsg);
          ifsSql.close();
          sqlite3_close(sqliteDB);
          // Make sure incomplete db file goes away.
          remove(dbName.c_str());
          return 1;
        }
      } // end while

      ifsSql.close();
      cout << "Database script '" << sqlCreateDBName
        << "' completed succesfully" << endl;
    } // end if

    sqlite3_close(sqliteDB);
  } // end if

  return 0;
}

void App::Run()
{
  //--[Initialize]--------------------------------------------------------

  using namespace PT::Event;
  eventThread.Set(new EventManager);
  eventThread.Start();
  server.setEventManager(eventThread.Get());

  cout << "Opening database '" << dbName << "'" << endl;
  auto_ptr<DbSQLite> db(new DbSQLite(dbName.c_str()));
  if (!db.get())
  {
    cout << "Failed opening DB!" << endl;
    return;
  }
  server.setDatabase(db.get());
  dbThread.Set(db.release());
  dbThread.Start();

  TableManager tableManager(server.getDatabase());
  server.SetTableManager(&tableManager);

  /// @TODO think this can be removed
  if (port == 0)
  {
    ConfigTableVOp p(tableManager.Get<ConfigTable>()->GetSingle("port"));
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
  tableManager.Get<ConfigTable>()->Insert("port", portStr.str());

  Common::World::WorldManager worldManager;

  timerThread.Set(new TimerEngine);
  timerThread.Start();
  server.setTimerEngine(timerThread.Get());

  InteractionManager interactionManager;
  server.setInteractionManager(&interactionManager);

  UserManager userManager;
  server.setUserManager(&userManager);

  // Character stuff.
  MovementManager movementManager;
  server.SetMovementManager(&movementManager);

  EquipmentFactory equipmentFactory(&tableManager);
  server.SetEquipmentFactory(&equipmentFactory);

  SkillsFactory skillsFactory(&tableManager);
  server.SetSkillsFactory(&skillsFactory);

  AbilitiesFactory abilitiesFactory(&tableManager);
  server.SetAbilitiesFactory(&abilitiesFactory);

  VulnerabilitiesFactory vulnerabilitiesFactory(&tableManager);
  server.SetVulnerabilitiesFactory(&vulnerabilitiesFactory);

  ReputationsFactory reputationsFactory(&tableManager);
  server.SetReputationsFactory(&reputationsFactory);

  AttributesFactory attributesFactory(&tableManager);
  server.SetAttributesFactory(&attributesFactory);

  ResourcesFactory resourcesFactory(&tableManager);
  server.SetResourcesFactory(&resourcesFactory);

  ItemTemplatesManager itemTemplatesManager;
  server.SetItemTemplatesManager(&itemTemplatesManager);

  SpeciesManager speciesManager;
  server.SetSpeciesManager(&speciesManager);

  EntityManager entityManager;
  server.setEntityManager(&entityManager);
  entityManager.LoadFromDB(tableManager.Get<EntityTable>());

  ChatManager::getChatManager();

  ZoneManager zoneManager;
  server.setZoneManager(&zoneManager);
  zoneManager.LoadFromDB();

  LocationManager locationManager;
  server.setLocationManager(&locationManager);
  locationManager.LoadFromDB();

  EnvironmentManager environmentManager;
  server.setEnvironmentManager(&environmentManager);

  cout << "Initialising collision detection... ";
  BulletCD cd;
  server.setCollisionDetection(&cd);
  cd.setup();
  cout << "done" << endl;
  //cd.begin();

  Spawner spawner;
  server.setSpawner(&spawner);

  NPCDialogManager dialogManager;
  dialogManager.LoadFromDB();

  // Finally initialising the network!
  network.reset(new Network(&server));
  server.setNetwork(network.get());
  network->init(port);
  //----------------------------------------------------------------------

  cout << "Server up and running!" << endl;

  size_t sentbyte = 0, recvbyte = 0, timestamp = 0;
  size_t delay_time = 1000; //1 sec = 1000 ms

  while (IsRunning())
  {
    pt_sleep(delay_time);
    network->getStats(sentbyte, recvbyte, timestamp);
    float uptraffic = sentbyte / static_cast<float>(delay_time);
    float downtraffic = recvbyte / static_cast<float>(delay_time);
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

