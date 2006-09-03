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

#include "cstool/initapp.h"
#include "csutil/cmdline.h"
#include "csutil/csstring.h"
#include "csutil/csshlib.h"
#include "csutil/event.h"
#include "csutil/sysfunc.h"
#include "csutil/syspath.h"

#include "csgeom/math3d.h"

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
#include "propclass/trigger.h"
#include "propclass/zone.h"
#include "propclass/sound.h"
#include <propclass/colldet.h>

//#include "common/entity/entity.h"
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
  moveEntity();
  moveToEntity();
  DrUpdateEntity();
  updatePcProp();
}

ptCelEntity* ptEntityManager::findPtEntById(int id)
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

void ptEntityManager::SetMaskColor(iMeshWrapper* mesh, const char* maskname, csVector4 colorvalue)
{
  if (!mesh)
  {
    printf("ptEntityManager: ERROR Couldn't find mesh to set color!\n");
    return;
  }

  if (!stringset)
  {
    printf("ptEntityManager: ERROR iStringSet wasn't loaded!\n");
    return;
  }
  
  printf("ptEntityManager: Setting variable\n");
  csStringID name = stringset->Request(maskname);
  csShaderVariable* variable = mesh->GetSVContext()->GetVariableAdd(name);
  variable->SetType(csShaderVariable::VECTOR4);
  variable->SetValue(colorvalue);
  mesh->GetSVContext()->AddVariable(variable);
}

void ptEntityManager::SetMaskColor(iMeshWrapper* mesh, const char* maskname, unsigned int colorvalue)
{
  csVector4 colorvalueh;
  colorvalueh.x =  ((colorvalue & 0xFF0000) >>  4) / 1.04448e+006; // Red
  colorvalueh.y =  ((colorvalue & 0x00FF00) >>  2) / 16320;        // Green
  colorvalueh.z =  (colorvalue & 0x0000FF) / 255;                  // Blue
  colorvalueh.w =  1;
  printf("Colorvalue is: %s", colorvalueh.Description().GetData());
  SetMaskColor(mesh, maskname, colorvalueh);
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
void ptEntityManager::updatePcProp()
{
  if (!update_pcprop_entity_name.GetSize()) return;
  printf("UPDATE PCPROP!!!!\n");
  mutex.lock();

  UpdatePcProp* update_pcprop;
  for (size_t i = 0; i < update_pcprop_entity_name.GetSize(); i++)
  {
   update_pcprop = update_pcprop_entity_name.Pop();
   int id = update_pcprop->entity_id;
   iCelEntity *ent = client->GetEntityManager()->findCelEntById(id);
   if (ent)
   {
     csRef<iPcProperties> pcprop = CEL_QUERY_PROPCLASS_ENT(ent, iPcProperties);
     switch(update_pcprop->value.type)
     {
	case CEL_DATA_BOOL:
     		pcprop->SetProperty(update_pcprop->pcprop, update_pcprop->value.value.bo);
		break;
	case CEL_DATA_LONG:
     		pcprop->SetProperty(update_pcprop->pcprop, (long)update_pcprop->value.value.l);
		break;
	case CEL_DATA_STRING:
     		pcprop->SetProperty(update_pcprop->pcprop, update_pcprop->value.value.s);
		break;
	default:
		printf("celData type not supported by updatePcProp!!\n");
     }
   }
   else
   {
	printf("NO ENTITY FOR updatePcProp!!\n");
   }
   delete update_pcprop;
  }
  mutex.unlock();
}
void ptEntityManager::updatePcProp(int entity_id, const char *pcprop, celData &value)
{
  mutex.lock();
  UpdatePcProp* updatePcprop = new UpdatePcProp();
  updatePcprop->entity_id = entity_id;
  updatePcprop->pcprop = pcprop;
  updatePcprop->value = value;
  
  update_pcprop_entity_name.Push(updatePcprop);
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
  iCelEntity* entity = findCelEntById(entity_id);
  if (!entity)
    return;

  mutex.lock();

  MoveTo* moveTo = new MoveTo();

  csVector3 pos_ori(fv1[0], fv1[1], fv1[2]);
  csVector3 pos_dst(fv2[0], fv2[1], fv2[2]);

  moveTo->turn_speed = 2*PI; // 1 revolution per second
  moveTo->walk_speed = walk_speed;

  // Getting the real world position of our entity.
  // TODO Do some SoftDRUpdate with the server position(fv1)?
  // Or is this redundant since the end position WILL be the same?
  csRef<iPcLinearMovement> pclinmove = CEL_QUERY_PROPCLASS_ENT(entity, iPcLinearMovement);
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
    MoveTo* m = move_to_entity_name.Get(i);
  
    if (m->entity_id == moveTo->entity_id)
    {
      move_to_entity_name.Delete(m);
      break;
    }
  }
  
  move_to_entity_name.Push(moveTo);
  
  mutex.unlock();
}

