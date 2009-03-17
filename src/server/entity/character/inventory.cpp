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

#include "inventory.h"
#include "common/network/serialiser.h"
#include "common/network/playermessages.h"
#include "server/network/connection.h"
#include "server/server.h"

#include "common/inventory/object.h"
#include "common/inventory/positionref.h"

#include "server/entity/entitymanager.h"
#include "server/entity/itementity.h"

#include "server/database/tablemanager.h"
#include "server/database/table-inventory.h"

namespace CI = Common::Inventory;

boost::shared_ptr<ItemEntity> Convert(boost::shared_ptr<CI::Object> o)
{
  return boost::shared_dynamic_cast<ItemEntity>(o);
}

boost::shared_ptr<ItemEntity> Convert(Entityp e)
{
  return boost::shared_dynamic_cast<ItemEntity>(e);
}

bool Inventory::AddObjectAt(const CI::PositionRef& position, boost::shared_ptr<ItemEntity> item)
{
  bool success = CI::SlotInventory::AddObjectAt(position, item);
  if (success)
  {
    // We added the item to the inventory, so remove it from the world.
    item->SetInWorld(false);
    item->SaveToDB();
    Server::getServer()->getEntityManager()->Remove(item);
    SaveItemToDB(position, item);
  }
  return success;
}

bool Inventory::AddObjectAt(unsigned int id, boost::shared_ptr<ItemEntity> item)
{
  return AddObjectAt(IdToPos(id), item);
}

boost::shared_ptr<CI::Object> Inventory::RemoveObjectAt(const CI::PositionRef& position)
{
  boost::shared_ptr<CI::Object> object = CI::SlotInventory::RemoveObjectAt(position);
  if (object)
  {
    DeleteItemFromDB(position, Convert(object));
  }
  return object;
}

boost::shared_ptr<CI::Object> Inventory::RemoveObjectAt(unsigned int id)
{
  return RemoveObjectAt(IdToPos(id));
}

boost::shared_ptr<CI::Slot> Inventory::RemoveObject(boost::shared_ptr<CI::Object> object)
{
  boost::shared_ptr<CI::Slot> slot = CI::SlotInventory::RemoveObject(object);
  if (slot)
  {
    DeleteItemFromDB(slot->GetPosition(), Convert(object));
  }
  return slot;
}

void Inventory::LoadFromDB()
{
  InventoryTable* table = Server::getServer()->GetTableManager()->Get<InventoryTable>();
  InventoryTableVOArray items = table->Get(entity.lock()->GetId());
  InventoryTableVOArray::iterator it = items.begin();
  for ( ; it != items.end(); it++)
  {
    Entityp entity = Server::getServer()->getEntityManager()->CreateNew(Common::Entity::ItemEntityType, (*it)->item_id);
    AddObjectAt(IdToPos((*it)->slot), Convert(entity));
  }
}

void Inventory::SaveItemToDB(const CI::PositionRef& position, boost::shared_ptr<ItemEntity> item)
{
  InventoryTable* table = Server::getServer()->GetTableManager()->Get<InventoryTable>();
  table->Insert(entity.lock()->GetId(), PosToId(position), item->GetId());
}

void Inventory::DeleteItemFromDB(const CI::PositionRef& position, boost::shared_ptr<ItemEntity> item)
{
  InventoryTable* table = Server::getServer()->GetTableManager()->Get<InventoryTable>();
  table->Delete(entity.lock()->GetId(), PosToId(position), item->GetId());
}

void Inventory::SaveToDB()
{
  //Already taken care of by AddObjectAt, RemoveObjectAt and RemoveObject.
}

void Inventory::SendAllItems(Connection* conn)
{
  InventoryListMessage itemlist_msg;
  itemlist_msg.setInventoryCount((char)GetObjectCount());
  itemlist_msg.setInventoryId(0); //TODO unused

  std::list<boost::shared_ptr<CI::Slot> >::const_iterator it;
  size_t n = 0;
  for (it=slots.begin(); it != slots.end(); it++)
  {
    if (!(*it)->HasContents()) continue;

    boost::shared_ptr<ItemEntity> item( Convert((*it)->GetContents()) );
    itemlist_msg.setItemId(n, item->GetId());
    
    itemlist_msg.setName(n, item->GetName());
    itemlist_msg.setIconName(n, item->GetIcon());
    itemlist_msg.setDescription(n, item->GetDescription());
    itemlist_msg.setWeight(n, item->GetWeight());
    //itemlist_msg.setEquipType(n, item->GetEquipType()); //TODO

    itemlist_msg.setSlotId(n, (unsigned char) PosToId( (*it)->GetPosition() ));

    n++;
  }

  ByteStream bs2;
  itemlist_msg.serialise(&bs2);
  conn->send(bs2);
}
