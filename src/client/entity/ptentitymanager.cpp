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

#include <iutil/objreg.h>
#include <imap/loader.h>

#include "client/network/network.h"

#include "common/network/netmessage.h"

#include "client/event/eventmanager.h"


ptEntityManager::ptEntityManager (iObjectRegistry* obj_reg)
{
  this->obj_reg = obj_reg;

  engine =  csQueryRegistry<iEngine> (obj_reg);

  vfs =  csQueryRegistry<iVFS> (obj_reg);

  stringset = csQueryRegistryTagInterface<iStringSet> (obj_reg,
    "crystalspace.shared.stringset");

  vc =  csQueryRegistry<iVirtualClock> (obj_reg);

  pl =  csQueryRegistry<iCelPlLayer> (obj_reg);

  ownent = 0;
}

ptEntityManager::~ptEntityManager ()
{
}

bool ptEntityManager::Initialize ()
{
  world_loaded = false;
  playing = false;
  own_char_id = 0;

  using namespace PT::Events;

  // Register listener for EntityAddEvent.
  EventHandler<ptEntityManager>* cbAdd = new EventHandler<ptEntityManager>(&ptEntityManager::AddEntity, this);
  PointerLibrary::getInstance()->getEventManager()->AddListener("EntityAddEvent", cbAdd);

  // Register listener for EntityRemoveEvent.
  EventHandler<ptEntityManager>* cbRemove = new EventHandler<ptEntityManager>(&ptEntityManager::RemoveEntity, this);
  PointerLibrary::getInstance()->getEventManager()->AddListener("EntityRemoveEvent", cbRemove);

  return true;
}

void ptEntityManager::Handle ()
{
    moveEntity();
    moveToEntity();
    DrUpdateEntity();
    updatePcProp();
    equip();
    mount();
    unmount();
    teleport();
}

PtEntity* ptEntityManager::findPtEntById(int id)
{
  for (size_t i = 0; i < entities.GetSize(); i++)
  {
    if (entities.Get(i)->GetId() == id)
    {
      return entities.Get(i);
    }
  }
  return 0;
}

iCelEntity* ptEntityManager::findCelEntById(int id)
{
  for (size_t i = 0; i < entities.GetSize(); i++)
  {
    if (entities.Get(i)->GetId() == id)
    {
      return entities.Get(i)->GetCelEntity();
    }
  }
  return 0;
}

bool ptEntityManager::AddEntity(PT::Events::Event* ev)
{
  using namespace PT::Events;

  EntityEvent* entityEv = static_cast<EntityEvent*> (ev);
  if (!entityEv)
  {
    printf("E: Not an Entity event!\n");
    return false;
  }
  EntityAddEvent* entityAddEv = static_cast<EntityAddEvent*> (entityEv);
  if (!entityAddEv)
  {
    printf("E: Not an EntityAddEvent event!\n");
    return false;
  }
  if ( findPtEntById(entityAddEv->entityId) )
  {
    printf("W: Skipping already existing entity '%s(%d)'\n", entityAddEv->entityName.c_str(), entityAddEv->entityId);
    return true;
  }

  PtEntity* entity; 
  if (entityAddEv->entityType == EntityEvent::PlayerEntity)
  {
    entity = new PtPcEntity();
    // Check if it is your own player.
    if (own_char_id == entityAddEv->entityId)
      ((PtPcEntity*)entity)->SetOwnEntity(true);
    // Add equipment.
    std::vector<std::pair<int, int>>::iterator it;
    for(it = entityAddEv->equipment.begin(); it != entityAddEv->equipment.end(); ++it)
      ((PtPcEntity*)entity)->GetEquipment()->Equip(it->first, it->second);
  }
  else if (entityAddEv->entityType == EntityEvent::NPCEntity)
  {
    entity = new PtNpcEntity();
  }
  else if (entityAddEv->entityType == EntityEvent::ItemEntity)
  {
    entity = new PtItemEntity();
    ((PtItemEntity*)entity)->SetItemId(entityAddEv->meshId);
  }
  else if (entityAddEv->entityType == EntityEvent::MountEntity)
  {
    entity = new PtMountEntity();
  }
  else if (entityAddEv->entityType == EntityEvent::DoorEntity)
  {
    entity = new PtDoorEntity();
    ((PtDoorEntity*)entity)->SetLocked(entityAddEv->locked);
    ((PtDoorEntity*)entity)->SetOpen(entityAddEv->open);
  }
  else
  {
    printf("E: Invalid Entity type: %d !\n", entityAddEv->entityType);
    return true;
  }

  entity->SetId(entityAddEv->entityId);
  entity->SetName(entityAddEv->entityName.c_str());
  entity->SetMeshName(entityAddEv->meshName.c_str());
  entity->SetPosition(entityAddEv->position);
  entity->SetSectorName(entityAddEv->sectorName.c_str());

  entity->Create();

  if (own_char_id == entityAddEv->entityId)
  {
    printf("I:  Adding Entity '%s(%d)' as me\n", entity->GetName().GetData(), entity->GetId());

    // Set up own player cam and entity for faster access.
    csRef<iPcDefaultCamera> pccamera = CEL_QUERY_PROPCLASS_ENT(entity->GetCelEntity(), iPcDefaultCamera);
    if (pccamera.IsValid())
    {
      pccamera->SetAutoDraw(false);
      //pccamera->SetMode(iPcDefaultCamera::thirdperson, true);
      //pccamera->SetPitch(-0.18f);
    }

    owncam = pccamera;
    ownent = entity;
    owncelent = entity->GetCelEntity();
    ownname = entity->GetName();

    

  }
  else
    printf("I: Adding Entity '%s(%d)'\n", entity->GetName().GetData(), entity->GetId());

  // Add our entity to the list.
  entities.Push(entity);

  return true;
}

