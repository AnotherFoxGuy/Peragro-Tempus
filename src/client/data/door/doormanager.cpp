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

#include "doormanager.h"

#include <iutil/vfs.h>
#include <iutil/objreg.h>
#include <imap/loader.h>

#include <csutil/xmltiny.h>

#include "client/reporter/reporter.h"

namespace PT
{
  namespace Data
  {
    DoorManager::DoorManager (iObjectRegistry* obj_reg)
    {
      engine = csQueryRegistry<iEngine> (obj_reg);
      if (!engine) Report(PT::Error, "Failed to locate 3D engine!");

      vfs = csQueryRegistry<iVFS> (obj_reg);
      if (!vfs) Report(PT::Error, "Failed to locate VFS!");

      docsys = csQueryRegistry<iDocumentSystem> (obj_reg);
    }

    DoorManager::~DoorManager ()
    {
      doors.DeleteAll();
    }

    bool DoorManager::Initialize ()
    {
      // Clear the array.
      doors.DeleteAll();

      csRef<iFile> buf = vfs->Open("/peragro/xml/doors/doors.xml", VFS_FILE_READ);

      if (!buf)
        return Report(PT::Error, "DoorManager: Couldn't open item file!");

      csRef<iDocument> doc;
      csRef<iDocumentNode> node;

      if (!docsys) docsys = csPtr<iDocumentSystem> (new csTinyDocumentSystem ());
      doc = docsys->CreateDocument ();
      const char* error = doc->Parse (buf, true);
      if (error != 0)
        return Report(PT::Error, "DoorManager: Couldn't open door file!");

      if (doc)
      {
        node = doc->GetRoot()->GetNode("doors");
        if (!node)
          return Report(PT::Error, "DoorManager: Couldn't open door file!");
      }

      Report(PT::Debug, "==Loading doors==========================");
      csRef<iDocumentNodeIterator> it = node->GetNodes ();
      while (it->HasNext ())
      {
        csRef<iDocumentNode> child = it->Next ();
        csRef<iDocumentNode> id          = child->GetNode ("id");
        csRef<iDocumentNode> name        = child->GetNode ("name");
        csRef<iDocumentNode> mesh        = child->GetNode ("mesh");
        csRef<iDocumentNode> pos         = child->GetNode ("position");
        csRef<iDocumentNode> sector      = child->GetNode ("sector");
        csRef<iDocumentNode> keyId       = child->GetNode ("keyId");
        csRef<iDocumentNode> quest       = child->GetNode ("quest");


        if (!id || !name || !mesh || !pos || !sector || !keyId)
        {
          csString error;
          csString good;
          if(!id) error = "<id>"; else good = id->GetContentsValue ();
          if(!name) error += "<name>"; else good = name->GetContentsValue ();
          if(!mesh) error += "<mesh>";
          if(!pos) error += "<position x="" y="" z=""/";
          if(!sector) error += "<sector>";
          if(!keyId) error += "<keyId>";
          if(!quest) error += "<quest>";

          Report(PT::Error, "DoorManager: Missing %s token(s) for %s!", error.GetData(), good.GetData());
          continue;
        }

        Door* door = new Door();

        door->SetId(id->GetContentsValueAsInt());
        door->SetName((csString)name->GetContentsValue ());
        door->SetMeshName((csString)mesh->GetContentsValue ());
        door->SetPosition(csVector3(pos->GetAttributeValueAsFloat("x"),
                                    pos->GetAttributeValueAsFloat("y"),
                                    pos->GetAttributeValueAsFloat("z")));
        door->SetSectorName((csString)sector->GetContentsValue ());
        door->SetKeyId(keyId->GetContentsValueAsInt());
        door->SetQuest((csString)quest->GetContentsValue());

        if (door->GetId() >= doors.Capacity())
          doors.SetCapacity(door->GetId()+1);

        doors.Put(door->GetId(), door);
        Report(PT::Debug, "%d : %s", door->GetId(), door->GetName().GetData());
      }
      Report(PT::Debug, "================================= %d door(s)\n", doors.GetSize()-1);

      return true;
    }

    Door* DoorManager::GetDoorById(unsigned int id)
    {
      if (id < doors.GetSize())
      {
        return doors[id];
      }
      Report(PT::Error, "DoorManager: Couldn't find door %d!", id);
      return 0;
    }

    Door* DoorManager::GetDoorByName(csString name)
    {
      for (size_t i = 0; i < doors.GetSize(); i++)
      {
        Door* door = doors[i];
        if(door)
          if(door->GetName().Compare(name))
            return door;
      }
      Report(PT::Error, "DoorManager: Couldn't find door %s!", name.GetData());
      return 0;
    }

  } // Data namespace
} // PT namespace
