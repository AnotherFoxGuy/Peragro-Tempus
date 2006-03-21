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

#include "client/entity/ptcelentity.h"
#include "client/entity/ptentitymanager.h"

#include "csgeom/path.h"
#include "cstool/initapp.h"
#include "csutil/cmdline.h"
#include "csutil/csstring.h"
#include "csutil/csshlib.h"
#include "csutil/event.h"
#include "csutil/sysfunc.h"
#include "csutil/syspath.h"
#include "iengine/camera.h"
#include "iengine/campos.h"
#include "iengine/mesh.h"
#include "iengine/sector.h"
#include "iengine/texture.h"
#include "iengine/material.h"
#include "iengine/scenenode.h"
#include "iengine/movable.h"
#include "imesh/object.h"
#include "imesh/spritecal3d.h"
#include "imesh/sprite2d.h"
#include "imesh/genmesh.h"
//#include "imesh/objmodel.h"
#include "iutil/databuff.h"
#include "iutil/event.h"
#include "iutil/eventq.h"
#include "iutil/object.h"
#include "iutil/vfs.h"
#include "ivaria/collider.h"
#include "ivideo/graph2d.h"
#include "ivideo/natwin.h"
#include "ivideo/txtmgr.h"
#include "ivideo/material.h"

#include "iutil/objreg.h"

#include "imap/loader.h"

#include "physicallayer/pl.h"
#include "physicallayer/propfact.h"
#include "physicallayer/propclas.h"
#include "physicallayer/entity.h"
#include "physicallayer/persist.h"
#include "celtool/initapp.h"
#include "celtool/persisthelper.h"
#include "celtool/stdparams.h"
#include "behaviourlayer/bl.h"
#include "propclass/test.h"
#include "propclass/mesh.h"
#include "propclass/meshsel.h"
#include "propclass/solid.h"
#include "propclass/inv.h"
#include "propclass/chars.h"
#include "propclass/move.h"
#include "propclass/prop.h"
#include "propclass/tooltip.h"
#include "propclass/defcam.h"
#include "propclass/gravity.h"
#include "propclass/timer.h"
#include "propclass/region.h"
#include "propclass/input.h"
#include "propclass/navgraph.h"
#include "propclass/linmove.h"
#include "propclass/actormove.h"
#include "propclass/quest.h"
#include "propclass/trigger.h"
#include "propclass/zone.h"
#include "propclass/sound.h"
#include <propclass/colldet.h>

//#include "common/entity/entity.h"
#include "client/network/network.h"

#include "common/network/netmessage.h"


ptEntityManager::ptEntityManager (iObjectRegistry* obj_reg)
{
  engine = CS_QUERY_REGISTRY(obj_reg, iEngine);
  //if (!engine) return ReportError("Failed to locate 3D engine!");

  vfs = CS_QUERY_REGISTRY(obj_reg, iVFS);
  //if (!vfs) return ReportError("Failed to locate VFS!");

  pl = CS_QUERY_REGISTRY (obj_reg, iCelPlLayer);
  //if (!pl) return ReportError("Failed to load CEL Physical Layer");

  world_loaded = false;
  playing = false;
  own_char_id = -1;
}

ptEntityManager::~ptEntityManager ()
{
}

bool ptEntityManager::Initialize (iObjectRegistry* obj_reg)
{
  loader = CS_QUERY_REGISTRY (obj_reg, iLoader);
  if (!loader)
  {
    //Report (CS_REPORTER_SEVERITY_ERROR, "No loader!");
    return false;
  }

  // load the factory for particles from file
  //if ( !loader->LoadLibraryFile ("/peragro/meshes/effects/alleffects.xml") ) return false;

  return true;
}