void ptEntityManager::moveEntity(int entity_id, float walk, float turn)
{
  mutex.lock();
  printf("ptEntityManager: Add movement for '%d': w(%.2f) r(%.2f)\n", entity_id, walk, turn);
  Movement* movement = new Movement();
  movement->entity_id = entity_id;
  movement->walk = walk;
  movement->turn = turn;
  move_entity_name.Push(movement);
  mutex.unlock();
}

void ptEntityManager::teleport(int entity_id, float* pos, const char* sector)
{
  csVector3 position(pos[0],pos[1],pos[2]);

  // TODO: do the mutex trick

  iCelEntity* entity = findCelEntById(entity_id);
  csRef<iPcMesh> pcmesh = CEL_QUERY_PROPCLASS_ENT(entity, iPcMesh);
  iMovable* mov = pcmesh->GetMesh()->GetMovable();
  mov->SetSector(engine->GetSectors()->FindByName(sector));
  mov->SetPosition(position);
}

void ptEntityManager::moveToEntity()
{
  if (!move_to_entity_name.GetSize()) return;
  
  csTicks ticks = vc->GetElapsedTicks ();
  if (!ticks)
    return;

  float elapsed = ticks/1000.0;
  
  mutex.lock();
  
  for (size_t i = 0; i < move_to_entity_name.GetSize(); i++)
  {
    MoveTo* moveTo = move_to_entity_name.Get(i);
  
    iCelEntity* entity = findCelEntById(moveTo->entity_id);
    if (entity)
    {
      csRef<iPcLinearMovement> pclinmove = CEL_QUERY_PROPCLASS_ENT(entity, iPcLinearMovement);
      csRef<iPcActorMove> pcactormove = CEL_QUERY_PROPCLASS_ENT(entity, iPcActorMove);
      if (pclinmove.IsValid() && pcactormove.IsValid())
      {
        csVector3 angular_vel;
        pclinmove->GetAngularVelocity(angular_vel);
        
        if (moveTo->elapsed_time == 0 && !moveTo->walking)
        {
          pcactormove->SetRotationSpeed(moveTo->turn_speed);
          pcactormove->RotateTo(moveTo->dest_angle);
        }
        else if (angular_vel.IsZero() && !moveTo->walking)
        {
          printf("Stopped turning at: %f\n", moveTo->elapsed_time);
          pcactormove->SetMovementSpeed(moveTo->walk_speed);
          pcactormove->Forward(true);
          moveTo->walking = true;
          moveTo->elapsed_time = 0;
        }
        else if (moveTo->elapsed_time >= moveTo->walk_duration && moveTo->walking)
        {
          printf("Stopped walking at: %f. Expected duration: %f\n", moveTo->elapsed_time, moveTo->walk_duration);
          pcactormove->Forward(false);
        }
      }
      
      if (moveTo->walking)
      {
        if (moveTo->elapsed_time >= moveTo->walk_duration)
        {
          pcactormove->Forward(false);
          move_to_entity_name.Delete(moveTo);
          continue;
        }
      }
      
      moveTo->elapsed_time += elapsed;
    }
  }
  
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

      client->getNetwork()->send(&drmsg);
    }
  }

  return drmsg;
}

