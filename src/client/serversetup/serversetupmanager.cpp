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

#include "client/reporter/reporter.h"
#include "client/pointer/pointer.h"

#include "client/data/sector.h"
#include "client/data/sectordatamanager.h"

#include "client/data/door.h"
#include "client/data/doordatamanager.h"

#include "client/data/item.h"
#include "client/data/itemdatamanager.h"

#include "client/data/npc.h"
#include "client/data/npcdatamanager.h"

#include "client/data/spawnpoint.h"
#include "client/data/spawnpointdatamanager.h"

namespace PT
{
  namespace Misc
  {

    ServerSetupManager::ServerSetupManager()
    {
      // Create and Initialize the DoorDataManager.
      doorDataManager = new PT::Data::DoorDataManager ();
      if (!doorDataManager->parse())
        Report(PT::Error, "Failed to initialize DoorDataManager!");

      // Create and Initialize the ItemDataManager.
      itemDataManager = new PT::Data::ItemDataManager ();
      if (!itemDataManager->parse())
        Report(PT::Error, "Failed to initialize ItemDataManager!");

      // Create and Initialize the NpcDataManager.
      npcDataManager = new PT::Data::NpcDataManager ();
      if (!npcDataManager->parse())
        Report(PT::Error, "Failed to initialize NpcDataManager!");

      // Create and Initialize the NpcDataManager.
      spawnpointDataManager = new PT::Data::SpawnPointDataManager ();
      if (!spawnpointDataManager->parse())
        Report(PT::Error, "Failed to initialize SpawnPointDataManager!");
    }

    ServerSetupManager::~ServerSetupManager()
    {
    }

