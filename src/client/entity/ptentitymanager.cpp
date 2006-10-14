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

#include "client/entity/ptentitymanager.h"

#include "iutil/objreg.h"

#include "imap/loader.h"

#include "client/network/network.h"

#include "common/network/netmessage.h"


ptEntityManager::ptEntityManager (iObjectRegistry* obj_reg, Client* client)
{
  this->client = client;

  engine = CS_QUERY_REGISTRY(obj_reg, iEngine);

  vfs = CS_QUERY_REGISTRY(obj_reg, iVFS);

  stringset = CS_QUERY_REGISTRY_TAG_INTERFACE (obj_reg,
    "crystalspace.shared.stringset", iStringSet);

  vc = CS_QUERY_REGISTRY (obj_reg, iVirtualClock);

  pl = CS_QUERY_REGISTRY (obj_reg, iCelPlLayer);
}

ptEntityManager::~ptEntityManager ()
{
}

bool ptEntityManager::Initialize ()
{
  world_loaded = false;
  playing = false;
  own_char_id = 0;

  return true;
}

void ptEntityManager::Handle ()
{
  addEntity();
  delEntity();
  //moveEntity();
  //moveToEntity();
  //DrUpdateEntity();
  //updatePcProp();
}

PtEntity* ptEntityManager::findPtEntById(int id)
{
  for (size_t i = 0; i < entities.Length(); i++)
  {
    if (entities[i].GetId() == id)
    {
      return &entities[i];
    }
  }
  return 0;
}

iCelEntity* ptEntityManager::findCelEntById(int id)
{
  for (size_t i = 0; i < entities.Length(); i++)
  {
    if (entities[i].GetId() == id)
    {
      return entities[i].GetCelEntity();
    }
  }
  return 0;
}

void ptEntityManager::addEntity(Entity* entity)
{
  mutex.lock();
  new_entity_name.Push(entity);
  mutex.unlock();
}

void ptEntityManager::addEntity()
{
  if (!new_entity_name.GetSize() || !world_loaded || !playing) return;

  mutex.lock();

  Entity* ent = new_entity_name.Pop();

  PtEntity* entity = 0;
  switch (ent->getType())
  {
  //case PtEntity::ItemEntity: entity = new ItemEntity(); break;
  case PtEntity::PlayerEntity: entity = new PtPcEntity(); break;
  //case PtEntity::NPCEntity: entity = new PtNpcEntity(); break;
  //case PtEntity::DoorEntity: entity = new DoorEntity(); break;
  default : {printf("EntityHandler: ERROR Unknown entity type for %s!\n", *ent->getName()); return;}
  };

  entity->SetId(ent->getId());
  entity->SetName(*ent->getName());
  entity->Create();

  delete ent;

  mutex.unlock();
}

void ptEntityManager::delEntity(Entity* entity)
{
  mutex.lock();
  del_entity_name.Push(entity);
  mutex.unlock();
}

void ptEntityManager::delEntity()
{
  if (!del_entity_name.GetSize()) return;

  mutex.lock();

  Entity* ent = del_entity_name.Pop();

  int id = ent->getId();

  for (size_t i = 0; i < entities.Length(); i++)
  {
    if (entities[i].GetId() == id)
    {
      pl->RemoveEntity(entities[i].GetCelEntity());
      entities.DeleteIndex(i);
    }
  }

  delete ent;

  mutex.unlock();
}
