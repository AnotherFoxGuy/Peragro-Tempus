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

#include "file-sectors.h"

#include "server/entity/sectormanager.h"
#include "server/server.h"

#include "ext/tinyxml/tinyxml.h"

void SectorsFile::load()
{
  SectorManager* sector_mgr = Server::getServer()->getSectorManager();

  // If the XML is not consistant, we just segfault!

  TiXmlDocument doc;
  if (!doc.LoadFile("data/xml/world/sectors.xml"))
    return;

  TiXmlElement* Sectors = doc.FirstChildElement("sectors");

  TiXmlElement* itemNode = Sectors->FirstChildElement("sector");
  while (itemNode)
  {
    int id = 0;
    itemNode->Attribute("id", &id);

    const char* name = itemNode->FirstChild()->ToText()->Value();

    sector_mgr->addSector(id, ptString(name, strlen(name)));

    itemNode = itemNode->NextSiblingElement("sector");
  }
}