void ptEntityManager::Attach(uint entityid, const char* socketName, const char* meshFactName )
{
  // Find the entity to attach the equipment to.
  iCelEntity* entity = findCelEntById(entityid);
  if ( !entity )
  {
    printf("Attach: Entity %d not found!\n", entityid);
    return;
  }

  // Get the mesh for the entity.
  csRef<iPcMesh> pcmesh = CEL_QUERY_PROPCLASS_ENT(entity, iPcMesh);
  if (pcmesh)
  {
    printf("Attach: pcmesh not found for entity %d!\n", entityid);
    return;
  }

  // Get his cal3d.
  csRef<iSpriteCal3DState> sprcal3d = SCF_QUERY_INTERFACE (pcmesh->GetMesh()->GetMeshObject(), iSpriteCal3DState);
  if (sprcal3d)
  {
    printf("Attach: sprcal3d not found for entity %d!\n", entityid);
    return;
  }

  // Get the specified socket.
  iSpriteCal3DSocket* socket = sprcal3d->FindSocket(socketName);
  if (socket)
  {
    printf("Attach: socket %s not found for entity %d!\n", socketName, entityid);
    return;
  }

  // Attach something to the socket.
  csRef<iCelEntity> equipment = pl->CreateEntity();
  pl->CreatePropertyClass(equipment, "pcmesh");
  csRef<iPcMesh> pcmesheq = CEL_QUERY_PROPCLASS_ENT(equipment, iPcMesh);
  if (pcmesheq->SetMesh(meshFactName, "/peragro/meshes/all.xml"))
  {
    pcmesheq->GetMesh()->QuerySceneNode()->SetParent( pcmesh->GetMesh()->QuerySceneNode ());
    socket->SetMeshWrapper(pcmesheq->GetMesh());
    printf("Attach: equipment attached!\n");
    //socket->SetMeshWrapper(0); // detaches the mesh.
  }
}

