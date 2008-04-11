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

#include "client/entity/entitymanager.h"
#include "client/entity/movementmanager.h"

#include <iutil/objreg.h>

#include "client/event/eventmanager.h"
#include "client/reporter/reporter.h"

#include "client/data/sectordatamanager.h"
#include "client/data/sector.h"

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

      // Register listener for EntityMoveToEvent.
      PointerLibrary::getInstance()->getEventManager()->AddListener("entity.moveto", cb);

      vfs = csQueryRegistry<iVFS> (PointerLibrary::getInstance()->getObjectRegistry());
      if (!vfs)
      {
        Report(PT::Error, "Can't find the vfs!");
        return false;
      }

      return true;
    }

    void MovementManager::ProcessEvents()
    {
      while (!events.IsEmpty())
      {
        csRef<iEvent> ev = events.Get(0);
        PT::Events::EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
        if (evmgr->IsKindOf(ev->GetName(), "entity.moveto"))
          MoveToEntity(*ev);

        events.DeleteIndex(0);
      } // for
    }

    void MovementManager::Handle ()
    {
      ProcessEvents();

      moveToUpdate();
    }

    bool MovementManager::GetEntityEvents(iEvent& ev)
    {
      using namespace PT::Events;

      events.Push(&ev);

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

    void MovementManager::RemoveMoveTos(unsigned int id)
    {
      for (size_t i = 0; i < move_to_entity.GetSize(); i++)
      {
        MoveToData* m = move_to_entity.Get(i);

        if (m->entity_id == id)
        {
          move_to_entity.Delete(m);
          break;
        }
      }
    }

    bool MovementManager::MoveToEntity(iEvent& ev)
    {
      using namespace PT::Events;

      unsigned int id = EntityHelper::GetEntityID(&ev);

      // Remove any other moveTo actions for this entity
      RemoveMoveTos(id);

      Entity* entity = PointerLibrary::getInstance()->getEntityManager()->findPtEntById(id);
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

      csVector3 pos_ori = EntityHelper::GetVector3(&ev, "origin");
      csVector3 pos_dst = EntityHelper::GetVector3(&ev, "destination");

      float speed = 0.0f;
      ev.Retrieve("speed", speed);

      // Getting the real world position of our entity.
      csRef<iPcLinearMovement> pclinmove = CEL_QUERY_PROPCLASS_ENT(entity->GetCelEntity(), iPcLinearMovement);
      float cur_yrot;
      csVector3 cur_position;
      iSector* cur_sector;
      pclinmove->GetLastFullPosition(cur_position, cur_yrot, cur_sector);

      csVector3 vec (0,0,1);
      //float yrot_dst = GetAngle (pos_dst - cur_position, vec);

      cur_position.y = pos_dst.y;
      csVector3 direction = pos_dst - cur_position;
      float yrot_dst = atan2 (-direction.x, -direction.z);

      //Calculate speed based on actual and server distance.
      //float speed = ((pos_dst - cur_position).Norm() * entityMoveEv->speed )/
      //  (pos_ori - cur_position).Norm();

      moveTo->destination = pos_dst;
      moveTo->turn_speed = 2*PI; // 1 revolution per second
      moveTo->walk_speed = speed;
      moveTo->dest_angle = yrot_dst;
      moveTo->walk_duration = (pos_dst - cur_position).Norm() / moveTo->walk_speed;
      moveTo->elapsed_time = 0;
      moveTo->walking = false;
      moveTo->entity_id = id;


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

        Entity* entity = PointerLibrary::getInstance()->getEntityManager()->findPtEntById(moveTo->entity_id);
        if (entity)
        {
          if(entity->MoveTo(moveTo))
            move_to_entity.Delete(moveTo);
        }
      }
    }

  } // Entity namespace
} // PT namespace
