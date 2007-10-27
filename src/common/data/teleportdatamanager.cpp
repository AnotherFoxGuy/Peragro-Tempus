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

#include "teleportdatamanager.h"
#include "teleport.h"

#include "ext/tinyxml/tinyxml.h"

namespace PT
{
  namespace Data
  {
    TeleportDataManager::~TeleportDataManager()
    {
      for (size_t i = 0; i<teleports.size(); i++) delete teleports[i];
    }

    ///@internal Currently the teleports vector is not preallocated, since
    ///there's no efficient way to get the count of teleports described in XML
    ///file. If need be, we could traverse the XML file once before adding the
    ///actual data in order to determine the number of teleports in file, and
    ///preallocate memory space.
    bool TeleportDataManager::LoadTeleportData()
    {
      TiXmlDocument doc;
      std::string file;

      file = dataPath + "/xml/teleports/teleports.xml";
      if (!doc.LoadFile(file.c_str())) return false;

      TiXmlElement* teleportsXML = doc.FirstChildElement("teleports");

      if (!teleportsXML) return false;

      TiXmlElement* teleportNode = teleportsXML->FirstChildElement("teleport");
      for (; teleportNode; teleportNode =
        teleportNode->NextSiblingElement("teleport"))
      {
        Teleport* teleport = new Teleport();

        teleport->SetId(atoi(teleportNode->FirstChildElement("id")->FirstChild()
            ->ToText()->Value()));

        teleport->SetName(teleportNode->FirstChildElement("name")->FirstChild()
            ->ToText()->Value());

        teleport->SetDescription(teleportNode->FirstChildElement("description")
            ->ToText()->Value());

        teleport->SetMeshName(teleportNode->FirstChildElement("mesh")->FirstChild()
            ->ToText()->Value());

        PtVector3 pos;
        double temp; //The Attribute() call requires a damn double
        teleportNode->FirstChildElement("position")->Attribute("x", &temp);
        pos.x = temp;
        teleportNode->FirstChildElement("position")->Attribute("y", &temp);
        pos.y = temp;
        teleportNode->FirstChildElement("position")->Attribute("z", &temp);
        pos.z = temp;
        teleport->SetPosition(pos);

        teleport->SetSectorName(teleportNode->FirstChildElement("sector")
            ->FirstChild()->ToText()->Value());

        teleportNode->FirstChildElement("destination")->Attribute("x", &temp);
        pos.x = temp;
        teleportNode->FirstChildElement("destination")->Attribute("y", &temp);
        pos.y = temp;
        teleportNode->FirstChildElement("destination")->Attribute("z", &temp);
        pos.z = temp;
        teleport->SetDestinationPosition(pos);

        teleport->SetDestinationSector(teleportNode->FirstChildElement("sector")
            ->FirstChild()->ToText()->Value());

        teleports.push_back(teleport);
      }

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
