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
#include "server/entity/itemmanager.h"
#include "common/network/serialiser.h"
#include "common/network/entitymessages.h"
#include "server/network/connection.h"
#include "server/server.h"

void Inventory::sendAllItems(Connection* conn)
{
  InventoryItemListMessage itemlist_msg;
  itemlist_msg.setItemCount((char)entries.getCount());
  for (size_t i=0; i<entries.getCount(); i++)
  {
    itemlist_msg.setItemId(int(i),entries.get(i)->item_id);
    Item* item = Server::getServer()->getItemManager()->findById(entries.get(i)->item_id);
    assert(item);
    itemlist_msg.setName(int(i),item->getName());
    itemlist_msg.setItemAmount(int(i),entries.get(i)->amount);
    itemlist_msg.setItemSlot(int(i),entries.get(i)->slot);
  }
  ByteStream bs2;
  itemlist_msg.serialise(&bs2);
  conn->send(bs2);
}
