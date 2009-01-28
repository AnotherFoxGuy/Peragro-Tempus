/*
    Copyright (C) 2008 Development Team of Peragro Tempus

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

#include <cssysdef.h>
#include "components/entity/move/networkmove.h"

#include <iengine/engine.h>
#include <iengine/sector.h>
#include <iengine/movable.h>

#include <iengine/mesh.h>
#include <iutil/objreg.h>
#include <iutil/cfgmgr.h>
#include <iutil/virtclk.h>

#include <physicallayer/pl.h>
#include <physicallayer/propfact.h>
#include <physicallayer/propclas.h>
#include <propclass/mesh.h>
#include <propclass/linmove.h>
#include <propclass/actormove.h>

#include "common/event/eventmanager.h"
#include "common/event/entityevent.h"

#include "client/entity/entity.h"
#include "client/entity/player/playerentity.h"

#include "client/data/sector/sectordatamanager.h"
#include "client/data/sector/sector.h"

#include "common/reporter/reporter.h"
#include "client/pointer/pointer.h"

CS_IMPLEMENT_PLUGIN
PT_IMPLEMENT_COMPONENTFACTORY (NetworkMove, "peragro.entity.move.networkmove")

ComponentNetworkMove::ComponentNetworkMove(iObjectRegistry* object_reg) :
  scfImplementationType (this, object_reg)
{
  moveTo = 0;
}

ComponentNetworkMove::~ComponentNetworkMove()
{
}

bool ComponentNetworkMove::Initialize (PointerLibrary* pl,
                                       PT::Entity::Entity* ent)
{
  pointerlib = pl;
  entity = ent;

  using namespace PT::Events;
  using namespace PT::Entity;

  EventManager* evmgr = pointerlib->getEventManager();

  PT_REGISTER_LISTENER_ENTITY(ComponentNetworkMove, Move, "entity.move", true);
  PT_REGISTER_LISTENER_ENTITY(ComponentNetworkMove, Teleport, "entity.teleport", true);
  PT_REGISTER_LISTENER_ENTITY(ComponentNetworkMove, DrUpdate, "entity.drupdate", true);
  PT_REGISTER_LISTENER_ENTITY(ComponentNetworkMove, MoveTo, "entity.moveto", true);
  PT_REGISTER_LISTENER_ENTITY(ComponentNetworkMove, UpdateOptions, "interface.options.movement", false);
  PT_REGISTER_LISTENER_ENTITY(ComponentNetworkMove, DisableGravity, "world.loading", false);
  PT_REGISTER_LISTENER_ENTITY(ComponentNetworkMove, ResetGravity, "world.loaded", false);

  csRef<iConfigManager> app_cfg = csQueryRegistry<iConfigManager>
    (pointerlib->getObjectRegistry());
  localMovement = app_cfg->GetBool("Peragro.Movement.Local", false);

  return true;
} // end Initialize()

bool ComponentNetworkMove::UpdateOptions(iEvent& ev)
{
  csRef<iConfigManager> app_cfg = csQueryRegistry<iConfigManager>
    (pointerlib->getObjectRegistry());
  localMovement = app_cfg->GetBool("Peragro.Movement.Local", false);
  return true;
} // end UpdateOptions()

bool ComponentNetworkMove::Move(iEvent& ev)
{
  using namespace PT::Events;

  unsigned int id = entity->GetId();

  float walk = 0;
  float turn = 0;
  bool run = false;
  bool jump = false;
  bool local = false;
  csEventError error = csEventErrNone;


  error = ev.Retrieve("walkDirection", walk);
  error = ev.Retrieve("turnDirection", turn);
  error = ev.Retrieve("run", run);
  error = ev.Retrieve("jump", jump);
  if (ev.AttributeExists("local"))
    error = ev.Retrieve("local", local);

  if (error != csEventErrNone)
  {
    pointerlib->getReporter()->Report(PT::Error,
      "NetworkMove::Move(): Failed to retrieve an attribute for %d!", id);
    return false;
  }

  // If it's the player and local movement, do nothing.
  if (!local && localMovement &&
    entity->GetType() == Common::Entity::PlayerEntityType) return false;

  csRef<iCelEntity> celEntity = entity->GetCelEntity();
  if (!celEntity.IsValid()) return false;

  csRef<iPcActorMove> pcactormove =
    CEL_QUERY_PROPCLASS_ENT(celEntity, iPcActorMove);
  if (pcactormove.IsValid())
  {
    pcactormove->SetAnimationMapping(CEL_ANIM_IDLE, "idle");
    pcactormove->SetMovementSpeed(fabsf(walk));
    pcactormove->SetRunningSpeed(fabsf(walk));
    pcactormove->SetRotationSpeed(PI);

    pcactormove->RotateLeft(turn < 0.0f);
    pcactormove->RotateRight(turn > 0.0f);

    pcactormove->Forward(walk > 0.0f);
    pcactormove->Backward(walk < 0.0f);

    if (jump) pcactormove->Jump();
    else if (abs((int)walk) > 0) pcactormove->Run(run);
    else pcactormove->Run(false);
  }

  return false;
} // end Move()

bool ComponentNetworkMove::Teleport(iEvent& ev)
{
  using namespace PT::Events;

  unsigned int entityId = EntityHelper::GetEntityID(&ev);
  unsigned int sectorId = EntityHelper::GetSectorId(&ev);

  float rotation = 0.0f;
  ev.Retrieve("rotation", rotation);

  PT::Data::SectorDataManager* sectorDataMgr = pointerlib->getSectorDataManager();
  PT::Data::Sector* dataSector = sectorDataMgr->GetSectorById(sectorId);
  std::string sectorName = "Default_Sector";
  if (dataSector)
    sectorName = dataSector->GetName();

  pointerlib->getReporter()->Report(PT::Debug,
    "NetworkMove: Teleporting entity '%d' to %s(%d)",
    entityId, sectorName.c_str(), sectorId);

  entity->Teleport(EntityHelper::GetPosition(&ev), rotation, sectorName.c_str());

  return true;
} // end Teleport()

bool ComponentNetworkMove::DrUpdate(iEvent& ev)
{
  using namespace PT::Events;

  // Skip DR updates for the player. This only works with local movement, with
  // server controlled movement other clients will see your char jerk about.
  // @TODO I'm not sure this is a good idea,  along with
  // EntityManager::DrUpdateOwnEntity(), it puts the client totally in charge.
  if (localMovement &&
    entity->GetType() == Common::Entity::PlayerEntityType) return false;

//unsigned int entityId = EntityHelper::GetEntityID(&ev);
  unsigned int sectorId = EntityHelper::GetSectorId(&ev);

  csVector3 position = PtVector3(EntityHelper::GetVector3(&ev, "from"));

  float rotation = 0.0f;
  ev.Retrieve("rotation", rotation);

  PT::Data::SectorDataManager* sectorDataMgr = pointerlib->getSectorDataManager();
  std::string sectorName = sectorDataMgr->GetSectorById(sectorId)->GetName();

  csRef<iObjectRegistry> obj_reg = pointerlib->getObjectRegistry();
  csRef<iEngine> engine =  csQueryRegistry<iEngine> (obj_reg);

  csRef<iCelEntity> celEntity = entity->GetCelEntity();
  if (!celEntity.IsValid()) return false;

  csRef<iPcLinearMovement> pclinmove = CEL_QUERY_PROPCLASS_ENT(celEntity, iPcLinearMovement);
  if (pclinmove.IsValid())
  {
    bool onGround = true;
    float speed = 0, rot = 0, avel = 0;
    csVector3 pos, vel, wvel;

    if (pclinmove->GetAnchor()->GetMesh()->GetMovable()
      ->GetSectors()->GetCount() == 0) return false; // unloaded region

    iSector* sector = 0;
    pclinmove->GetDRData(onGround, speed, pos, rot, sector, vel, wvel, avel);

    sector = engine->FindSector(sectorName.c_str());
    ///@bug It seems that CEL interface has some issues. The below method,
    ///SetDRData seems not to take const references/pointers as arguments.
    csVector3 tempPos = position;
    float tempRot = rotation;
    pclinmove->SetDRData(onGround, speed, tempPos, tempRot, sector, vel,
      wvel, avel);

    entity->SetFullPosition(EntityHelper::GetPosition(&ev), rotation, sectorName);
  }

  return false;
} // end DrUpdate()

bool ComponentNetworkMove::MoveTo(iEvent& ev)
{
  using namespace PT::Events;

  csRef<iCelEntity> celEntity = entity->GetCelEntity();
  if (!celEntity.IsValid()) return false;

  if (moveTo) delete moveTo;
  moveTo = new MoveToData();

  csVector3 pos_ori = PtVector3(EntityHelper::GetVector3(&ev, "from"));
  csVector3 pos_dst = PtVector3(EntityHelper::GetVector3(&ev, "to"));

  float speed = 0.0f;
  ev.Retrieve("speed", speed);

  // Getting the real world position of our entity.
  csRef<iPcLinearMovement> pclinmove =
    CEL_QUERY_PROPCLASS_ENT(celEntity, iPcLinearMovement);
  float cur_yrot;
  csVector3 cur_position;
  iSector* cur_sector;
  pclinmove->GetLastFullPosition(cur_position, cur_yrot, cur_sector);

  cur_position.y = pos_dst.y;
  csVector3 direction = pos_dst - cur_position;
  float yrot_dst = atan2 (-direction.x, -direction.z);

  moveTo->destination = pos_dst;
  moveTo->turn_speed = 2*PI; // 1 revolution per second
  moveTo->walk_speed = speed;
  moveTo->dest_angle = yrot_dst;
  moveTo->walk_duration = (pos_dst - cur_position).Norm() / moveTo->walk_speed;
  moveTo->elapsed_time = 0;
  moveTo->walking = false;

  using namespace PT::Events;
  EventManager* evmgr = pointerlib->getEventManager();

  // Register listener for frame.
  if (!cbMoveToUpdate.IsValid())
  {
    cbMoveToUpdate.AttachNew(new EventHandler<ComponentNetworkMove>
      (&ComponentNetworkMove::MoveToUpdate, this));
  }
  evmgr->AddListener("crystalspace.frame", cbMoveToUpdate);

  return true;
} // end MoveTo()

bool ComponentNetworkMove::MoveToUpdate(iEvent& ev)
{
  csRef<iCelEntity> celEntity = entity->GetCelEntity();
  if (!celEntity.IsValid())
  {
    RemoveMoveToUpdate();
    return false;
  }

  csRef<iObjectRegistry> obj_reg = pointerlib->getObjectRegistry();
  csRef<iEngine> engine =  csQueryRegistry<iEngine> (obj_reg);

  csRef<iVirtualClock> vc = csQueryRegistry<iVirtualClock> (obj_reg);
  csTicks ticks = vc->GetElapsedTicks ();

  if (!ticks)
  {
    RemoveMoveToUpdate();
    return false;
  }

  float elapsed = ticks/1000.0;

  csRef<iPcLinearMovement> pclinmove =
    CEL_QUERY_PROPCLASS_ENT(celEntity, iPcLinearMovement);
  csRef<iPcActorMove> pcactormove =
    CEL_QUERY_PROPCLASS_ENT(celEntity, iPcActorMove);

  if (pclinmove.IsValid() && pcactormove.IsValid())
  {
    csVector3 angular_vel;

    pclinmove->GetAngularVelocity(angular_vel);
    pcactormove->SetAnimationMapping(CEL_ANIM_IDLE, "idle");

    if (moveTo->elapsed_time == 0 && !moveTo->walking)
    {
      // \todo: this is still buggy. fix it!
      //pcactormove->SetRotationSpeed(moveTo->turn_speed);
      //pcactormove->RotateTo(moveTo->dest_angle);

      // Workaround:
      csRef<iPcMesh> pcmesh = CEL_QUERY_PROPCLASS_ENT(celEntity, iPcMesh);
      if (pcmesh.IsValid() && pcmesh->GetMesh ())
      {
        csMatrix3 matrix = (csMatrix3) csYRotMatrix3 (moveTo->dest_angle);
        pcmesh->GetMesh ()->GetMovable ()->GetTransform ().SetO2T (matrix);
        pcmesh->GetMesh ()->GetMovable ()->UpdateMove ();
      }

    }
    else if (angular_vel.IsZero() && !moveTo->walking)
    {
      pcactormove->SetMovementSpeed(moveTo->walk_speed);
      pcactormove->Forward(true);
      moveTo->walking = true;
      moveTo->elapsed_time = 0;
    }
    else if (moveTo->elapsed_time >= moveTo->walk_duration &&
      moveTo->walking)
    {
      pcactormove->Forward(false);
    }

    if (moveTo->walking)
    {
      // Update current position.
      entity->SetPosition(pclinmove->GetFullPosition());
      entity->SetRotation(pclinmove->GetYRotation());

      if (moveTo->elapsed_time >= moveTo->walk_duration)
      {
        // Arrived at destination. Return true for deletion.
        pcactormove->Forward(false);

        entity->SetPosition(moveTo->destination);
        entity->SetRotation(moveTo->dest_angle);

        RemoveMoveToUpdate();
        return false;
      }
    }

    moveTo->elapsed_time += elapsed;

    return false;
  }

  RemoveMoveToUpdate();

  return false;

} // end MoveToUpdate()

bool ComponentNetworkMove::RemoveMoveToUpdate()
{
  using namespace PT::Events;

  EventManager* evmgr = pointerlib->getEventManager();
  evmgr->RemoveListener(cbMoveToUpdate);

  return true;
} // end RemoveMoveToUpdate()

bool ComponentNetworkMove::DisableGravity(iEvent& ev)
{
  csRef<iPcLinearMovement> pclinmove =
    CEL_QUERY_PROPCLASS_ENT(entity->GetCelEntity(), iPcLinearMovement);
  pclinmove->ClearWorldVelocity();
  pclinmove->SetGravity(0.0f);

  return false;
} // end WorldLoading()

bool ComponentNetworkMove::ResetGravity(iEvent& ev)
{
  csRef<iPcLinearMovement> pclinmove =
    CEL_QUERY_PROPCLASS_ENT(entity->GetCelEntity(), iPcLinearMovement);
  pclinmove->ResetGravity();

  return false;
} // end WorldLoaded()
