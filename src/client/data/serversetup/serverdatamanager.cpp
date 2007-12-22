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
#include "serverdatamanager.h"

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

      csRef<iDocumentSystem> docsys (csQueryRegistry<iDocumentSystem> (PointerLibrary::getInstance()->getObjectRegistry()));
      csRef<iDocument> doc (docsys->CreateDocument());

      const char* error = doc->Parse (xmlfile, true);
      if (error)
      {
        Report(PT::Error, error);
        return false;
      }


/*      TiXmlDocument doc;
      if (!doc.Parse(xmlfile->GetData()))
        return false;

      TiXmlElement* items = doc.FirstChildElement("doors");
      TiXmlElement* itemNode = items->FirstChildElement("door");
      while (itemNode)*/

      csRef<iDocumentNode> doorsXML = doc->GetRoot ()->GetNode ("doors");
      if (!doorsXML.IsValid()) return false;

      csRef<iDocumentNodeIterator> it (doorsXML->GetNodes ("door"));
      while (it->HasNext())
      {
        csRef<iDocumentNode> doorNode (it->Next());
        /* (uncomment when we know how to send the data, commented since they would cause "unused variable" warnings otherwise)
        unsigned int door_id = doorNode->GetNode("id")->GetContentsValueAsInt();
        const char* name = doorNode->GetNode("name")->GetContentsValue();
        const char* mesh = doorNode->GetNode("mesh")->GetContentsValue();
        const char* sector = doorNode->GetNode("sector")->GetContentsValue();

        const char* str_x = doorNode->GetNode("position")->GetAttributeValue("x");
        const char* str_y = doorNode->GetNode("position")->GetAttributeValue("y");
        const char* str_z = doorNode->GetNode("position")->GetAttributeValue("z");*/

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
