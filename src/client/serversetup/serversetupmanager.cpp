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

#include "client/data/chat/chat.h"
#include "client/data/chat/chatdatamanager.h"

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

      // Create and Initialize the ChatDataManager.
      chatDataManager = new PT::Data::ChatDataManager (ptrlib);
      if (!chatDataManager->parse())
        Report(PT::Error, "Failed to initialize ChatDataManager!");
    }

    ServerSetupManager::~ServerSetupManager()
    {
      delete doorDataManager;
      delete itemDataManager;
      delete spawnpointDataManager;
      delete zoneDataManager;
      delete chatDataManager;
    }

    bool ServerSetupManager::UploadServerData()
    {
      // Be careful about the order!
/*
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

      rmmsg.setDataType(ptString::create("zones"));
      PointerLibrary::getInstance()->getNetwork()->send(&rmmsg);

      rmmsg.setDataType(ptString::create("chat"));
      PointerLibrary::getInstance()->getNetwork()->send(&rmmsg);

      Report(PT::Notify, "Not yet fully implemented");
      // TODO: Send a message to tell the server to wipe its current settings to make room for the new ones (otherwise database entries cannot be updated, nor will they be possible to remove by removing them from the XML)
*/
      // ==[ Doors ]===========================================================
      std::vector<PT::Data::Door*> doors;

      doorDataManager->GetAllDoors(doors);

      for (size_t i = 0; i < doors.size(); i++ )
      {
        const char* name = doors[i]->GetName().c_str();
        const char* mesh = doors[i]->GetMeshName().c_str();

        WFMath::Point<3> position = doors[i]->GetPosition();

        const char* quest = doors[i]->GetQuestName().c_str();

        bool open = doors[i]->GetOpenState();
        bool locked = doors[i]->GetLockState();

        Report(PT::Debug, "Loading doors, name=\"%s\"", name);

        // Just send the data here, one door/package. TCP will group it as suitable
        SpawnDoorMessage doormsg;
        doormsg.setEntityId(0); // 0 means create new entity.
        doormsg.setName(ptString(name, strlen(name)));
        doormsg.setMesh(ptString(mesh, strlen(mesh)));
        doormsg.setPosition(position);
        doormsg.setAnimation(ptString(quest, strlen(quest)));

        doormsg.setIsOpen(open);
        doormsg.setIsLocked(locked);

        PointerLibrary::getInstance()->getNetwork()->send(&doormsg);
      }

      // ==[ Items ]===========================================================
/*
      std::vector<PT::Data::Item*> items;

      itemDataManager->GetAllItems(items);

      for (size_t i = 0; i < items.size(); i++ )
      {
        ptString name = ptString::create(items[i]->GetName());
        ptString icon = ptString::create(items[i]->GetIconName());
        ptString description = ptString::create(items[i]->GetDescription());
        ptString file = ptString::create(items[i]->GetMeshFile());
        ptString mesh = ptString::create(items[i]->GetMeshName());
        float weight = items[i]->GetWeight();
        unsigned int equiptypeid = ptString::create(items[i]->GetEquiptype());
        unsigned int skilltypeid = ptString::create(items[i]->GetSkilltype());

        Report(PT::Debug, "Loading item, name=\"%s\", icon=\"%s\", description=\"%s\", file=\"%s\", mesh=\"%s\", weight=%f, equiptype=\"%s\"", *name, *icon, *description, *file, *mesh, weight, *equiptype);

        // Just send the data here, one item/package
        CreateItemTemplateMessage itemmsg;
        itemmsg.setItemTemplateId(0); // 0 means create new.
        itemmsg.setName(name);
        itemmsg.setMeshId(0);
        itemmsg.setIcon(icon);
        itemmsg.setDescription(description);
        itemmsg.setEquipType(equiptype);
        itemmsg.setSkillType(skilltype);
        itemmsg.setRevision(1);
        itemmsg.setFile(file);
        PointerLibrary::getInstance()->getNetwork()->send(&itemmsg);
      }
*/
      // ==[ SpawnPoints ]=====================================================
      std::vector<PT::Data::SpawnPoint*> spawnpoints;

      spawnpointDataManager->GetAllSpawnPoints(spawnpoints);

      for (size_t i = 0; i < spawnpoints.size(); i++ )
      {
        unsigned int itemid = spawnpoints[i]->GetItem();

        WFMath::Point<3> position = spawnpoints[i]->GetPosition();
        const char* sector = spawnpoints[i]->GetSectorName().c_str();

        unsigned int interval = spawnpoints[i]->GetInterval();


        Report(PT::Debug, "Loading spawnpoint, item=%d, %s <%.2f,%.2f,%.2f>, interval=%d", itemid, sector, position[0], position[1], position[2], interval);

        // Just send the data here, one spawnpoint/package
        CreateSpawnPointMessage spawnmsg;
        spawnmsg.setItemTemplateId(itemid);
        spawnmsg.setPosition(position);
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

        Report(PT::Debug, "Loading zone, id=%d, type=%s", zoneid, *zonetype);

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

      // ==[ ChatConfig ]======================================================
      for (unsigned i=0;  i<chatDataManager->GetInitGroups().size();  i++)
      {
        const char* group = (chatDataManager->GetInitGroups())[i].c_str();

        Report(PT::Debug, "Adding InitGroup %s", group);

        CreateChanDefaultMessage ccdmsg;
        ccdmsg.setIsInitChan(true);
        ccdmsg.setPermanent(false); //dummy value - not used
        ccdmsg.setGroup(ptString::create(group));

        PointerLibrary::getInstance()->getNetwork()->send(&ccdmsg);
      }

      for (unsigned i=0;  i<chatDataManager->GetDefGroups().size();  i++)
      {
        std::pair<std::string, bool> group = (chatDataManager->GetDefGroups())[i];

        Report(PT::Debug, "Adding DefGroup %s%s", group.first.c_str(), group.second?" (permanent)":"");

        CreateChanDefaultMessage ccdmsg;
        ccdmsg.setIsInitChan(false);
        ccdmsg.setPermanent(group.second);
        ccdmsg.setGroup(ptString::create(group.first.c_str()));

        PointerLibrary::getInstance()->getNetwork()->send(&ccdmsg);
      }

      std::vector<PT::Data::ChanSpace*> chanspaces;
      chatDataManager->GetAllChanSpaces(chanspaces);

      for (size_t i=0; i<chanspaces.size(); i++)
      {
        PT::Data::ChanSpace* cspace = chanspaces[i];
        if (!cspace) continue;

        Report(PT::Debug, "Loading ChanSpace %s: form=\"%s\", create(\"%s\",%d%s), join(\"%s\",%d%s), invite(\"%s\",%d)", cspace->GetType().c_str(), cspace->GetForm().c_str(), cspace->GetCreateType().c_str(), cspace->GetCreateVal(), cspace->GetCreatePerm()?",*":"", cspace->GetJoinType().c_str(), cspace->GetJoinVal(), cspace->GetJoinPerm()?",*":"", cspace->GetInviteType().c_str(), cspace->GetInviteVal());

        CreateChanSpaceMessage ccsmsg;
        ccsmsg.setName(ptString::create(cspace->GetType().c_str()));
        ccsmsg.setForm(ptString::create(cspace->GetForm().c_str()));
        ccsmsg.setCreateType(ptString::create(cspace->GetCreateType().c_str()));
        ccsmsg.setCreateVal(cspace->GetCreateVal());
        ccsmsg.setCreatePerm(cspace->GetCreatePerm());
        ccsmsg.setJoinType(ptString::create(cspace->GetJoinType().c_str()));
        ccsmsg.setJoinVal(cspace->GetJoinVal());
        ccsmsg.setJoinPerm(cspace->GetJoinPerm());
        ccsmsg.setInviteType(ptString::create(cspace->GetInviteType().c_str()));
        ccsmsg.setInviteVal(cspace->GetInviteVal());
        ccsmsg.setVisChannel(cspace->GetVisibilityChannel());
        ccsmsg.setVisMembers(cspace->GetVisibilityMembers());

        PointerLibrary::getInstance()->getNetwork()->send(&ccsmsg);
      }
      // ======================================================================

      return true;
    } // end UploadServerData()

    bool ServerSetupManager::DownloadServerData()
    {
      Report(PT::Notify, "Not yet implemented");
      return true;
    } // end DownloadServerData()

  } // Data namespace
} // PT namespace
