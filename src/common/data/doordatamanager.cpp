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

#include "doordatamanager.h"
#include "door.h"

#include "ext/tinyxml/tinyxml.h"

namespace PT
{
  namespace Data
  {

    DoorDataManager::~DoorDataManager()
    {
      for (size_t i = 0; i<doors.size(); i++) delete doors[i];
    }

    ///@internal Currently the doors vector is not preallocated, since there's
    ///no efficient way to get the count of doors described in XML file. If need
    ///be, we could traverse the XML file once before adding the actual data in
    ///order to determine the number of doors in file, and preallocate memory
    ///space.
    bool DoorDataManager::LoadDoorData()
    {
      TiXmlDocument doc;
      std::string file;

      file = dataPath+"/xml/doors/doors.xml";
      if (!doc.LoadFile(file.c_str())) return false;

      TiXmlElement* doorsXML = doc.FirstChildElement("doors");

      if (!doorsXML) return false;

      TiXmlElement* doorNode = doorsXML->FirstChildElement("door");
      for (; doorNode; doorNode = doorNode->NextSiblingElement("door"))
      {
        Door* door = new Door();

        door->SetId(atoi(doorNode->FirstChildElement("id") ->FirstChild()
            ->ToText()->Value()));

        door->SetName(doorNode->FirstChildElement("name")->FirstChild()
            ->ToText()->Value());

        door->SetMeshName(doorNode->FirstChildElement("mesh")->FirstChild()
            ->ToText()->Value());

        PtVector3 pos;
        double temp; //The Attribute() call requires a damn double
        doorNode->FirstChildElement("position")->Attribute("x", &temp);
        pos.x = temp;
        doorNode->FirstChildElement("position")->Attribute("y", &temp);
        pos.x = temp;
        doorNode->FirstChildElement("position")->Attribute("z", &temp);
        pos.x = temp;
        door->SetPosition(pos);

        door->SetSectorName(doorNode->FirstChildElement("sector")->FirstChild()
            ->ToText()->Value());

        door->SetKeyId(atoi(doorNode->FirstChildElement("keyId") ->FirstChild()
            ->ToText()->Value()));

        door->SetQuestName(doorNode->FirstChildElement("quest")->FirstChild()
            ->ToText()->Value());

        doors.push_back(door);
      }

      return true;
    }

    Door* DoorDataManager::GetDoorById(unsigned int id) const
    {
      for (size_t i = 0; i < doors.size(); i++ )
        if (doors[i]->GetId() == id) return doors[i];

      return 0;
    }

    Door* DoorDataManager::GetDoorByName(const std::string& name) const
    {
      for (size_t i = 0; i < doors.size(); i++ )
        if (doors[i]->GetName() == name) return doors[i];

      return 0;
    }
  }
}
