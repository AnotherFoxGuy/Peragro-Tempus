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

#include "doordatamanager.h"
#include "door.h"

#include "plugins/tiledworld/maptile.h"

namespace PT
{
  namespace Data
  {

    DoorDataManager::DoorDataManager(PointerLibrary* ptrlib) : DataManager(ptrlib)
    {
      file = "/peragro/xml/doors/doors.xml";
    }

    DoorDataManager::~DoorDataManager()
    {
      for (size_t i = 0; i<doors.size(); i++) delete doors[i];
    }

    ///@internal Currently the doors vector is not preallocated, since there's
    ///no efficient way to get the count of doors described in XML file. If need
    ///be, we could traverse the XML file once before adding the actual data in
    ///order to determine the number of doors in file, and preallocate memory
    ///space.
    bool DoorDataManager::parseElement(iDocumentNode* node)
    {
      Door* door = new Door();

      door->SetId(node->GetNode("id")->GetContentsValueAsInt());

      door->SetName(node->GetNode("name")->GetContentsValue());

      door->SetMeshName(node->GetNode("mesh")->GetContentsValue());

      int offsetX, offsetY;
      offsetX = node->GetNode("position")->GetAttributeValueAsInt("tileX") * (int)TILESIZE;
      offsetY = node->GetNode("position")->GetAttributeValueAsInt("tileY") * (int)TILESIZE;

      WFMath::Point<3> pos;
      pos[0] = node->GetNode("position")->GetAttributeValueAsFloat("x") + offsetX;
      pos[1] = node->GetNode("position")->GetAttributeValueAsFloat("y");
      pos[2] = node->GetNode("position")->GetAttributeValueAsFloat("z") + offsetY;
      door->SetPosition(pos);

      door->SetSectorName(node->GetNode("position")->GetAttributeValue("sector"));

      door->SetKeyId(node->GetNode("keyId") ->GetContentsValueAsInt());

      door->SetOpenState(node->GetNode("default")->GetAttributeValueAsBool("open"));

      door->SetLockState(node->GetNode("default")->GetAttributeValueAsBool("locked"));

      door->SetQuestName(node->GetNode("quest")->GetContentsValue());

      doors.push_back(door);

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
  } // Data namespace
} // PT namespace