void ptEntityManager::createCelEntity(Entity* ent)
{
  csRef<iCelEntity> entity = pl->CreateEntity();

  if (ent->getType() == Entity::ItemEntity)
  {
    char buffer[1024];
    sprintf(buffer, "%s:%d:%d", *ent->getName(), ent->getType(), ent->getId());
    entity->SetName(buffer);
  }
  else if (ent->getType() == Entity::PlayerEntity)
  {
    char buffer[32];
    cs_snprintf(buffer, 32, "player_%d", ent->getId());
    entity->SetName(buffer);
  }
  else
  {
    entity->SetName(*ent->getName());
  }

  pl->CreatePropertyClass(entity, "pcmesh");
  csRef<iPcMesh> pcmesh = CEL_QUERY_PROPCLASS_ENT(entity, iPcMesh);
  csRef<iPcLinearMovement> pclinmove;
  pl->CreatePropertyClass(entity, "pcsolid");
  csRef<iPcSolid> pctemp = CEL_QUERY_PROPCLASS_ENT(entity, iPcSolid);

  pl->CreatePropertyClass(entity, "pcactormove");
  csRef<iPcActorMove> pcactormove = CEL_QUERY_PROPCLASS_ENT (entity, iPcActorMove);
  pcactormove->SetMovementSpeed (2.0f);
  pcactormove->SetRunningSpeed (5.0f);
  pcactormove->SetRotationSpeed (1.75f);
  pcactormove->SetJumpingVelocity (6.31f);

  printf("Loading Actor\n");
  if (ent->getType() == Entity::DoorEntity)
  {
    csRef<iMeshWrapper> doormesh = engine->FindMeshObject(*ent->getMesh());
    if (doormesh)
      pcmesh->SetMesh(doormesh);
  }
  else
  {
    vfs->ChDir("/cellib/objects/");
    pcmesh->SetMesh(*ent->getMesh(), "/peragro/meshes/all.xml");

    // Forcing the speed on the Cal3d mesh, so it will go in idle animation.
    csRef<iSpriteCal3DState> sprcal3d =
      SCF_QUERY_INTERFACE (pcmesh->GetMesh()->GetMeshObject(), iSpriteCal3DState);
    if (sprcal3d) sprcal3d->SetVelocity(0);
  }

  pl->CreatePropertyClass(entity, "pclinearmovement");
  pclinmove = CEL_QUERY_PROPCLASS_ENT(entity, iPcLinearMovement);

  csRef<iCelEntity> region = pl->FindEntity("World");
  if (region)
  {
    csRef<iPcRegion> pcregion = CEL_QUERY_PROPCLASS_ENT(region, iPcRegion);
    pcmesh->MoveMesh(pcregion->GetStartSector(), pcregion->GetStartPosition());
  }

  if (own_char_id == ent->getId())
  {
    printf("ptEntityManager:  Adding Entity '%s' as me\n", entity->GetName());
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
    printf("ptEntityManager: Adding Entity '%s'\n", entity->GetName());
  }

  if (ent->getType() != Entity::DoorEntity)
  {
  iSector* sector = engine->FindSector(*ent->getSector());
  pclinmove->InitCD(
    csVector3(0.5f,0.8f,0.5f),
    csVector3(0.5f,0.8f,0.5f),
    csVector3(0,0,0));
    csVector3 pos(ent->getPos()[0], ent->getPos()[1], ent->getPos()[2]);
    pclinmove->SetPosition(pos,0,sector);
  }

  pl->CreatePropertyClass(entity, "pcproperties");
  csRef<iPcProperties> pcprop = CEL_QUERY_PROPCLASS_ENT(entity, iPcProperties);
  pcprop->SetProperty("Entity Type", (long)ent->getType());
  pcprop->SetProperty("Entity ID", (long)ent->getId());
  pcprop->SetProperty("Entity Name", *ent->getName());
  if (ent->getType() == Entity::DoorEntity)
  {
    DoorEntity *door = (DoorEntity*)ent;
    pcprop->SetProperty("Door Open", door->getOpen());
    pcprop->SetProperty("Door Locked", door->getLocked());
    pl->CreatePropertyClass(entity, "pcquest");
    csRef<iPcQuest> pcquest = CEL_QUERY_PROPCLASS_ENT(entity, iPcQuest);

    celQuestParams parameters;

    pcquest->NewQuest("PropDoor",parameters);
    pcquest->GetQuest()->SwitchState("closed");
  }

  csVector4 skinColour(0,0,0,1);
  csVector4 decalColour(0,0,0,1);
  csVector4 hairColour(0,0,0,1);

  if (ent->getType() == Entity::PlayerEntity)
  {
    PcEntity* player = (PcEntity*) ent;
    Character* character = player->getCharacter();
    if (character)
    {
      unsigned char* skincolour = character->getSkinColour();
      skinColour.Set(skincolour[0], skincolour[1], skincolour[2], 1);

      unsigned char* decalcolour = character->getDecalColour();
      decalColour.Set(decalcolour[0], decalcolour[1], decalcolour[2], 1);

      unsigned char* haircolour = character->getHairColour();
      hairColour.Set(haircolour[0], haircolour[1], haircolour[2], 1);
    }
  }

  // Create shadervariables.
  csRef<iMeshWrapper> parent = pcmesh->GetMesh();
  SetMaskColor(parent, "skincolor", skinColour);
  SetMaskColor(parent, "decalcolor", decalColour);
  SetMaskColor(parent, "haircolor", hairColour);

  // Add to the entities list
  ptCelEntity ptent (ent->getId(), ent->getType(), *ent->getName(), entity);
  entities.Push (ptent);

}
