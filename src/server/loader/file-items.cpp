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

#include "file-items.h"

#include "server/entity/itemmanager.h"
#include "server/server.h"

#include "ext/tinyxml/tinyxml.h"

void ItemsFile::load()
{
  ItemManager* item_mgr = Server::getServer()->getItemManager();

  // If the XML is not consistant, we just segfault!

  TiXmlDocument doc;
  if (!doc.LoadFile("data/xml/items/items.xml"))
    return;

  TiXmlElement* items = doc.FirstChildElement("items");

  TiXmlElement* itemNode = items->FirstChildElement("item");
  while (itemNode)
  {
    Item* item = new Item();
    unsigned int id = atoi(itemNode->FirstChildElement("id")
      ->FirstChild()->ToText()->Value());
    const char* name = itemNode->FirstChildElement("name")
      ->FirstChild()->ToText()->Value();
    const char* mesh = itemNode->FirstChildElement("mesh")
      ->FirstChild()->ToText()->Value();
    unsigned int stackmax = atoi(itemNode->FirstChildElement("stackable")
      ->FirstChild()->ToText()->Value());
    unsigned int weight = atoi(itemNode->FirstChildElement("weight")
      ->FirstChild()->ToText()->Value());

    item->setId(id);
    item->setName(ptString(name, strlen(name)));
    item->setMesh(ptString(mesh, strlen(mesh)));
    item->setStackMax(stackmax);
    item->setWeight(weight);

    item_mgr->addItem(item);

    itemNode = itemNode->NextSiblingElement("item");
  }
}
