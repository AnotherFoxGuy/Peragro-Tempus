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

#include "common/util/monitorable.h"

class Item;
class Entity;

class ItemEntity : public ptMonitorable<ItemEntity>
{
private:
  ptMonitor<Item> item;
  ptMonitor<Entity> entity;

public:
  ItemEntity()
  {
    entity = (new Entity(Common::Entity::ItemEntityType))->getRef();

    ptScopedMonitorable<Entity> e (entity.get());
    e->setItemEntity(this);
  }

  ~ItemEntity() {}

  const Item* getItem() const { return item.get(); }
  const Entity* getEntity() const { return entity.get(); }

  unsigned int variation;

  void createFromItem(unsigned int item_id, unsigned int variation);
  //void createFromItem(Item* item, unsigned int variation);
};

#endif // ITEMENTITY_H
