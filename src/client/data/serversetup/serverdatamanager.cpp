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

#include "client/reporter/reporter.h"
#include "client/pointer/pointer.h"
#include "serverdatamanager.h"
#include "ext/tinyxml/tinyxml.h"

namespace PT
{
  namespace Data
  {

    ServerDataManager::ServerDataManager()
    {
      vfs = csQueryRegistry<iVFS> (PointerLibrary::getInstance()->getObjectRegistry());
      if (!vfs) Report(PT::Error, "Failed to locate VFS!");
    }

    ServerDataManager::~ServerDataManager()
    {
    }

    bool ServerDataManager::UploadServerData()
    {
      // Send a message to tell the server to wipe its current settings to make room for the new ones (Just don't know how yet)
      Report(PT::Notify, "Not yet implemented (need to know how to send the messages)");

      // ==[ Doors ]=============================================================
      const char* fname="/peragro/xml/doors/doors.xml";
      csRef<iDataBuffer> xmlfile = vfs->ReadFile (fname);
      if (!xmlfile){return Report(PT::Error, "Can't load file '%s'!", fname);}

      TiXmlDocument doc;
      if (!doc.Parse(xmlfile->GetData()))
        return false;

      TiXmlElement* items = doc.FirstChildElement("doors");
      TiXmlElement* itemNode = items->FirstChildElement("door");
      while (itemNode)
      {/* (uncomment when we know how to send the data, commented since they would cause "unused variable" warnings otherwise)
        unsigned int door_id = atoi(itemNode->FirstChildElement("id")->FirstChild()->ToText()->Value());
        const char* name = itemNode->FirstChildElement("name")->FirstChild()->ToText()->Value();
        const char* mesh = itemNode->FirstChildElement("mesh")->FirstChild()->ToText()->Value();
        const char* sector = itemNode->FirstChildElement("sector")->FirstChild()->ToText()->Value();

        const char* str_x = itemNode->FirstChildElement("position")->Attribute("x");
        const char* str_y = itemNode->FirstChildElement("position")->Attribute("y");
        const char* str_z = itemNode->FirstChildElement("position")->Attribute("z");*/

        // Just send the data here, one door/package (Just don't know how yet)

        itemNode = itemNode->NextSiblingElement("door");
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
