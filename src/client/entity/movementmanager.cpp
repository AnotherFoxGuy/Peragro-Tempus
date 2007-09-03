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
#include "client/entity/movementmanager.h"

#include <iutil/objreg.h>

#include "client/event/eventmanager.h"
#include "client/reporter/reporter.h"

#include "client/data/sector/sectormanager.h"

namespace PT
{
  namespace Entity
  {

    MovementManager::MovementManager (iObjectRegistry* obj_reg)
    {
      this->obj_reg = obj_reg;

      engine =  csQueryRegistry<iEngine> (obj_reg);
      vc =  csQueryRegistry<iVirtualClock> (obj_reg);
    }

    MovementManager::~MovementManager ()
    {
    }

    bool MovementManager::Initialize ()
    {
      using namespace PT::Events;

      EventHandler<MovementManager>* cb = new EventHandler<MovementManager>(&MovementManager::GetEntityEvents, this);

      // Register listener for EntityMoveEvent.
      PointerLibrary::getInstance()->getEventManager()->AddListener("EntityMoveEvent", cb);
      // Register listener for EntityMoveToEvent.
      PointerLibrary::getInstance()->getEventManager()->AddListener("EntityMoveToEvent", cb);
      // Register listener for EntityTeleportEvent.
      PointerLibrary::getInstance()->getEventManager()->AddListener("EntityTeleportEvent", cb);
      // Register listener for EntityDrUpdateEvent.
      PointerLibrary::getInstance()->getEventManager()->AddListener("EntityDrUpdateEvent", cb);
      // Register listener for EntityPcPropUpdateEvent.
      PointerLibrary::getInstance()->getEventManager()->AddListener("EntityPcPropUpdateEvent", cb);

      return true;
    }

    void MovementManager::ProcessEvents()
    {
      using namespace PT::Events;

      for (size_t i = 0; i < events.GetSize(); i++)
      {
        Eventp ev = events.Pop();
        if (ev->GetEventID().compare("EntityMoveEvent") == 0)
          MoveEntity(ev);
        else if (ev->GetEventID().compare("EntityMoveToEvent") == 0)
          MoveToEntity(ev);
        else if (ev->GetEventID().compare("EntityTeleportEvent") == 0)
          TeleportEntity(ev);
        else if (ev->GetEventID().compare("EntityDrUpdateEvent") == 0)
          DrUpdateEntity(ev);
        else if (ev->GetEventID().compare("EntityPcPropUpdateEvent") == 0)
          UpdatePcProp(ev);

      } // for
    }

    void MovementManager::Handle ()
    {
      ProcessEvents();

      moveToUpdate();
    }

    bool MovementManager::GetEntityEvents(PT::Events::Eventp ev)
    {
      using namespace PT::Events;

      Eventp evcopy(ev);
      events.Push(evcopy);

      return true;
    }

    float MovementManager::GetAngle (const csVector3& v1, const csVector3& v2)
    {
      float len = sqrt (csSquaredDist::PointPoint (v1, v2));
      float angle = acos ((v2.x-v1.x) / len);
      if ((v2.z-v1.z) > 0) angle = 2*PI - angle;
      angle += PI / 2.0f;
      if (angle > 2*PI) angle -= 2*PI;
      return angle;
    }

    bool MovementManager::MoveEntity(PT::Events::Eventp ev)
    {
      using namespace PT::Events;

      EntityMoveEvent* entityMoveEv = GetEntityEvent<EntityMoveEvent*>(ev);
      if (!entityMoveEv) return false;

      int id = entityMoveEv->entityId;

      PtEntity* entity = PointerLibrary::getInstance()->getEntityManager()->findPtEntById(id);
      if (!entity)
      {
        Report(PT::Error, "MoveEntity: Couldn't find entity with ID %d!", id);
        return true;
      }

      MovementData* movement	= new MovementData();
      movement->entity_id			= entityMoveEv->entityId;
      movement->walk					= entityMoveEv->walkDirection;
      movement->turn					= entityMoveEv->turnDirection;
      movement->run           = entityMoveEv->run;

      entity->Move(movement);
      delete movement;

      return true;
    }

