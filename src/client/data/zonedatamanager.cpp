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

#include "zonedatamanager.h"

namespace PT
{
  namespace Data
  {

    ZoneDataManager::ZoneDataManager(PointerLibrary* ptrlib) : DataManager(ptrlib)
    {
      file = "/peragro/xml/zones/zones.xml";
    }

    ZoneDataManager::~ZoneDataManager()
    {
      for (size_t i = 0; i<zones.size(); i++) delete zones[i];
    }

    ///@internal Currently the zones vector is not preallocated, since there's
    ///no efficient way to get the count of zones described in XML file. If need
    ///be, we could traverse the XML file once before adding the actual data in
    ///order to determine the number of zones in file, and preallocate memory
    ///space.
    bool ZoneDataManager::parseElement(iDocumentNode* node)
    {
      Zone* zone = new Zone();

      zone->SetId(node->GetAttributeValueAsInt("id"));
      zone->SetType(ptString::create(node->GetAttributeValue("type")));
      csRef<iDocumentNodeIterator> nodes(node->GetNodes("node"));

      while (nodes->HasNext())
      {
        csRef<iDocumentNode> zonenode = nodes->Next();
        zone->AddNode(PtVector2(zonenode->GetAttributeValueAsFloat("X"), zonenode->GetAttributeValueAsFloat("Z")));
      }

      zones.push_back(zone);

      return true;
    }
  } // Data namespace
} // PT namespace