bool ptEntityManager::RemoveEntity(PT::Events::Event* ev)
{
  using namespace PT::Events;

  EntityEvent* entityEv = static_cast<EntityEvent*> (ev);
  if (!entityEv)
  {
    printf("E: Not an Entity event!\n");
    return false;
  }
  EntityRemoveEvent* entityRemoveEv = static_cast<EntityRemoveEvent*> (entityEv);
  if (!entityRemoveEv)
  {
    printf("E: Not an EntityRemoveEvent event!\n");
    return false;
  }

  int id = entityRemoveEv->entityId;

  for (size_t i = 0; i < entities.GetSize(); i++)
  {
    if (entities[i]->GetId() == id)
    {
      pl->RemoveEntity(entities[i]->GetCelEntity());
      entities.DeleteIndex(i);
    }
  }

  return true;
}

void ptEntityManager::delAllEntities()
{
  ownent = 0;
  for (size_t i = 0; i < entities.GetSize(); i++)
  {
    pl->RemoveEntity(entities[i]->GetCelEntity());
    entities.DeleteIndex(i);
  }
}

void ptEntityManager::teleport(int entity_id, float* pos, const char* sector)
{
  mutex.lock();
  csVector3 position(pos[0],pos[1],pos[2]);
  printf("ptEntityManager: Teleporting entity '%d'\n", entity_id);
  TeleportData* teleportdata = new TeleportData();
  teleportdata->entity_id = entity_id;
  teleportdata->position = position;
  teleportdata->sector = sector;
  teleport_entity_name.Push(teleportdata);
  mutex.unlock();
}

