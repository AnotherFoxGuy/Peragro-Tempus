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

  this->obj_reg = obj_reg;

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
  moveToEntity();
  DrUpdateEntity();
  //updatePcProp();
}

PtEntity* ptEntityManager::findPtEntById(int id)
{
  for (size_t i = 0; i < entities.Length(); i++)
  {
    if (entities[i]->GetId() == id)
    {
      return entities[i];
    }
  }
  return 0;
}

iCelEntity* ptEntityManager::findCelEntById(int id)
{
  for (size_t i = 0; i < entities.Length(); i++)
  {
    if (entities[i]->GetId() == id)
    {
      return entities[i]->GetCelEntity();
    }
  }
  return 0;
}

void ptEntityManager::addEntity(PtEntity* entity)
{
  mutex.lock();
  new_entity_name.Push(entity);
  mutex.unlock();
}

void ptEntityManager::addEntity()
{
  if (!new_entity_name.GetSize() || !world_loaded || !playing) return;

  mutex.lock();

  PtEntity* entity = new_entity_name.Pop();

  if(entity)
  {
    entity->Create();

    if (own_char_id == entity->GetId())
    {
      printf("ptEntityManager:  Adding Entity '%s' as me\n", entity->GetName());
      csRef<iPcDefaultCamera> pccamera = CEL_QUERY_PROPCLASS_ENT(entity->GetCelEntity(), iPcDefaultCamera);
      pccamera->SetMode(iPcDefaultCamera::thirdperson, true);
      pccamera->SetPitch(-0.18f);

      // Set up own player cam and entity for faster access.
      owncam = pccamera->GetCamera();
      ownent = entity->GetCelEntity();
      ownname = entity->GetName();
    }
    else
      printf("ptEntityManager: Adding Entity '%s'\n", entity->GetName());

    // Add our entity to the list.
    entities.Push (entity);
  }

  mutex.unlock();
}

void ptEntityManager::delEntity(PtEntity* entity)
{
  mutex.lock();
  del_entity_name.Push(entity);
  mutex.unlock();
}

void ptEntityManager::delEntity()
{
  if (!del_entity_name.GetSize()) return;

  mutex.lock();

  PtEntity* ent = del_entity_name.Pop();

  int id = ent->GetId();

  for (size_t i = 0; i < entities.Length(); i++)
  {
    if (entities[i]->GetId() == id)
    {
      pl->RemoveEntity(entities[i]->GetCelEntity());
      entities.DeleteIndex(i);
    }
  }

  delete ent;

  mutex.unlock();
}

float ptEntityManager::GetAngle (const csVector3& v1, const csVector3& v2)
{
  float len = sqrt (csSquaredDist::PointPoint (v1, v2));
  float angle = acos ((v2.x-v1.x) / len);
  if ((v2.z-v1.z) > 0) angle = 2*PI - angle;
  angle += PI / 2.0f;
  if (angle > 2*PI) angle -= 2*PI;
  return angle;
}

void ptEntityManager::moveEntity(int entity_id, float walk_speed, float* fv1, float* fv2)
{
  PtEntity* entity = findPtEntById(entity_id);
  if (!entity) return;

  mutex.lock();

  MoveToData* moveTo = new MoveToData();

  csVector3 pos_ori(fv1[0], fv1[1], fv1[2]);
  csVector3 pos_dst(fv2[0], fv2[1], fv2[2]);

  moveTo->turn_speed = 2*PI; // 1 revolution per second
  moveTo->walk_speed = walk_speed;

  // Getting the real world position of our entity.
  // TODO Do some SoftDRUpdate with the server position(fv1)?
  // Or is this redundant since the end position WILL be the same?
  csRef<iPcLinearMovement> pclinmove = CEL_QUERY_PROPCLASS_ENT(entity->GetCelEntity(), iPcLinearMovement);
  float cur_yrot;
  csVector3 cur_position;
  iSector* cur_sector;
  pclinmove->GetLastFullPosition(cur_position, cur_yrot, cur_sector);

  csVector3 vec (0,0,1);
  float yrot_dst = GetAngle (pos_dst - cur_position, vec);
  
  moveTo->dest_angle = yrot_dst;
  
  moveTo->walk_duration = (pos_dst - cur_position).Norm() / moveTo->walk_speed;
  
  moveTo->elapsed_time = 0;
  moveTo->walking = false;
  moveTo->entity_id = entity_id;
  
  // Remove any other moveTo actions for this entity
  for (size_t i = 0; i < move_to_entity_name.GetSize(); i++)
  {
    MoveToData* m = move_to_entity_name.Get(i);
  
    if (m->entity_id == moveTo->entity_id)
    {
      move_to_entity_name.Delete(m);
      break;
    }
  }
  
  move_to_entity_name.Push(moveTo);
  
  mutex.unlock();
}

void ptEntityManager::moveToEntity()
{
  if (!move_to_entity_name.GetSize()) return;
  mutex.lock();
  
  for (size_t i = 0; i < move_to_entity_name.GetSize(); i++)
  {
    MoveToData* moveTo = move_to_entity_name.Get(i);

    PtEntity* entity = findPtEntById(moveTo->entity_id);
    if (entity)
    {
      if(entity->MoveTo(moveTo))
        move_to_entity_name.Delete(moveTo);
    }
  }
  mutex.unlock();
}

void ptEntityManager::DrUpdateEntity(DrUpdateData* drupdate)
{
  mutex.lock();
  if (own_char_id == drupdate->entity_id)
  {
    delete drupdate;
    mutex.unlock();
    return;
  }
  drupdate_entity_name.Push(drupdate);
  mutex.unlock();
}

void ptEntityManager::DrUpdateEntity()
{
  if (!drupdate_entity_name.GetSize()) return;
  mutex.lock();
  DrUpdateData* drupdate = drupdate_entity_name.Pop();

  PtEntity* entity = findPtEntById(drupdate->entity_id);
  if (entity)
  {
    entity->DrUpdate(drupdate);
  }
  delete drupdate;
  mutex.unlock();
}
