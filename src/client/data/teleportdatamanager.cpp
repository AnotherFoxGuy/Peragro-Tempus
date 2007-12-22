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

#include "teleportdatamanager.h"
#include "teleport.h"

namespace PT
{
  namespace Data
  {

    TeleportDataManager::TeleportDataManager()
    {
      file = "/peragro/xml/teleports/teleports.xml";
    }

    TeleportDataManager::~TeleportDataManager()
    {
      for (size_t i = 0; i<teleports.size(); i++) delete teleports[i];
    }

    ///@internal Currently the teleports vector is not preallocated, since
    ///there's no efficient way to get the count of teleports described in XML
    ///file. If need be, we could traverse the XML file once before adding the
    ///actual data in order to determine the number of teleports in file, and
    ///preallocate memory space.
    bool TeleportDataManager::parseElement(iDocumentNode* node)
    {
      Teleport* teleport = new Teleport();

      teleport->SetId(node->GetNode("id")->GetContentsValueAsInt());

      teleport->SetName(node->GetNode("name")->GetContentsValue());

      teleport->SetDescription(node->GetNode("description")->GetContentsValue());

      teleport->SetMeshName(node->GetNode("mesh")->GetContentsValue());

      PtVector3 pos;
      pos.x = node->GetNode("position")->GetAttributeValueAsFloat("x");
      pos.y = node->GetNode("position")->GetAttributeValueAsFloat("y");
      pos.z = node->GetNode("position")->GetAttributeValueAsFloat("z");
      teleport->SetPosition(pos);

      teleport->SetSectorName(node->GetNode("sector")->GetContentsValue());

      PtVector3 dest;
      dest.x = node->GetNode("destination")->GetAttributeValueAsFloat("x");
      dest.y = node->GetNode("destination")->GetAttributeValueAsFloat("y");
      dest.z = node->GetNode("destination")->GetAttributeValueAsFloat("z");
      teleport->SetDestinationPosition(dest);

      teleport->SetDestinationSector(node->GetNode("sector")->GetContentsValue());

      teleports.push_back(teleport);

      return true;
    }

    Teleport* TeleportDataManager::GetTeleportById(unsigned int id) const
    {
      for (size_t i = 0; i < teleports.size(); i++ )
        if (teleports[i]->GetId() == id) return teleports[i];

      return 0;
    }

    Teleport* TeleportDataManager::GetTeleportByName(const std::string& name) const
    {
      for (size_t i = 0; i < teleports.size(); i++ )
        if (teleports[i]->GetName() == name) return teleports[i];

      return 0;
    }
  }
}
