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

      // ==[ Sectors ]=============================================================
      std::vector<PT::Data::Sector*> sectors;

      PointerLibrary::getInstance()->getSectorDataManager()->GetAllSectors(sectors);

      for (size_t i = 0; i < sectors.size(); i++ )
      {
        unsigned int sector_id=sectors[i]->GetId();
        ptString name = ptString::create(sectors[i]->GetName());
        ptString region = ptString::create(sectors[i]->GetRegion());

        // Just to avoid "unused variable" warnings, until we have a message to send it to the server instead
        Report(PT::Debug, "Loading sector, sector_id=%u, name=\"%s\", region=\"%s\"\n", sector_id, name, region);

        // Just send the data here, one sector/package
        CreateSectorMessage sectormsg;
        sectormsg.setSectorId(sector_id);
        sectormsg.setName(name);
        sectormsg.setRegion(region);

        PointerLibrary::getInstance()->getNetwork()->send(&sectormsg);
      }

      // ==[ Doors ]=============================================================
      std::vector<PT::Data::Door*> doors;

      PointerLibrary::getInstance()->getDoorDataManager()->GetAllDoors(doors);

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

        unsigned int sector_id = secmgr->GetSectorByName(sector)->GetId();

        Report(PT::Debug, "Loading doors, id=%u, name=\"%s\", sector=\"%s\"\n", door_id, name, sector);

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

      PointerLibrary::getInstance()->getItemDataManager()->GetAllItems(items);

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

        // Just to avoid "unused variable" warnings, until we have a message to send it to the server instead
        Report(PT::Debug, "Loading item, item_id=%u, name=\"%s\", icon=\"%s\", description=\"%s\", file=\"%s\", mesh=\"%s\", weight=%f, equiptype=\"%s\"\n", item_id, name, icon, description, file, mesh, weight, equiptype);

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

      return true;
    } // end UploadServerData()

    bool ServerSetupManager::DownloadServerData()
    {
      Report(PT::Notify, "Not yet implemented");
      return true;
    } // end DownloadServerData()

  } // Data namespace
} // PT namespace
