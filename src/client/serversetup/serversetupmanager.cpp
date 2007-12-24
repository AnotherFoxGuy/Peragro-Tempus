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

#include <cssysdef.h>
#include <iutil/document.h>

#include "client/reporter/reporter.h"
#include "client/pointer/pointer.h"

// Comment this include out to build on Linux (currently a GCC specific problem)
#include "client/network/network.h"

#include "client/data/sector.h"
#include "client/data/sectordatamanager.h"

#include "client/data/doordatamanager.h"
#include "serversetupmanager.h"

namespace PT
{
  namespace Misc
  {

    ServerDataManager::ServerDataManager()
    {
      // Create and Initialize the DoorDataManager.
      doorDataManager = new PT::Data::DoorDataManager ();
      if (!doorDataManager->parse())
        return Report(PT::Error, "Failed to initialize DoorDataManager!");
      pointerlib.setDoorDataManager(doorDataManager);

//      vfs = csQueryRegistry<iVFS> (PointerLibrary::getInstance()->getObjectRegistry());
//      if (!vfs) Report(PT::Error, "Failed to locate VFS!");
    }

    ServerDataManager::~ServerDataManager()
    {
    }

    bool ServerDataManager::UploadServerData()
    {
      // Send a message to tell the server to wipe its current settings to make room for the new ones (Just don't know how yet)
      Report(PT::Notify, "Not yet implemented (need to fix issue with inclusion of network.h)");

      // ==[ Doors ]=============================================================

      //Maybe use this instead?
      //DoorDataManager* doors = PointerLibrary::getInstance()->getSectorDataManager();

      const char* fname="/peragro/xml/doors/doors.xml";
      csRef<iDataBuffer> xmlfile = vfs->ReadFile (fname);
      if (!xmlfile){return Report(PT::Error, "Can't load file '%s'!", fname);}

      csRef<iDocumentSystem> docsys (csQueryRegistry<iDocumentSystem> (PointerLibrary::getInstance()->getObjectRegistry()));
      csRef<iDocument> doc (docsys->CreateDocument());

      const char* error = doc->Parse (xmlfile, true);
      if (error)
      {
        Report(PT::Error, error);
        return false;
      }

      csRef<iDocumentNode> fileXML = doc->GetRoot ()->GetNode ("doors");
      if (!fileXML.IsValid()) return false;

      csRef<iDocumentNodeIterator> it (fileXML->GetNodes ("door"));
      while (it->HasNext())
      {
        csRef<iDocumentNode> doorNode (it->Next());

        unsigned int door_id = doorNode->GetNode("id")->GetContentsValueAsInt();
        const char* name = doorNode->GetNode("name")->GetContentsValue();
        const char* mesh = doorNode->GetNode("mesh")->GetContentsValue();
        const char* sector = doorNode->GetNode("sector")->GetContentsValue();

        float x = doorNode->GetNode("position")->GetAttributeValueAsFloat("x");
        float y = doorNode->GetNode("position")->GetAttributeValueAsFloat("y");
        float z = doorNode->GetNode("position")->GetAttributeValueAsFloat("z");

/* (For Linux)
        // Just to avoid "unused variable" warnings, until there are better things to do with them:
        printf("Loading door, door_id=%u, name=\"%s\", mesh=\"%s\", sector=\"%s\", x=%f, y=%f, z=%f\n", door_id, name, mesh, sector, x, y, z);

        // Just send the data here, one door/package (Just don't know how yet)*/

// Comment this out (down to the send call) to build on Linux (currently a GCC specific problem)
        const char* quest = doorNode->GetNode("quest")->GetContentsValue();

        bool open = doorNode->GetNode("default")->GetAttributeValueAsBool("open");
        bool locked = doorNode->GetNode("default")->GetAttributeValueAsBool("locked");

        SectorDataManager* secmgr = PointerLibrary::getInstance()->getSectorDataManager();

        // Just send the data here, one door/package. TCP will group it as suitable
        SpawnDoorMessage doormsg;
        doormsg.setDoorId(door_id);
        doormsg.setName(ptString(name, strlen(name)));
        doormsg.setMesh(ptString(mesh, strlen(mesh)));
        doormsg.setSectorId(secmgr->GetSectorByName(sector)->GetId());
        doormsg.setPos(x, y, z);
        doormsg.setAnimation(ptString(quest, strlen(quest)));

        doormsg.setIsOpen(open);
        doormsg.setIsLocked(locked);

        PointerLibrary::getInstance()->getNetwork()->send(&doormsg);
      }

      // ==[ Items ]=============================================================
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
      }

      return true;
    } // end UploadServerData()

    bool ServerDataManager::DownloadServerData()
    {
      Report(PT::Notify, "Not yet implemented");
      return true;
    } // end DownloadServerData()

  } // Data namespace
} // PT namespace
