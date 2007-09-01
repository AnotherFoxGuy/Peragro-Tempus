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

#include "client/sector/sectormanager.h"

#include <iutil/vfs.h>
#include <iutil/objreg.h>
#include <imap/loader.h>

#include <csutil/xmltiny.h>

#include "client/reporter/reporter.h"



SectorMGR::SectorMGR (iObjectRegistry* obj_reg)
{
  engine = csQueryRegistry<iEngine> (obj_reg);
  //if (!engine) return ReportError("Failed to locate 3D engine!");

  vfs = csQueryRegistry<iVFS> (obj_reg);
  //if (!vfs) return ReportError("Failed to locate VFS!");

  stringset = csQueryRegistryTagInterface<iStringSet> (obj_reg,
    "crystalspace.shared.stringset");

  docsys = csQueryRegistry<iDocumentSystem> (obj_reg);
}

SectorMGR::~SectorMGR ()
{
  sectors.DeleteAll();
}

bool SectorMGR::Initialize ()
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
    csString name ( child->GetContentsValue() );

    sectors.Put(id, name);

    Report(PT::Debug, "%d : %s", id, name.GetData());
  }
  Report(PT::Debug, "================================= %d sector(s)\n", sectors.GetSize()-1);

  return true;
}

const csString* SectorMGR::GetSectorName(uint id) const
{
  return sectors.GetElementPointer(id);
}

uint SectorMGR::GetSectorId(const char* name) const
{
  csHash<csString, uint>::ConstGlobalIterator it = sectors.GetIterator();

  while( it.HasNext() ) {
    const csTuple2<csString, uint> tuple = it.NextTuple();
    if (tuple.first.Compare(name)) {
      return tuple.second;
    }
  }

  Report(PT::Error, "SectorMGR: Couldn't find sector %s!", name);
  return 0;
}
