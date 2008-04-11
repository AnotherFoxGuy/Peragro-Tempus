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

#include "sectordatamanager.h"
#include "sector.h"

namespace PT
{
  namespace Data
  {

    SectorDataManager::SectorDataManager(PointerLibrary* ptrlib) : DataManager(ptrlib)
    {
      file = "/peragro/xml/world/sectors.xml";
    }

    SectorDataManager::~SectorDataManager()
    {
      for (size_t i = 0; i < sectors.size(); i++) delete sectors[i];
    }

    ///@internal Currently the sectors vector is not preallocated, since there's
    ///no efficient way to get the count of items described in XML file. If need
    ///be, we could traverse the XML file once before adding the actual data in
    ///order to determine the number of sectors in file, and preallocate memory
    ///space.
    bool SectorDataManager::parseElement(iDocumentNode* node)
    {
      Sector* sector = new Sector();

      sector->SetId(node->GetAttributeValueAsInt("id"));

      sector->SetRegion(node->GetAttributeValue("region"));

      sector->SetName(node->GetContentsValue());

      sectors.push_back(sector);

      return true;
    }

    Sector* SectorDataManager::GetSectorById(unsigned int id) const
    {
      for (size_t i = 0; i < sectors.size(); i++)
        if (sectors[i]->GetId() == id) return sectors[i];

      return 0;
    }

    Sector* SectorDataManager::GetSectorByName(const std::string& name) const
    {
      for (size_t i = 0; i < sectors.size(); i++)
        if (sectors[i]->GetName() == name) return sectors[i];

      return 0;
    }
  } // Data namespace
} // PT namespace