void ptEntityManager::Handle ()
{
  addEntity();
  delEntity();
  moveEntity();
  DrUpdateEntity();
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

  createCelEntity(ent);

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
void ptEntityManager::moveEntity(int entity_id, float speed, float* fv1, float* fv2)
{
  iCelEntity* entity = findCelEntById(entity_id);
  if (entity)
  {
    csRef<iPcLinearMovement> pclinmove = CEL_QUERY_PROPCLASS_ENT(entity, iPcLinearMovement);
    if (pclinmove.IsValid())
    {
      csVector3 pos_clt;
      iSector* sect = 0;
      float rot = 0;

      csVector3 pos_ori(fv1[0], fv1[1], fv1[2]);
      csVector3 pos_dst(fv2[0], fv2[1], fv2[2]);

      //make path
      csPath* path = new csPath(2);
      path->SetPositionVector(0,pos_ori);
      path->SetPositionVector(1,pos_dst);
      path->SetForwardVector(0,pos_ori-pos_dst);
      path->SetForwardVector(1,pos_ori-pos_dst);
      path->SetTime(0, 0.0f);
      path->SetTime(1, 1.0f);
      path->SetUpVector(0, csVector3(0,1,0));
      path->SetUpVector(1, csVector3(0,1,0));

      pclinmove->GetLastFullPosition(pos_clt, rot, sect);

      // Some basic physics
      // v = s/t
      // t = s / v
      // v_s / s_s = v_c / s_c
      // v_c = v_s * v_c / s_s
      speed = speed * (pos_dst - pos_clt).Norm() / (pos_dst - pos_ori).Norm();

      pclinmove->SetPath(path);
      pclinmove->SetPathSpeed(-speed);
      pclinmove->SetPathTime(0);

      csRef<iPcMesh> mesh = CEL_QUERY_PROPCLASS_ENT(entity, iPcMesh);
      csRef<iSpriteCal3DState> sprcal3d =
        SCF_QUERY_INTERFACE (mesh->GetMesh()->GetMeshObject(), iSpriteCal3DState);
      if (sprcal3d)
        sprcal3d->SetVelocity(speed);
    }
  }
  else printf("entity not found!");
}

void ptEntityManager::moveEntity(int entity_id, float walk, float turn)
{
  mutex.lock();
  printf("Add movement for '%d': w(%.2f) r(%.2f)\n", entity_id, walk, turn);
  Movement* movement = new Movement();
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
  Movement* movement = move_entity_name.Pop();

  iCelEntity* entity = findCelEntById(movement->entity_id);
  if (entity)
  {
    csRef<iPcLinearMovement> pclinmove = CEL_QUERY_PROPCLASS_ENT(entity, iPcLinearMovement);
    if (pclinmove.IsValid())
    {
      pclinmove->SetAngularVelocity(csVector3(0,-movement->turn,0));
      pclinmove->SetVelocity(csVector3(0,0,-movement->walk));
    }
    csRef<iPcMesh> mesh = CEL_QUERY_PROPCLASS_ENT(entity, iPcMesh);
    csRef<iSpriteCal3DState> sprcal3d =
      SCF_QUERY_INTERFACE (mesh->GetMesh()->GetMeshObject(), iSpriteCal3DState);
    if (sprcal3d)
      sprcal3d->SetVelocity(movement->walk);
  }
  delete movement;
  mutex.unlock();
}

void ptEntityManager::DrUpdateEntity(DrUpdate* drupdate)
{
  mutex.lock();
  if (own_char_id == drupdate->entity_id)
  {
    delete drupdate;
    return;
  }
  drupdate_entity_name.Push(drupdate);
  mutex.unlock();
}

void ptEntityManager::DrUpdateEntity()
{
  if (!drupdate_entity_name.GetSize()) return;
  mutex.lock();
  DrUpdate* drupdate = drupdate_entity_name.Pop();

  iCelEntity* entity = findCelEntById(drupdate->entity_id);
  if (entity)
  {
    csRef<iPcLinearMovement> pclinmove = CEL_QUERY_PROPCLASS_ENT(entity, iPcLinearMovement);
    if (pclinmove.IsValid())
    {
      //bool on_ground;
      //float speed, rot, avel;
      //csVector3 pos, vel, wvel;

      iSector* sector = engine->FindSector(drupdate->sector.GetData());

      //pclinmove->GetDRData(on_ground, speed, pos, rot, sector, vel, wvel, avel);
      //printf("Get DR: %.2f, <%.2f,%.2f,%.2f>, %.2f\n", speed, pos.x, pos.y, pos.z, rot);
      pclinmove->SetDRData(drupdate->on_ground, drupdate->speed, drupdate->pos,
        drupdate->rot, sector, drupdate->vel, drupdate->wvel, drupdate->avel);
    }
  }
  delete drupdate;
  mutex.unlock();
}

UpdateDREntityRequestMessage ptEntityManager::DrUpdateOwnEntity()
{
  UpdateDREntityRequestMessage drmsg;

  if (own_char_id != -1)
  {
    if (ownent)
    {
      csRef<iPcLinearMovement> pclinmove = CEL_QUERY_PROPCLASS_ENT(ownent, iPcLinearMovement);

      bool on_ground;
      float speed, rot, avel;
      csVector3 pos, vel, wvel;
      iSector* sector;

      pclinmove->GetDRData(on_ground, speed, pos, rot, sector, vel, wvel, avel);
      //printf("Send DR: %.2f, <%.2f,%.2f,%.2f>, %.2f\n", speed, pos.x, pos.y, pos.z, rot);

      drmsg.setOnGround(on_ground?1:0);
      drmsg.setSpeed(speed);
      drmsg.setRot(rot);
      drmsg.setAVel(avel);
      drmsg.setPos(pos.x,pos.y,pos.z);
      drmsg.setVel(vel.x,vel.y,vel.z);
      drmsg.setWVel(wvel.x,wvel.y,wvel.z);
      if (sector->QueryObject()->GetName())
        drmsg.setSector(ptString(sector->QueryObject()->GetName(), strlen(sector->QueryObject()->GetName())));
      else
        drmsg.setSector(ptString(0,0));
    }
  }

  return drmsg;
}
void ptEntityManager::createCelEntity(Entity* ent)
{
  csRef<iCelEntity> entity = pl->CreateEntity();

  // Create the tooltip for this entity.
  pl->CreatePropertyClass(entity, "pctooltip");
  csRef<iPcTooltip> nametag = CEL_QUERY_PROPCLASS_ENT(entity, iPcTooltip);
  nametag->SetText(*ent->getName());
  nametag->SetJustify(CEL_TOOLTIP_CENTER);
  nametag->SetBackgroundColor(0, 0, 0);

  if (ent->getType() == Entity::ItemEntity)
  {
    char buffer[1024];
    sprintf(buffer, "%s:%d:%d", *ent->getName(), ent->getType(), ent->getId());
    entity->SetName(buffer);
    nametag->SetTextColor(140, 140, 255);
  }
  else if (ent->getType() == Entity::PlayerEntity)
  {
    char buffer[32];
    cs_snprintf(buffer, 32, "player_%d", ent->getId());
    entity->SetName(buffer);
    nametag->SetTextColor(0, 211, 111);
  }
  else
    entity->SetName(*ent->getName());

  pl->CreatePropertyClass(entity, "pcmesh");
  csRef<iPcMesh> pcmesh = CEL_QUERY_PROPCLASS_ENT(entity, iPcMesh);
  pl->CreatePropertyClass(entity, "pclinearmovement");
  csRef<iPcLinearMovement> pclinmove = CEL_QUERY_PROPCLASS_ENT(entity, iPcLinearMovement);
  pl->CreatePropertyClass(entity, "pcsolid");
  csRef<iPcSolid> pctemp = CEL_QUERY_PROPCLASS_ENT(entity, iPcSolid);

  pl->CreatePropertyClass(entity, "pcactormove");
  csRef<iPcActorMove> pcactormove = CEL_QUERY_PROPCLASS_ENT (entity, iPcActorMove);
  pcactormove->SetMovementSpeed (2.0f);
  pcactormove->SetRunningSpeed (5.0f);
  pcactormove->SetRotationSpeed (1.75f);
  pcactormove->SetJumpingVelocity (6.31f);

  iSector* sector = engine->FindSector(*ent->getSector());

  printf("Loading Actor\n");
  vfs->ChDir("/cellib/objects/");
  pcmesh->SetMesh(*ent->getMesh(), "/peragro/meshes/all.xml");

  csRef<iCelEntity> region = pl->FindEntity("World");
  if (region)
  {
    csRef<iPcRegion> pcregion = CEL_QUERY_PROPCLASS_ENT(region, iPcRegion);
    pcmesh->MoveMesh(pcregion->GetStartSector(), pcregion->GetStartPosition());
  }

  if (own_char_id == ent->getId())
  {
    printf("Adding Entity '%s' as me\n", entity->GetName());
    pl->CreatePropertyClass(entity, "pcdefaultcamera");
    csRef<iPcDefaultCamera> pccamera = CEL_QUERY_PROPCLASS_ENT(entity, iPcDefaultCamera);
    pccamera->SetMode(iPcDefaultCamera::thirdperson, true);
    pccamera->SetPitch(-0.18f);

    // Set up own player cam and entity for faster access.
    owncam = pccamera->GetCamera();
    ownent = entity;
    ownname = *ent->getName();
  }
  else
  {
    printf("Adding Entity '%s'\n", entity->GetName());
  }

  pclinmove->InitCD(
    csVector3(0.5f,0.8f,0.5f),
    csVector3(0.5f,0.8f,0.5f),
    csVector3(0,0,0));

  csVector3 pos(ent->getPos()[0], ent->getPos()[1], ent->getPos()[2]);
  pclinmove->SetPosition(pos,0,sector);

  pl->CreatePropertyClass(entity, "pcproperties");
  csRef<iPcProperties> pcprop = CEL_QUERY_PROPCLASS_ENT(entity, iPcProperties);
  pcprop->SetProperty("Entity Type", (long)ent->getType());
  pcprop->SetProperty("Entity ID", (long)ent->getId());
  pcprop->SetProperty("Entity Name", *ent->getName());

  // Add to the entities list
  ptCelEntity ptent (ent->getId(), ent->getType(), *ent->getName(), entity);
  entities.Push (ptent);

}