    bool MovementManager::MoveToEntity(PT::Events::Eventp ev)
    {
      using namespace PT::Events;

      EntityMoveToEvent* entityMoveEv = GetEntityEvent<EntityMoveToEvent*>(ev);
      if (!entityMoveEv) return false;

      unsigned int id = entityMoveEv->entityId;

      // Remove any other moveTo actions for this entity
      for (size_t i = 0; i < move_to_entity.GetSize(); i++)
      {
        MoveToData* m = move_to_entity.Get(i);

        if (m->entity_id == id)
        {
          move_to_entity.Delete(m);
          break;
        }
      }

      PtEntity* entity = PointerLibrary::getInstance()->getEntityManager()->findPtEntById(id);
      if (!entity)
      {
        Report(PT::Error, "MoveToEntity: Couldn't find entity with ID %d!", id);
        return true;
      }
      if(!entity->GetCelEntity()) 
      {
        Report(PT::Error, "MoveToEntity: %d: No CelEntity!", id);
        return true;
      }

      MoveToData* moveTo = new MoveToData();

      csVector3 pos_ori = entityMoveEv->origin;
      csVector3 pos_dst = entityMoveEv->destination;

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

      moveTo->turn_speed			= 2*PI; // 1 revolution per second
      moveTo->walk_speed			= entityMoveEv->speed;
      moveTo->dest_angle			= yrot_dst;
      moveTo->walk_duration		= (pos_dst - cur_position).Norm() / moveTo->walk_speed;
      moveTo->elapsed_time		= 0;
      moveTo->walking					= false;
      moveTo->entity_id				= id;


      entity->MoveTo(moveTo);
      move_to_entity.Push(moveTo);

      return true;
    }

    void MovementManager::moveToUpdate()
    {
      if (!move_to_entity.GetSize()) return;

      for (size_t i = 0; i < move_to_entity.GetSize(); i++)
      {
        MoveToData* moveTo = move_to_entity.Get(i);

        PtEntity* entity = PointerLibrary::getInstance()->getEntityManager()->findPtEntById(moveTo->entity_id);
        if (entity)
        {
          if(entity->MoveTo(moveTo))
            move_to_entity.Delete(moveTo);
        }
      }
    }

    bool MovementManager::TeleportEntity(PT::Events::Eventp ev)
    {
      using namespace PT::Events;

      EntityTeleportEvent* entityMoveEv = GetEntityEvent<EntityTeleportEvent*>(ev);
      if (!entityMoveEv) return false;

      unsigned int entityId = entityMoveEv->entityId;
      unsigned int sectorId = entityMoveEv->sectorId;

      PT::Data::SectorManager* sectormgr = PointerLibrary::getInstance()->getSectorManager();
      std::string sectorName = sectormgr->GetSectorName(sectorId);

      PtEntity* entity = PointerLibrary::getInstance()->getEntityManager()->findPtEntById(entityId);
      if (!entity)
      {
        Report(PT::Error, "MovementManager: Couldn't find entity with ID %d!", entityId);
        return true;
      }

      Report(PT::Debug, "MovementManager: Teleporting entity '%d'", entityId);

      entity->Teleport(entityMoveEv->position, sectorName.c_str());

      return true;
    }

    bool MovementManager::DrUpdateEntity(PT::Events::Eventp ev)
    {
      using namespace PT::Events;

      EntityDrUpdateEvent* entityMoveEv = GetEntityEvent<EntityDrUpdateEvent*>(ev);
      if (!entityMoveEv) return false;

      unsigned int id = entityMoveEv->entityId;
      unsigned int sectorId = entityMoveEv->sectorId;

      PT::Data::SectorManager* sectormgr = PointerLibrary::getInstance()->getSectorManager();
      std::string sectorName = sectormgr->GetSectorName(sectorId);

      PtEntity* entity = PointerLibrary::getInstance()->getEntityManager()->findPtEntById(id);
      if (!entity)
      {
        Report(PT::Error, "DrUpdateEntity: Couldn't find entity with ID %d!", id);
        return true;
      }

      DrUpdateData* drupdate	= new DrUpdateData();
      drupdate->entity_id	= entityMoveEv->entityId;
      drupdate->pos		= entityMoveEv->position;
      drupdate->rot		= entityMoveEv->rotation;
      drupdate->sector		= sectorName.c_str();

      entity->DrUpdate(drupdate);
      delete drupdate;

      return true;
    }

    bool MovementManager::UpdatePcProp(PT::Events::Eventp ev)
    {
      using namespace PT::Events;

      EntityPcPropUpdateEvent* entityMoveEv = GetEntityEvent<EntityPcPropUpdateEvent*>(ev);
      if (!entityMoveEv) return false;

      int id = entityMoveEv->entityId;

      PtEntity* entity = PointerLibrary::getInstance()->getEntityManager()->findPtEntById(id);
      if (!entity)
      {
        Report(PT::Error, "UpdatePcProp: Couldn't find entity with ID %d!\n", id);
        return true;
      }

      UpdatePcPropData* updatePcprop  = new UpdatePcPropData();
      updatePcprop->entity_id	      = entityMoveEv->entityId;
      updatePcprop->pcprop	      = entityMoveEv->pcprop.c_str();
      updatePcprop->value	      = entityMoveEv->celdata;

      entity->UpdatePcProp(updatePcprop);
      delete updatePcprop;

      return true;
    }


  } // Entity namespace 
} // PT namespace 