    bool ServerSetupManager::UploadServerData()
    {
      // Be careful about the order!

      RemoveAllMessage rmmsg;
      rmmsg.setDataType(ptString::create("npc-dialogs"));
      PointerLibrary::getInstance()->getNetwork()->send(&rmmsg);

      rmmsg.setDataType(ptString::create("npc-entities"));
      PointerLibrary::getInstance()->getNetwork()->send(&rmmsg);

      rmmsg.setDataType(ptString::create("item-entities"));
      PointerLibrary::getInstance()->getNetwork()->send(&rmmsg);

      rmmsg.setDataType(ptString::create("mount-entities"));
      PointerLibrary::getInstance()->getNetwork()->send(&rmmsg);

      rmmsg.setDataType(ptString::create("doors"));
      PointerLibrary::getInstance()->getNetwork()->send(&rmmsg);

      rmmsg.setDataType(ptString::create("items"));
      PointerLibrary::getInstance()->getNetwork()->send(&rmmsg);

      rmmsg.setDataType(ptString::create("sectors"));
      PointerLibrary::getInstance()->getNetwork()->send(&rmmsg);

      Report(PT::Notify, "Not yet fully implemented");
      // TODO: Send a message to tell the server to wipe its current settings to make room for the new ones (otherwise database entries cannot be updated, nor will they be possible to remove by removing them from the XML)


      PT::Data::SectorDataManager* secmgr = PointerLibrary::getInstance()->getSectorDataManager();

      // ==[ Sectors ]=============================================================
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

      // ==[ Doors ]=============================================================
      std::vector<PT::Data::Door*> doors;

      doorDataManager->GetAllDoors(doors);

      for (size_t i = 0; i < doors.size(); i++ )
      {
        unsigned int door_id = doors[i]->GetId();
        const char* name = doors[i]->GetName().c_str();
        const char* mesh = doors[i]->GetMeshName().c_str();
        const char* sector = doors[i]->GetSectorName().c_str();

        PtVector3 position = doors[i]->GetPosition();

        const char* quest = doors[i]->GetQuestName().c_str();

        bool open = doors[i]->GetOpenState();
        bool locked = doors[i]->GetLockState();

        unsigned int sector_id = secmgr->GetSectorByName(sector)->GetId();

        Report(PT::Debug, "Loading doors, id=%d, name=\"%s\", sector=\"%s\"", door_id, name, sector);

        // Just send the data here, one door/package. TCP will group it as suitable
        SpawnDoorMessage doormsg;
        doormsg.setDoorId(door_id);
        doormsg.setName(ptString(name, strlen(name)));
        doormsg.setMesh(ptString(mesh, strlen(mesh)));
        doormsg.setSectorId(sector_id);
        doormsg.setPos(position.x, position.y, position.z);
        doormsg.setAnimation(ptString(quest, strlen(quest)));

        doormsg.setIsOpen(open);
        doormsg.setIsLocked(locked);

        PointerLibrary::getInstance()->getNetwork()->send(&doormsg);
      }

      // ==[ Items ]=============================================================
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

      // ==[ NPCs ]=============================================================
      std::vector<PT::Data::Npc*> npcs;

      npcDataManager->GetAllNpcs(npcs);

      for (size_t i = 0; i < npcs.size(); i++ )
      {
        ptString name = ptString::create(npcs[i]->GetName());
        ptString mesh = ptString::create(npcs[i]->GetMeshName());
        PtVector3 position = npcs[i]->GetPosition();
        const char* sector = npcs[i]->GetSectorName().c_str();
        ptString race = ptString::create(npcs[i]->GetRace());
        const unsigned char* hair = npcs[i]->GetHairColor();
        const unsigned char* skin = npcs[i]->GetSkinColor();
        const unsigned char* decal = npcs[i]->GetDecalColor();
        unsigned int dialog = npcs[i]->GetDialog();
        ptString ai = ptString::create(npcs[i]->GetAi());

        const std::map<std::string, std::string>& settings = 
          npcs[i]->GetAllSetting();

        const std::map<int, std::pair<int, int> >& inventory = 
          npcs[i]->GetAllInventory();

        unsigned int sector_id = secmgr->GetSectorByName(sector)->GetId();

        CreateNpcMessage npcmsg;

        npcmsg.setName(name);
        npcmsg.setMesh(mesh);
        npcmsg.setPos(position.x, position.y, position.z);
        npcmsg.setSectorId(sector_id);
        npcmsg.setRace(race);
        npcmsg.setHairColour(hair);
        npcmsg.setSkinColour(skin);
        npcmsg.setDecalColour(decal);
        npcmsg.setDialog(dialog);
        npcmsg.setAi(ai);

        size_t j = 0;

        npcmsg.setAiSettingCount(settings.size());
        std::map<std::string, std::string>::const_iterator it_s;
        for (it_s = settings.begin(); it_s != settings.end(); ++it_s)
        {
          npcmsg.setKey(j, ptString::create(it_s->first));
          npcmsg.setValue(j, ptString::create(it_s->second));
          j++;
        }

        j = 0;

        // TODO: Inventory
        npcmsg.setInventoryCount(inventory.size());
        std::map<int, std::pair<int, int> >::const_iterator it_i;
        for (it_i = inventory.begin(); it_i != inventory.end(); ++it_i)
        {
          npcmsg.setSlotId(j, it_i->first);
          npcmsg.setItemId(j, it_i->second.first);
          npcmsg.setVariation(j, it_i->second.second);
          j++;
        }
        PointerLibrary::getInstance()->getNetwork()->send(&npcmsg);
      }

      // ==[ SpawnPoints ]=============================================================
      std::vector<PT::Data::SpawnPoint*> spawnpoints;

      spawnpointDataManager->GetAllSpawnPoints(spawnpoints);

      for (size_t i = 0; i < spawnpoints.size(); i++ )
      {
        unsigned int itemid = spawnpoints[i]->GetItem();
        unsigned int variation = spawnpoints[i]->GetVariation();

        PtVector3 position = spawnpoints[i]->GetPosition();
        const char* sector = spawnpoints[i]->GetSectorName().c_str();

        unsigned int interval = spawnpoints[i]->GetInterval();

        unsigned int sector_id = secmgr->GetSectorByName(sector)->GetId();

        Report(PT::Debug, "Loading spawnpoint, item=%d, var=%d, %s <%.2f,%.2f,%.2f>, interval=%d\n", itemid, variation, sector, position.x, position.y, position.z, interval);

        // Just send the data here, one spawnpoint/package
        CreateSpawnPointMessage spawnmsg;
        spawnmsg.setItemId(itemid);
        spawnmsg.setVariation(variation);
        spawnmsg.setPos(position.x, position.y, position.z);
        spawnmsg.setSectorId(sector_id);
        spawnmsg.setInterval(interval);

        PointerLibrary::getInstance()->getNetwork()->send(&spawnmsg);
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
