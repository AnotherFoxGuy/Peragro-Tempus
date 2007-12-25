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

// Comment this include out to build on Linux (currently a GCC specific problem)
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

namespace PT
{
  namespace Misc
  {

    ServerSetupManager::ServerSetupManager()
    {
    }

    ServerSetupManager::~ServerSetupManager()
    {
    }

    bool ServerSetupManager::UploadServerData()
    {
      Report(PT::Notify, "Not yet fully implemented");
      // TODO: Send a message to tell the server to wipe its current settings to make room for the new ones (otherwise database entries cannot be updated, nor will they be possible to remove by removing them from the XML)

      // ==[ Doors ]=============================================================
      std::vector<PT::Data::Door*> doors;

      PointerLibrary::getInstance()->getDoorDataManager()->GetAllDoors(doors);

      // Initialize the DoorDataManager, if it isn't already.
      if(doors.size()==0)
      {
        if (!PointerLibrary::getInstance()->getDoorDataManager()->parse())
        {
          Report(PT::Error, "Failed to initialize DoorDataManager!");
          return false;
        }
        PointerLibrary::getInstance()->getDoorDataManager()->GetAllDoors(doors); // Update list
      }

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

        PT::Data::SectorDataManager* secmgr = PointerLibrary::getInstance()->getSectorDataManager();

        // Just send the data here, one door/package. TCP will group it as suitable
        SpawnDoorMessage doormsg;
        doormsg.setDoorId(door_id);
        doormsg.setName(ptString(name, strlen(name)));
        doormsg.setMesh(ptString(mesh, strlen(mesh)));
        doormsg.setSectorId(secmgr->GetSectorByName(sector)->GetId());
        doormsg.setPos(position.x, position.y, position.z);
        doormsg.setAnimation(ptString(quest, strlen(quest)));

        doormsg.setIsOpen(open);
        doormsg.setIsLocked(locked);

        PointerLibrary::getInstance()->getNetwork()->send(&doormsg);
      }

      // ==[ Items ]=============================================================
      std::vector<PT::Data::Item*> items;

      PointerLibrary::getInstance()->getItemDataManager()->GetAllItems(items);

/*      // Initialize the ItemDataManager, if it isn't already.
      if(items.size()==0)
      {
        if (!PointerLibrary::getInstance()->getItemDataManager()->parse())
        {
          Report(PT::Error, "Failed to initialize ItemDataManager!");
          return false;
        }
        PointerLibrary::getInstance()->getItemDataManager()->GetAllItems(items); // Update list
      }*/

      for (size_t i = 0; i < items.size(); i++ )
      {
        unsigned int item_id = items[i]->GetId();
        const char* name = items[i]->GetName().c_str();
        const char* icon = items[i]->GetIconName().c_str();
        const char* description = items[i]->GetDescription().c_str();
        const char* file = items[i]->GetMeshFile().c_str();
        const char* mesh = items[i]->GetMeshName().c_str();
        float weight = items[i]->GetWeight();
        const char* equiptype = items[i]->GetEquiptype().c_str();

        // Just to avoid "unused variable" warnings, until we have a message to send it to the server instead
        printf("Loading item, item_id=%u, name=\"%s\", icon=\"%s\", description=\"%s\", file=\"%s\", mesh=\"%s\", weight=%f, equiptype=\"%s\"\n", item_id, name, icon, description, file, mesh, weight, equiptype);

        // Just send the data here, one item/package
      }

      // ==[ Sectors ]=============================================================
      std::vector<PT::Data::Sector*> sectors;

      PointerLibrary::getInstance()->getSectorDataManager()->GetAllSectors(sectors);

/*      // Initialize the SectorDataManager, if it isn't already.
      if(items.size()==0)
      {
        if (!PointerLibrary::getInstance()->getSectorDataManager()->parse())
        {
          Report(PT::Error, "Failed to initialize SectorDataManager!");
          return false;
        }
        PointerLibrary::getInstance()->getSectorDataManager()->GetAllSectors(sectors); // Update list
      }*/

      for (size_t i = 0; i < sectors.size(); i++ )
      {
        unsigned int sector_id=sectors[i]->GetId();
        const char* name=sectors[i]->GetName().c_str();
        const char* region=sectors[i]->GetRegion().c_str();

        // Just to avoid "unused variable" warnings, until we have a message to send it to the server instead
        printf("Loading sector, sector_id=%u, name=\"%s\", region=\"%s\"\n", sector_id, name, region);

        // Just send the data here, one sector/package
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
