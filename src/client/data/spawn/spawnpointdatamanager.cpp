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

#include "spawnpointdatamanager.h"
#include "spawnpoint.h"

namespace PT
{
  namespace Data
  {

    SpawnPointDataManager::SpawnPointDataManager(PointerLibrary* ptrlib) : DataManager(ptrlib)
    {
      file = "/peragro/xml/spawnpoints/spawnpoints.xml";
    }

    SpawnPointDataManager::~SpawnPointDataManager()
    {
      for (size_t i = 0; i<spawnpoints.size(); i++) delete spawnpoints[i];
    }

    ///@internal Currently the spawnpoints vector is not preallocated, since there's
    ///no efficient way to get the count of spawnpoints described in XML file. If need
    ///be, we could traverse the XML file once before adding the actual data in
    ///order to determine the number of spawnpoints in file, and preallocate memory
    ///space.
    bool SpawnPointDataManager::parseElement(iDocumentNode* node)
    {
      SpawnPoint* spawnpoint = new SpawnPoint();

      spawnpoint->SetItem(node->GetNode("item")->GetAttributeValueAsInt("id"));

      spawnpoint->SetVariation(node->GetNode("item")->GetAttributeValueAsInt("variation"));

      WFMath::Point<3> pos;
      pos[0] = node->GetNode("position")->GetAttributeValueAsFloat("x");
      pos[1] = node->GetNode("position")->GetAttributeValueAsFloat("y");
      pos[2] = node->GetNode("position")->GetAttributeValueAsFloat("z");
      spawnpoint->SetPosition(pos);

      spawnpoint->SetSectorName(node->GetNode("position")->GetAttributeValue("sector"));

      spawnpoint->SetInterval(node->GetNode("interval") ->GetContentsValueAsInt());

      spawnpoints.push_back(spawnpoint);

      return true;
    }
  } // Data namespace
} // PT namespace
