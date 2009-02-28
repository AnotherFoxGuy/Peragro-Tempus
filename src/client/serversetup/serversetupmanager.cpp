/*
    Copyright (C) 2007 Development Team of Peragro Tempus

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

#include "serversetupmanager.h"

#include <iutil/document.h>

#include "client/network/network.h"
#include "common/network/netmessage.h"

#include "common/reporter/reporter.h"
#include "client/pointer/pointer.h"

#include "client/data/sector/sector.h"
#include "client/data/sector/sectordatamanager.h"

#include "client/data/door/door.h"
#include "client/data/door/doordatamanager.h"

#include "client/data/item/item.h"
#include "client/data/item/itemdatamanager.h"

#include "client/data/spawn/spawnpoint.h"
#include "client/data/spawn/spawnpointdatamanager.h"

#include "client/data/zone/zone.h"
#include "client/data/zone/zonedatamanager.h"

#include "client/pointer/pointer.h"

namespace PT
{
  namespace Misc
  {

    ServerSetupManager::ServerSetupManager()
    {
      PointerLibrary* ptrlib = PointerLibrary::getInstance();

      // Create and Initialize the DoorDataManager.
      doorDataManager = new PT::Data::DoorDataManager (ptrlib);
      if (!doorDataManager->parse())
        Report(PT::Error, "Failed to initialize DoorDataManager!");

      // Create and Initialize the ItemDataManager.
      itemDataManager = new PT::Data::ItemDataManager (ptrlib);
      if (!itemDataManager->parseItems())
        Report(PT::Error, "Failed to initialize ItemDataManager!");

      // Create and Initialize the SpawnpointDataManager.
      spawnpointDataManager = new PT::Data::SpawnPointDataManager (ptrlib);
      if (!spawnpointDataManager->parse())
        Report(PT::Error, "Failed to initialize SpawnPointDataManager!");

      // Create and Initialize the ZoneDataManager.
      zoneDataManager = new PT::Data::ZoneDataManager (ptrlib);
      if (!zoneDataManager->parse())
        Report(PT::Error, "Failed to initialize ZoneDataManager!");
    }

    ServerSetupManager::~ServerSetupManager()
    {
      delete doorDataManager;
      delete itemDataManager;
      delete spawnpointDataManager;
      delete zoneDataManager;
    }

    bool ServerSetupManager::UploadServerData()
    {
      // Be careful about the order!

      RemoveAllMessage rmmsg;

      rmmsg.setDataType(ptString::create("item-entities"));
      PointerLibrary::getInstance()->getNetwork()->send(&rmmsg);

      rmmsg.setDataType(ptString::create("mount-entities"));
      PointerLibrary::getInstance()->getNetwork()->send(&rmmsg);

      rmmsg.setDataType(ptString::create("doors"));
      PointerLibrary::getInstance()->getNetwork()->send(&rmmsg);

      rmmsg.setDataType(ptString::create("spawnpoints"));
      PointerLibrary::getInstance()->getNetwork()->send(&rmmsg);

      rmmsg.setDataType(ptString::create("items"));
      PointerLibrary::getInstance()->getNetwork()->send(&rmmsg);

      rmmsg.setDataType(ptString::create("sectors"));
      PointerLibrary::getInstance()->getNetwork()->send(&rmmsg);

      rmmsg.setDataType(ptString::create("zones"));
      PointerLibrary::getInstance()->getNetwork()->send(&rmmsg);

      Report(PT::Notify, "Not yet fully implemented");
      // TODO: Send a message to tell the server to wipe its current settings to make room for the new ones (otherwise database entries cannot be updated, nor will they be possible to remove by removing them from the XML)


      PT::Data::SectorDataManager* secmgr = PointerLibrary::getInstance()->getSectorDataManager();

      // ==[ Sectors ]=========================================================
      std::vector<PT::Data::Sector*> sectors;

      PointerLibrary::getInstance()->getSectorDataManager()->GetAllSectors(sectors);

      for (size_t i = 0; i < sectors.size(); i++ )
      {
        unsigned int sector_id=sectors[i]->GetId();
        ptString name = ptString::create(sectors[i]->GetName());
        ptString region = ptString::create(sectors[i]->GetRegion());

        Report(PT::Debug, "Loading sector, sector_id=%d, name=\"%s\", region=\"%s\"", sector_id, *name, *region);

        // Just send the data here, one sector/package
        CreateSectorMessage sectormsg;
        sectormsg.setSectorId(sector_id);
        sectormsg.setName(name);
        sectormsg.setRegion(region);

        PointerLibrary::getInstance()->getNetwork()->send(&sectormsg);
      }

      // ==[ Doors ]===========================================================
      std::vector<PT::Data::Door*> doors;

      doorDataManager->GetAllDoors(doors);

      for (size_t i = 0; i < doors.size(); i++ )
      {
        unsigned int door_id = doors[i]->GetId();
        const char* name = doors[i]->GetName().c_str();
        const char* mesh = doors[i]->GetMeshName().c_str();
        const char* sector = doors[i]->GetSectorName().c_str();

        WFMath::Point<3> position = doors[i]->GetPosition();

        const char* quest = doors[i]->GetQuestName().c_str();

        bool open = doors[i]->GetOpenState();
        bool locked = doors[i]->GetLockState();

        if (!secmgr->GetSectorByName(sector))
        {
          Report(PT::Debug, "Failed to load door, couldn't find sector \"%s\"", sector);
          continue;
        }

        unsigned int sector_id = secmgr->GetSectorByName(sector)->GetId();

        Report(PT::Debug, "Loading doors, id=%d, name=\"%s\", sector=\"%s\"", door_id, name, sector);

        // Just send the data here, one door/package. TCP will group it as suitable
        SpawnDoorMessage doormsg;
        doormsg.setDoorId(door_id);
        doormsg.setName(ptString(name, strlen(name)));
        doormsg.setMesh(ptString(mesh, strlen(mesh)));
        doormsg.setSectorId(sector_id);
        doormsg.setPos(position);
        doormsg.setAnimation(ptString(quest, strlen(quest)));

        doormsg.setIsOpen(open);
        doormsg.setIsLocked(locked);

        PointerLibrary::getInstance()->getNetwork()->send(&doormsg);
      }

      // ==[ Items ]===========================================================
      std::vector<PT::Data::Item*> items;

      itemDataManager->GetAllItems(items);

      for (size_t i = 0; i < items.size(); i++ )
      {
        unsigned int item_id = items[i]->GetId();
        ptString name = ptString::create(items[i]->GetName());
        ptString icon = ptString::create(items[i]->GetIconName());
        ptString description = ptString::create(items[i]->GetDescription());
        ptString file = ptString::create(items[i]->GetMeshFile());
        ptString mesh = ptString::create(items[i]->GetMeshName());
        float weight = items[i]->GetWeight();
        ptString equiptype = ptString::create(items[i]->GetEquiptype());

        Report(PT::Debug, "Loading item, item_id=%d, name=\"%s\", icon=\"%s\", description=\"%s\", file=\"%s\", mesh=\"%s\", weight=%f, equiptype=\"%s\"", item_id, *name, *icon, *description, *file, *mesh, weight, *equiptype);

        // Just send the data here, one item/package
        CreateItemMessage itemmsg;
        itemmsg.setItemId(item_id);
        itemmsg.setName(name);
        itemmsg.setIcon(icon);
        itemmsg.setDescription(description);
        itemmsg.setFile(file);
        itemmsg.setMesh(mesh);
        itemmsg.setWeight(weight);
        itemmsg.setEquipType(equiptype);
        PointerLibrary::getInstance()->getNetwork()->send(&itemmsg);
      }

      // ==[ SpawnPoints ]=====================================================
      std::vector<PT::Data::SpawnPoint*> spawnpoints;

      spawnpointDataManager->GetAllSpawnPoints(spawnpoints);

      for (size_t i = 0; i < spawnpoints.size(); i++ )
      {
        unsigned int itemid = spawnpoints[i]->GetItem();
        unsigned int variation = spawnpoints[i]->GetVariation();

        WFMath::Point<3> position = spawnpoints[i]->GetPosition();
        const char* sector = spawnpoints[i]->GetSectorName().c_str();

        unsigned int interval = spawnpoints[i]->GetInterval();

        if (!secmgr->GetSectorByName(sector))
        {
          Report(PT::Debug, "Failed to load spawn point, couldn't find sector \"%s\"", sector);
          continue;
        }

        unsigned int sector_id = secmgr->GetSectorByName(sector)->GetId();

        Report(PT::Debug, "Loading spawnpoint, item=%d, var=%d, %s <%.2f,%.2f,%.2f>, interval=%d\n", itemid, variation, sector, position[0], position[1], position[2], interval);

        // Just send the data here, one spawnpoint/package
        CreateSpawnPointMessage spawnmsg;
        spawnmsg.setItemId(itemid);
        spawnmsg.setVariation(variation);
        spawnmsg.setPos(position);
        spawnmsg.setSectorId(sector_id);
        spawnmsg.setInterval(interval);

        PointerLibrary::getInstance()->getNetwork()->send(&spawnmsg);
      }

      // ==[ Zones ]===========================================================
      std::vector<PT::Data::Zone*> zones;

      zoneDataManager->GetAllZones(zones);

      for (size_t i = 0; i < zones.size(); i++ )
      {
        unsigned int zoneid = zones[i]->GetId();
        ptString zonetype = zones[i]->GetType();

        Report(PT::Debug, "Loading zone, id=%d, type=%s\n", zoneid, *zonetype);

        CreateZoneMessage zonemsg;
        zonemsg.setZoneId(zoneid);
        zonemsg.setZoneType(zonetype);
        zonemsg.setNodesCount((unsigned char)zones[i]->GetSize());
        for(size_t a_i=0; a_i<zones[i]->GetSize(); a_i++)
        {
          zonemsg.setX(a_i, zones[i]->GetNode(a_i)[0]);
          zonemsg.setZ(a_i, zones[i]->GetNode(a_i)[1]);
        }

        PointerLibrary::getInstance()->getNetwork()->send(&zonemsg);
      }

      return true;
    } // end UploadServerData()

    bool ServerSetupManager::DownloadServerData()
    {
      Report(PT::Notify, "Not yet implemented");
      return true;
    } // end DownloadServerData()

  } // Data namespace
} // PT namespace
