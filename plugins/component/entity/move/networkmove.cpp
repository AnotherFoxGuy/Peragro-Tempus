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
#include "plugins/component/entity/move/networkmove.h"

#include <iutil/objreg.h>
#include <iutil/cfgmgr.h>

#include "client/event/eventmanager.h"
#include "client/event/entityevent.h"

//#include "client/entity/entitymanager.h"
#include "client/entity/entity.h"
#include "client/entity/player/playerentity.h"

#include "client/data/sectordatamanager.h"
#include "client/data/sector.h"

#include "client/reporter/reporter.h"
#include "client/pointer/pointer.h"

CS_IMPLEMENT_PLUGIN
IMPLEMENT_COMPONENTFACTORY (NetworkMove, "peragro.entity.move.networkmove")

ComponentNetworkMove::ComponentNetworkMove(iObjectRegistry* object_reg) :
	scfImplementationType (this, object_reg)
{
}

ComponentNetworkMove::~ComponentNetworkMove()
{
}

bool ComponentNetworkMove::Initialize (PointerLibrary* pl, PT::Entity::Entity* ent)
{
  pointerlib = pl;
  entity = ent;

  using namespace PT::Events;
  using namespace PT::Entity;

  EventManager* evmgr = pointerlib->getEventManager();

  // Register listener for move.
  csRef<EventHandlerCallback> cbMove;
  cbMove.AttachNew(new EventHandler<ComponentNetworkMove>(&ComponentNetworkMove::Move, this));
  evmgr->AddListener(EntityHelper::MakeEntitySpecific("entity.move", entity->GetId()), cbMove);
  eventHandlers.Push(cbMove);

  // Register listener for teleport.
  csRef<EventHandlerCallback> cbTeleport;
  cbTeleport.AttachNew(new EventHandler<ComponentNetworkMove>(&ComponentNetworkMove::Teleport, this));
  evmgr->AddListener(EntityHelper::MakeEntitySpecific("entity.teleport", entity->GetId()), cbTeleport);
  eventHandlers.Push(cbTeleport);

  // Register listener for drupdate.
  csRef<EventHandlerCallback> cbDRUpdate;
  cbDRUpdate.AttachNew(new EventHandler<ComponentNetworkMove>(&ComponentNetworkMove::DrUpdate, this));
  evmgr->AddListener(EntityHelper::MakeEntitySpecific("entity.drupdate", entity->GetId()), cbDRUpdate);
  eventHandlers.Push(cbDRUpdate);

  // Register listener for InterfaceOptionsEvent.
  csRef<EventHandlerCallback> cbUpdate;
  cbUpdate.AttachNew(new EventHandler<ComponentNetworkMove>(&ComponentNetworkMove::UpdateOptions, this));
  evmgr->AddListener("interface.options", cbUpdate);
  eventHandlers.Push(cbUpdate);

  csRef<iConfigManager> app_cfg = csQueryRegistry<iConfigManager> (pointerlib->getObjectRegistry());
  local_movement = app_cfg->GetBool("Client.local_movement", false);

  return true;
} // end Initialize()

bool ComponentNetworkMove::UpdateOptions(iEvent& ev)
{
  csRef<iConfigManager> app_cfg = csQueryRegistry<iConfigManager> (pointerlib->getObjectRegistry());
  local_movement = app_cfg->GetBool("Client.local_movement", false);
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
  error = ev.Retrieve("local", local);

  if (error != csEventErrNone)
  {
    pointerlib->getReporter()->Report(PT::Error, "NetworkMove::Move(): Failed to retrieve an attribute for %d!", id);
    return false;
  }

  // If it's the player and local movement, do nothing.
  PT::Entity::PlayerEntity* player = static_cast<PT::Entity::PlayerEntity*>(entity);
  if (!local && local_movement && player){ return false; }

  csRef<iCelEntity> celEntity = entity->GetCelEntity();
  if(!celEntity.IsValid()) return false;

  csRef<iPcActorMove> pcactormove = CEL_QUERY_PROPCLASS_ENT(celEntity, iPcActorMove);
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
  csVector3 pos = EntityHelper::GetPosition(&ev);

  float rotation = 0.0f;
  ev.Retrieve("rotation", rotation);

  PT::Data::SectorDataManager* sectorDataMgr = pointerlib->getSectorDataManager();
  PT::Data::Sector* dataSector = sectorDataMgr->GetSectorById(sectorId);
  std::string sectorName = "Default_Sector";
  if (dataSector)
    sectorName = dataSector->GetName();

  pointerlib->getReporter()->Report(PT::Debug, "MovementManager: Teleporting entity '%d' to %s(%d)", entityId, sectorName.c_str(), sectorId);

  entity->Teleport(pos, rotation, sectorName.c_str());

  return true;
} // end Teleport()

bool ComponentNetworkMove::DrUpdate(iEvent& ev)
{
  using namespace PT::Events;

  // If it's the player, do nothing.
  PT::Entity::PlayerEntity* player = static_cast<PT::Entity::PlayerEntity*>(entity);
  if (player) return false;

  unsigned int entityId = EntityHelper::GetEntityID(&ev);
  unsigned int sectorId = EntityHelper::GetSectorId(&ev);
  csVector3 position = EntityHelper::GetPosition(&ev);

  float rotation = 0.0f;
  ev.Retrieve("rotation", rotation);

  PT::Data::SectorDataManager* sectorDataMgr = pointerlib->getSectorDataManager();
  std::string sectorName = sectorDataMgr->GetSectorById(sectorId)->GetName();

  csRef<iObjectRegistry> obj_reg = pointerlib->getObjectRegistry();
  csRef<iEngine> engine =  csQueryRegistry<iEngine> (obj_reg);

  csRef<iCelEntity> celEntity = entity->GetCelEntity();
  if(!celEntity.IsValid()) return false; 

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

    if (vel.Norm() > 0 || avel > 0) return false; // Don't update while moving!

    sector = engine->FindSector(sectorName.c_str());
    ///@bug It seems that CEL interface has some issues. The below method,
    ///SetDRData seems not to take const references/pointers as arguments.
    csVector3 tempPos = position;
    float tempRot = rotation;
    pclinmove->SetDRData(onGround, speed, tempPos, tempRot, sector, vel,
      wvel, avel);

    entity->SetFullPosition(position, rotation, sectorName);
  }

  return false;
} // end DrUpdateEntity()
