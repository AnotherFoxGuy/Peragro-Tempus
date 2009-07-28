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

#ifndef ITEMENTITY_H
#define ITEMENTITY_H

#include "entity.h"

#include "itemtemplatesmanager.h"

#include "statistic/abilities.h"
#include "statistic/vulnerabilities.h"
#include "statistic/attributes.h"

#include "common/inventory/item.h"

#include "../server.h"

#include "common/util/exception.h"

PT_DEFINE_EXCEPTION(InvalidItemEntity);

class ItemEntity : public Entity, public Common::Inventory::Item
{
private:
  bool inWorld;
  boost::shared_ptr<ItemTemplate> itemTemplate;

  boost::shared_ptr<Abilities> abilities;
  boost::shared_ptr<Vulnerabilities> vulnerabilities;
  boost::shared_ptr<Attributes> attributes;

public:
  ItemEntity() : Entity(Common::Entity::ItemEntityType)
  {
    inWorld = false;
    abilities = Server::getServer()->GetAbilitiesFactory()->Create(this);
    vulnerabilities = Server::getServer()->GetVulnerabilitiesFactory()->Create(this);
    attributes = Server::getServer()->GetAttributesFactory()->Create(this);
  }

  ~ItemEntity() {}

  bool GetInWorld() { return inWorld; }
  void SetInWorld(bool value);

  std::string GetEquipType() {assert(itemTemplate); return itemTemplate->equipTypeName; }  /// @TODO needs changing to a id value

  boost::shared_ptr<ItemTemplate> GetItemTemplate() { return itemTemplate; }
  void SetItemTemplate(boost::shared_ptr<ItemTemplate> value) { itemTemplate = value; value->SetDataOn(this); }

  boost::shared_ptr<Abilities> GetAbilities() const { return abilities; }
  boost::shared_ptr<Vulnerabilities> GetVulnerabilities() const { return vulnerabilities; }
  boost::shared_ptr<Attributes> GetAttributes() const { return attributes; }

  virtual void LoadFromDB();
  virtual void SaveToDB();
  virtual void DeleteFromDB();
};

#endif // ITEMENTITY_H
