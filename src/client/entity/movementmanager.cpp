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
					MoveEntity(ev);
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

			EntityEvent* entityEv = static_cast<EntityEvent*> (ev.px);
			if (!entityEv)
			{
				printf("E: Not an Entity event!\n");
				return false;
			}
			EntityMoveEvent* entityMoveEv = static_cast<EntityMoveEvent*> (entityEv);
			if (!entityMoveEv)
			{
				printf("E: Not an EntityMoveEvent event!\n");
				return false;
			}

			int id = entityMoveEv->entityId;

			PtEntity* entity = PointerLibrary::getInstance()->getEntityManager()->findPtEntById(id);
			if (!entity)
			{
				printf("E: Couldn't find entity with ID %d!\n", id);
				return true;
			}

			MovementData* movement	= new MovementData();
			movement->entity_id			= entityMoveEv->entityId;
			movement->walk					= entityMoveEv->walkDirection;
			movement->turn					= entityMoveEv->turnDirection;

			entity->Move(movement);
			delete movement;

			return true;
		}

		bool MovementManager::MoveToEntity(PT::Events::Eventp ev)
		{
			using namespace PT::Events;

			EntityEvent* entityEv = static_cast<EntityEvent*> (ev.px);
			if (!entityEv)
			{
				printf("E: Not an Entity event!\n");
				return false;
			}
			EntityMoveToEvent* entityMoveEv = static_cast<EntityMoveToEvent*> (entityEv);
			if (!entityMoveEv)
			{
				printf("E: Not an EntityMoveToEvent event!\n");
				return false;
			}

			int id = entityMoveEv->entityId;

			PtEntity* entity = PointerLibrary::getInstance()->getEntityManager()->findPtEntById(id);
			if (!entity)
			{
				printf("E: Couldn't find entity with ID %d!\n", id);
				return true;
			}
			if(!entity->GetCelEntity()) 
			{
				printf("E: moveToEntity %d: No CelEntity!\n", id);
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

			// Remove any other moveTo actions for this entity
			for (size_t i = 0; i < move_to_entity.GetSize(); i++)
			{
				MoveToData* m = move_to_entity.Get(i);

				if (m->entity_id == moveTo->entity_id)
				{
					move_to_entity.Delete(m);
					break;
				}
			}

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

			EntityEvent* entityEv = static_cast<EntityEvent*> (ev.px);
			if (!entityEv)
			{
				printf("E: Not an Entity event!\n");
				return false;
			}
			EntityTeleportEvent* entityMoveEv = static_cast<EntityTeleportEvent*> (entityEv);
			if (!entityMoveEv)
			{
				printf("E: Not an EntityTeleportEvent event!\n");
				return false;
			}

			int id = entityMoveEv->entityId;

			PtEntity* entity = PointerLibrary::getInstance()->getEntityManager()->findPtEntById(id);
			if (!entity)
			{
				printf("E: Couldn't find entity with ID %d!\n", id);
				return true;
			}

			printf("I: Teleporting entity '%d'\n", id);

			entity->Teleport(entityMoveEv->position, entityMoveEv->sectorName.c_str());

			return true;
		}

		bool MovementManager::DrUpdateEntity(PT::Events::Eventp ev)
		{
			using namespace PT::Events;

			EntityEvent* entityEv = static_cast<EntityEvent*> (ev.px);
			if (!entityEv)
			{
				printf("E: Not an Entity event!\n");
				return false;
			}
			EntityDrUpdateEvent* entityMoveEv = static_cast<EntityDrUpdateEvent*> (entityEv);
			if (!entityMoveEv)
			{
				printf("E: Not an EntityTeleportEvent event!\n");
				return false;
			}

			int id = entityMoveEv->entityId;

			PtEntity* entity = PointerLibrary::getInstance()->getEntityManager()->findPtEntById(id);
			if (!entity)
			{
				printf("E: Couldn't find entity with ID %d!\n", id);
				return true;
			}

			DrUpdateData* drupdate	= new DrUpdateData();
			drupdate->entity_id			= entityMoveEv->entityId;
			drupdate->pos						= entityMoveEv->position;
			drupdate->rot						= entityMoveEv->rotation;
			drupdate->sector				= entityMoveEv->sectorName.c_str();

			entity->DrUpdate(drupdate);
			delete drupdate;

			return true;
		}

		bool MovementManager::UpdatePcProp(PT::Events::Eventp ev)
		{
			using namespace PT::Events;

			EntityEvent* entityEv = static_cast<EntityEvent*> (ev.px);
			if (!entityEv)
			{
				printf("E: Not an Entity event!\n");
				return false;
			}
			EntityPcPropUpdateEvent* entityMoveEv = static_cast<EntityPcPropUpdateEvent*> (entityEv);
			if (!entityMoveEv)
			{
				printf("E: Not an EntityTeleportEvent event!\n");
				return false;
			}

			int id = entityMoveEv->entityId;

			PtEntity* entity = PointerLibrary::getInstance()->getEntityManager()->findPtEntById(id);
			if (!entity)
			{
				printf("E: Couldn't find entity with ID %d!\n", id);
				return true;
			}

			UpdatePcPropData* updatePcprop	= new UpdatePcPropData();
			updatePcprop->entity_id					= entityMoveEv->entityId;
			updatePcprop->pcprop						= entityMoveEv->pcprop.c_str();
			updatePcprop->value							= entityMoveEv->celdata;

			entity->UpdatePcProp(updatePcprop);
			delete updatePcprop;

			return true;
		}


	} // Entity namespace 
} // PT namespace 
