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
      // TODO: Send a message to tell the server to wipe its current settings to make room for the new ones

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

/*      // ==[ Items ]=============================================================
      fname="/peragro/xml/items/items.xml";
      xmlfile = vfs->ReadFile (fname);
      if (!xmlfile){return Report(PT::Error, "Can't load file '%s'!", fname);}

      docsys = csQueryRegistry<iDocumentSystem> (PointerLibrary::getInstance()->getObjectRegistry());
      doc = docsys->CreateDocument();

      error = doc->Parse (xmlfile, true);
      if (error)
      {
        Report(PT::Error, error);
        return false;
      }

      fileXML = doc->GetRoot ()->GetNode ("items");
      if (!fileXML.IsValid()) return false;

      it = fileXML->GetNodes ("item");
      while (it->HasNext())
      {
        csRef<iDocumentNode> itemNode (it->Next());

        unsigned int item_id = itemNode->GetNode("id")->GetContentsValueAsInt();
        const char* name = itemNode->GetNode("name")->GetContentsValue();
        const char* icon = itemNode->GetNode("icon")->GetContentsValue();
        const char* description = itemNode->GetNode("description")->GetContentsValue();
        const char* file = itemNode->GetNode("file")->GetContentsValue();
        const char* mesh = itemNode->GetNode("mesh")->GetContentsValue();
        unsigned int weight = itemNode->GetNode("weight")->GetContentsValueAsInt();
        const char* equiptype = "";
        if(itemNode->GetNode("equiptype")){equiptype = itemNode->GetNode("equiptype")->GetContentsValue();}

        // Just to avoid "unused variable" warnings, until there are better things to do with them:
        printf("Loading item, item_id=%u, name=\"%s\", icon=\"%s\", description=\"%s\", file=\"%s\", mesh=\"%s\", weight=%u, equiptype=\"%s\"\n", item_id, name, icon, description, file, mesh, weight, equiptype);

        // Just send the data here, one door/package (Just don't know how yet)
      }*/

      return true;
    } // end UploadServerData()

    bool ServerSetupManager::DownloadServerData()
    {
      Report(PT::Notify, "Not yet implemented");
      return true;
    } // end DownloadServerData()

  } // Data namespace
} // PT namespace
