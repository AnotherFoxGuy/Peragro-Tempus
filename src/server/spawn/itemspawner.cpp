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

#include "itemspawner.h"

#include "server/entity/entitymanager.h"
#include "server/entity/itementity.h"
#include "server/entity/itemtemplatesmanager.h"

#include "common/util/thread.h"
#include "common/util/sleep.h"
#include "common/util/timer.h"
#include "common/util/printhelper.h"
#include "server/server.h"

#include "server/database/tablemanager.h"
#include "server/database/table-spawnpoints.h"

ItemSpawner::ItemSpawner() : timeCounter(0)
{
  this->setInterval(10);
}

ItemSpawner::~ItemSpawner()
{
  LockType lock(mutex);
  std::vector<boost::shared_ptr<SpawnPoint> >::const_iterator it;
  for (it = spawnpoints.begin(); it != spawnpoints.end(); it++)
  {
    boost::shared_ptr<ItemEntity> i = (*it)->itemEntity.lock();
    if (i && i->GetInWorld())
    {
      i->DeleteFromDB();
    }
  }
}

void ItemSpawner::timeOut()
{
  LockType lock(mutex);
  std::vector<boost::shared_ptr<SpawnPoint> >::const_iterator it;
  for (it = spawnpoints.begin(); it != spawnpoints.end(); it++)
  {
    CheckSpawnPoint(*it);
  }
  timeCounter++;
}

void ItemSpawner::CheckSpawnPoint(boost::shared_ptr<SpawnPoint> sp)
{
  // The item is gone or not in the world.
  if (!sp->itemEntity.lock() || !sp->itemEntity.lock()->GetInWorld())
  {
    if ( sp->pickTime == 0)
    {
      sp->pickTime = timeCounter;
    }
    if (timeCounter - sp->pickTime > sp->spawnInterval)
    {
      boost::shared_ptr<ItemEntity> item;
      try
      {
        Server* server = Server::getServer();
        item = server->GetItemTemplatesManager()->CreateItemFromTemplate(sp->itemTemplateId);
        printf("Spawning  '%s'\n", item->GetName().c_str());
      }
      catch (InvalidItemTemplate& /*ex*/)
      {
        printf("E: Invalid template %"SIZET"!\n", sp->itemTemplateId);
        return;
      }
      item->SetPosition(sp->position);
      item->SetInWorld(true);
      item->SaveToDB();

      Server::getServer()->getEntityManager()->Add(item);
      sp->itemEntity = item;
      sp->pickTime = 0;
    }
  }
}

void ItemSpawner::AddSpawnPoint(size_t itemTemplateId, WFMath::Point<3> position, size_t spawnInterval)
{
  boost::shared_ptr<SpawnPoint> sp(new SpawnPoint());
  sp->itemTemplateId = itemTemplateId;
  sp->position = position;
  sp->spawnInterval = spawnInterval;

  LockType lock(mutex);
  spawnpoints.push_back(sp);
}

void ItemSpawner::LoadFromDB()
{
  SpawnPointsTable* table = Server::getServer()->GetTableManager()->Get<SpawnPointsTable>();
  SpawnPointsTableVOArray points = table->GetAll();
  SpawnPointsTableVOArray::const_iterator it;
  for (it = points.begin(); it != points.end(); it++)
  {
    SpawnPointsTableVOp p = *it;
    AddSpawnPoint(p->ItemTemplate_id, p->position, p->interval);
  }
}

void ItemSpawner::CreateSpawnPoint(size_t itemTemplateId, WFMath::Point<3> position, size_t spawnInterval)
{
  SpawnPointsTable* table = Server::getServer()->GetTableManager()->Get<SpawnPointsTable>();
  size_t maxId = table->GetMaxId() + 1;
  table->Insert(maxId, itemTemplateId, position, spawnInterval);
  AddSpawnPoint(itemTemplateId, position, spawnInterval);
}

void ItemSpawner::RemoveAllSpawnPoints()
{
  SpawnPointsTable* table = Server::getServer()->GetTableManager()->Get<SpawnPointsTable>();
  table->DeleteAll();

  LockType lock(mutex);
  spawnpoints.clear();
}