void ptEntityManager::teleport()
{
  if (!teleport_entity_name.GetSize()) return;
  mutex.lock();
  TeleportData* teleportdata = teleport_entity_name.Pop();

  PtEntity* entity = findPtEntById(teleportdata->entity_id);
  if(entity)
    entity->Teleport(teleportdata->position, teleportdata->sector);
  delete teleportdata;
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
  if(!entity->GetCelEntity()) 
  {
    printf("ptEntityManager: ERROR : moveEntity %d: No CelEntity\n", entity_id);
    return;
  }

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

void ptEntityManager::moveEntity(int entity_id, float walk, float turn)
{
  mutex.lock();
  printf("ptEntityManager: Add movement for '%d': w(%.2f) r(%.2f)\n", entity_id, walk, turn);
  MovementData* movement = new MovementData();
  movement->entity_id = entity_id;
  movement->walk = walk;
  movement->turn = turn;
  move_entity_name.Push(movement);
  mutex.unlock();
}

void ptEntityManager::moveEntity()
{
  if (!move_entity_name.GetSize()) return;
  mutex.lock();
  MovementData* movement = move_entity_name.Pop();

  PtEntity* entity = findPtEntById(movement->entity_id);
  if (entity)
  {
    entity->Move(movement);
  }
  delete movement;
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

void ptEntityManager::updatePcProp()
{
  if (!update_pcprop_entity_name.GetSize()) return;
  mutex.lock();

  UpdatePcPropData* update_pcprop;
  for (size_t i = 0; i < update_pcprop_entity_name.GetSize(); i++)
  {
    update_pcprop = update_pcprop_entity_name.Pop();
    int id = update_pcprop->entity_id;
    PtEntity *ent = findPtEntById(id);
    if (ent)
      ent->UpdatePcProp(update_pcprop);
    else
      printf("ptEntityManager: ERROR Failed to do updatePcProp on %d!!\n", id);

    delete update_pcprop;
  }
  mutex.unlock();
}

void ptEntityManager::updatePcProp(int entity_id, const char *pcprop, celData &value)
{
  mutex.lock();
  UpdatePcPropData* updatePcprop = new UpdatePcPropData();
  updatePcprop->entity_id = entity_id;
  updatePcprop->pcprop = pcprop;
  updatePcprop->value = value;
  
  update_pcprop_entity_name.Push(updatePcprop);
  mutex.unlock();
}

void ptEntityManager::equip(int entity_id, int item_id, int slot_id)
{
  mutex.lock();
  printf("ptEntityManager: Add equip for '%d': item %d in slot %d\n", entity_id, item_id, slot_id);
  EquipData* equipdata = new EquipData();
  equipdata->entity_id = entity_id;
  equipdata->item_id = item_id;
  equipdata->slot_id = slot_id;
  equip_entity_name.Push(equipdata);
  mutex.unlock();
}

void ptEntityManager::equip()
{
  if (!equip_entity_name.GetSize()) return;
  mutex.lock();
  EquipData* equipdata = equip_entity_name.Pop();

  PtEntity* entity = findPtEntById(equipdata->entity_id);
  if (entity)
  {
    if (entity->GetType() == PtEntity::PlayerEntity)
    {
      if(!equipdata->item_id == 0)
        ((PtPcEntity*) entity)->GetEquipment()->Equip(equipdata->item_id, equipdata->slot_id);
      else
        ((PtPcEntity*) entity)->GetEquipment()->UnEquip(equipdata->slot_id);
    }
  }
  delete equipdata;
  mutex.unlock();
}

void ptEntityManager::mount(int entity_id, int mount_id, bool control)
{
  mutex.lock();

  MountData* mountdata = new MountData();
  mountdata->entity_id = entity_id;
  mountdata->mount_id= mount_id;
  mountdata->control = control;
  mount_entity_name.Push(mountdata);

  mutex.unlock();
}

void ptEntityManager::mount()
{
  if (!mount_entity_name.GetSize()) return;
  mutex.lock();
  MountData* mountdata = mount_entity_name.Pop();

  PtEntity* entity = findPtEntById(mountdata->entity_id);
  PtEntity* mount = findPtEntById(mountdata->mount_id);
  if (entity && mount)
  {
    if (entity->GetType() == PtEntity::PlayerEntity && mount->GetType() == PtEntity::MountEntity)
    {
      PtMountEntity* m = static_cast<PtMountEntity*>(mount);
      m->Mount(entity);
    }
  }
  delete mountdata;
  mutex.unlock();
}

void ptEntityManager::unmount(int entity_id, int mount_id)
{
  mutex.lock();

  UnMountData* mountdata = new UnMountData();
  mountdata->entity_id = entity_id;
  mountdata->mount_id= mount_id;
  unmount_entity_name.Push(mountdata);

  mutex.unlock();
}

void ptEntityManager::unmount()
{
  if (!unmount_entity_name.GetSize()) return;
  mutex.lock();
  UnMountData* mountdata = unmount_entity_name.Pop();

  PtEntity* entity = findPtEntById(mountdata->entity_id);
  PtEntity* mount = findPtEntById(mountdata->mount_id);
  if (entity && mount)
  {
    if (entity->GetType() == PtEntity::PlayerEntity && mount->GetType() == PtEntity::MountEntity)
    {
      PtMountEntity* m = static_cast<PtMountEntity*>(mount);
      m->UnMount(entity);
    }
  }
  delete mountdata;
  mutex.unlock();
}

void ptEntityManager::DrUpdateOwnEntity()
{
  return;
  DrUpdateRequestMessage drmsg;

  if (own_char_id != -1)
  {
    if (owncelent.IsValid())
    {
      csRef<iPcLinearMovement> pclinmove = CEL_QUERY_PROPCLASS_ENT(owncelent, iPcLinearMovement);
      if (pclinmove.IsValid())
      {
        bool on_ground;
        float speed, rot, avel;
        csVector3 pos, vel, wvel;
        iSector* sector = 0;

        pclinmove->GetDRData(on_ground, speed, pos, rot, sector, vel, wvel, avel);
        //printf("Send DR: %.2f, <%.2f,%.2f,%.2f>, %.2f\n", speed, pos.x, pos.y, pos.z, rot);

        drmsg.setRotation(rot);
        drmsg.setPos(pos.x,pos.y,pos.z);
        if (sector && sector->QueryObject()->GetName())
          drmsg.setSector(ptString(sector->QueryObject()->GetName(), strlen(sector->QueryObject()->GetName())));
        else
          drmsg.setSector(ptString(0,0));

        PointerLibrary::getInstance()->getNetwork()->send(&drmsg);
      }
    }
  }
}
