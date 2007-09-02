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

#include "sectormanager.h"

#include <iutil/vfs.h>
#include <iutil/objreg.h>
#include <imap/loader.h>

#include <csutil/xmltiny.h>

#include "client/reporter/reporter.h"


namespace PT
{
  namespace Data
  {
    SectorManager::SectorManager (iObjectRegistry* obj_reg)
    {
      vfs = csQueryRegistry<iVFS> (obj_reg);
      //if (!vfs) return ReportError("Failed to locate VFS!");

      stringset = csQueryRegistryTagInterface<iStringSet> (obj_reg,
        "crystalspace.shared.stringset");

      docsys = csQueryRegistry<iDocumentSystem> (obj_reg);

      errorSector = "ErrorSector";
    }

    SectorManager::~SectorManager ()
    {
      sectors.DeleteAll();
    }

    bool SectorManager::Initialize ()
    {
      // Clear the array.
      sectors.DeleteAll();

      csRef<iFile> buf = vfs->Open("/peragro/xml/world/sectors.xml", VFS_FILE_READ);

      if (!buf)
        return Report(PT::Error, "SectorManager: Couldn't open sector file!");

      csRef<iDocument> doc;
      csRef<iDocumentNode> node;

      if (!docsys) docsys = csPtr<iDocumentSystem> (new csTinyDocumentSystem ());
      doc = docsys->CreateDocument ();
      const char* error = doc->Parse (buf, true);
      if (error != 0)
        return Report(PT::Error, "SectorManager: Couldn't open sector file!");

      if (doc)
      {
        node = doc->GetRoot()->GetNode("sectors");
        if (!node)
          return Report(PT::Error, "SectorManager: Couldn't open sector file!");
      }

      Report(PT::Debug, "==Loading sectors==========================");
      csRef<iDocumentNodeIterator> it = node->GetNodes ();
      while (it->HasNext ())
      {
        csRef<iDocumentNode> child = it->Next ();
        int id = child->GetAttributeValueAsInt("id");
        std::string name ( child->GetContentsValue() );

        sectors.Put(id, name);

        Report(PT::Debug, "%d : %s", id, name.c_str());
      }
      Report(PT::Debug, "================================= %d sector(s)\n", sectors.GetSize()-1);

      return true;
    }

    const std::string& SectorManager::GetSectorName(unsigned int id) const
    {
      if (id < sectors.GetSize())
      {
        return sectors[id];
      }
      Report(PT::Error, "SectorMGR: Couldn't find sector %d!", id);
      return errorSector;
    }

    unsigned int SectorManager::GetSectorId(const char* name) const
    {
      for (size_t i = 0; i < sectors.GetSize(); i++)
      {
        std::string sector = sectors[i];
        if (sector.compare(name) == 0)
          return i;
      }
      Report(PT::Error, "SectorMGR: Couldn't find sector %s!", name);
      return 0;
    }

  } // Data namespace 
} // PT namespace 
