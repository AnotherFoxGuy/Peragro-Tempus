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

#ifndef INVENTORY_H_
#define INVENTORY_H_

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include "common/inventory/gridinventory.h"

class Connection;
class Entity;
class ItemEntity;

namespace CI = Common::Inventory;

namespace Common
{
  namespace Inventory
  {
    struct PositionRef;
    class Object;
  } // end Inventory
} // end Common

class Inventory : public CI::GridInventory
{
private:
  boost::weak_ptr<Entity> entity;

private:
  //Only allow adding ItemEntitys.
  virtual bool AddObjectAt(const CI::PositionRef& position, boost::shared_ptr<CI::Object> object)
  { return false; }

private:
  void SaveItemToDB(const CI::PositionRef& position, boost::shared_ptr<ItemEntity> item);
  void DeleteItemFromDB(const CI::PositionRef& position);

public:
  Inventory(boost::shared_ptr<Entity> entity) : CI::GridInventory("Inventory", ALLOW_ITEMS, 5, 4), entity(entity) {}
  ~Inventory() {}

  bool AddObjectAt(const CI::PositionRef&, boost::shared_ptr<ItemEntity>);

  boost::shared_ptr<CI::Object> RemoveObjectAt(const CI::PositionRef& position);
  CI::PositionRef RemoveObject(boost::shared_ptr<CI::Object> object);

  void LoadFromDB();
  void SaveToDB();
  void SendAllItems(Connection* conn);
};

#endif // INVENTORY_H_
