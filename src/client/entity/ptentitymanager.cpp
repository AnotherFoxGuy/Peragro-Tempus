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

#include "client/reporter/reporter.h"

namespace PT
{
	namespace Entity
	{

		EntityManager::EntityManager (iObjectRegistry* obj_reg)
		{
			this->obj_reg = obj_reg;

			movementManager = new MovementManager(obj_reg);

			engine =  csQueryRegistry<iEngine> (obj_reg);

			vfs =  csQueryRegistry<iVFS> (obj_reg);

			stringset = csQueryRegistryTagInterface<iStringSet> (obj_reg,
				"crystalspace.shared.stringset");

			vc =  csQueryRegistry<iVirtualClock> (obj_reg);

			pl =  csQueryRegistry<iCelPlLayer> (obj_reg);

			ownent = 0;
		}

		EntityManager::~EntityManager ()
		{
      delete movementManager;
		}

		bool EntityManager::Initialize ()
		{
			world_loaded = false;
			playing = false;
			own_char_id = 0;

			movementManager->Initialize();

			using namespace PT::Events;

			EventHandler<EntityManager>* cb = new EventHandler<EntityManager>(&EntityManager::GetEntityEvents, this);
			// Register listener for EntityAddEvent.
			PointerLibrary::getInstance()->getEventManager()->AddListener("EntityAddEvent", cb);
			// Register listener for EntityRemoveEvent.
			PointerLibrary::getInstance()->getEventManager()->AddListener("EntityRemoveEvent", cb);
			// Register listener for EntityEquipEvent.
			PointerLibrary::getInstance()->getEventManager()->AddListener("EntityEquipEvent", cb);
			// Register listener for EntityEquipEvent.
			PointerLibrary::getInstance()->getEventManager()->AddListener("EntityMountEvent", cb);

			// Register listener for EntityEquipEvent.
			PointerLibrary::getInstance()->getEventManager()->AddListener("StatePlayEvent", cb);

			return true;
		}

		void EntityManager::ProcessEvents()
		{
			using namespace PT::Events;

			for (size_t i = 0; i < events.GetSize(); i++)
			{
				Eventp ev = events.Pop();
				if (ev->GetEventID().compare("EntityAddEvent") == 0)
					AddEntity(ev);
				else if (ev->GetEventID().compare("EntityRemoveEvent") == 0)
					RemoveEntity(ev);
				else if (ev->GetEventID().compare("EntityEquipEvent") == 0)
					Equip(ev);
				else if (ev->GetEventID().compare("EntityMountEvent") == 0)
					Mount(ev);

				else if (ev->GetEventID().compare("StatePlayEvent") == 0)
					SetOwnId(ev);
			} // for
		}

		void EntityManager::Handle ()
		{
			if (!world_loaded || !playing) return;

			movementManager->Handle();

			ProcessEvents();
		}

		PtEntity* EntityManager::findPtEntById(int id)
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

		iCelEntity* EntityManager::findCelEntById(int id)
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

		bool EntityManager::GetEntityEvents(PT::Events::Eventp ev)
		{
			using namespace PT::Events;

			Eventp evcopy(ev);
			events.Push(evcopy);

			return true;
		}

		bool EntityManager::SetOwnId(PT::Events::Eventp ev)
		{
			using namespace PT::Events;

			StatePlayEvent* playEv = GetStateEvent<StatePlayEvent*>(ev);
			if (!playEv) return false;

			own_char_id = playEv->ownEntityId;

			return true;
		}

		bool EntityManager::AddEntity(PT::Events::Eventp ev)
		{
			using namespace PT::Events;

			EntityAddEvent* entityAddEv = GetEntityEvent<EntityAddEvent*>(ev);
			if (!entityAddEv) return false;

			if ( findPtEntById(entityAddEv->entityId) )
			{
				Report(PT::Warning, "AddEntity: Skipping already existing entity '%s(%d)'", entityAddEv->entityName.c_str(), entityAddEv->entityId);
				return true;
			}

			PtEntity* entity; 
			if (entityAddEv->entityType == EntityEvent::PlayerEntity)
			{
				entity = new PtPcEntity(); 
				// Add equipment.
				/*std::vector<EntityAddEvent::SlotAndItem>::const_iterator it;
				for(it = entityAddEv->equipment.begin(); it != entityAddEv->equipment.end(); ++it)
					((PtPcEntity*)entity)->GetEquipment()->Equip(it->slotId, it->itemId);*/
				for(size_t i = 0; i < entityAddEv->equipment.GetSize(); i++)
					((PtPcEntity*)entity)->GetEquipment()->Equip(entityAddEv->equipment.Get(i).slotId, entityAddEv->equipment.Get(i).itemId);
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
				Report(PT::Error, "Invalid Entity type: %d !", entityAddEv->entityType);
				return true;
			}

			entity->SetId(entityAddEv->entityId);
			entity->SetName(entityAddEv->entityName.c_str());
			entity->SetMeshName(entityAddEv->meshName.c_str());
			entity->SetPosition(entityAddEv->position);
			entity->SetSectorName(entityAddEv->sectorName.c_str());

			if (own_char_id == entity->GetId())
				((PtPcEntity*)entity)->SetOwnEntity(true);

			entity->Create();

			// It's our player.
			if (own_char_id == entity->GetId())
			{
				Report(PT::Notify, "Adding Entity '%s(%d)' at %s as me.", entity->GetName().GetData(), entity->GetId(), entityAddEv->position.Description().GetData());

				// Set up own player cam and entity for faster access.
				csRef<iPcDefaultCamera> pccamera = CEL_QUERY_PROPCLASS_ENT(entity->GetCelEntity(), iPcDefaultCamera);
				if (pccamera.IsValid())
				{
					pccamera->SetAutoDraw(false);
					pccamera->SetMode(iPcDefaultCamera::thirdperson, true);
					pccamera->SetPitch(-0.18f);
				}
				else
					Report(PT::Error, "Failed to get PcDefaultCamera for %s!(%d)", entity->GetName().GetData(), entity->GetId());

				owncam = pccamera;
				ownent = entity;
				owncelent = entity->GetCelEntity();
				ownname = entity->GetName();
			}
			else
				Report(PT::Notify, "Adding Entity '%s(%d)' at %s.", entity->GetName().GetData(), entity->GetId(), entityAddEv->position.Description().GetData());

			// Add our entity to the list.
			entities.Push(entity);

			return true;
		}

		bool EntityManager::RemoveEntity(PT::Events::Eventp ev)
		{
			using namespace PT::Events;

			EntityRemoveEvent* entityRemoveEv = GetEntityEvent<EntityRemoveEvent*>(ev);
			if (!entityRemoveEv) return false;

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

		bool EntityManager::Equip(PT::Events::Eventp ev)
		{
			using namespace PT::Events;

			EntityEquipEvent* entityEquipEv = GetEntityEvent<EntityEquipEvent*>(ev);
			if (!entityEquipEv) return false;

			Report(PT::Debug, "Equip for '%d': item %d in slot %d", entityEquipEv->entityId, entityEquipEv->itemId, entityEquipEv->slotId);

			PtEntity* entity = findPtEntById(entityEquipEv->entityId);
			if (entity)
			{
				if (entity->GetType() == PtEntity::PlayerEntity)
				{
					if (!entityEquipEv->itemId == 0)
						((PtPcEntity*) entity)->GetEquipment()->Equip(entityEquipEv->slotId, entityEquipEv->itemId);
					else
						((PtPcEntity*) entity)->GetEquipment()->UnEquip(entityEquipEv->slotId);
				}
			}

			return true;
		}

		bool EntityManager::Mount(PT::Events::Eventp ev)
		{
			using namespace PT::Events;

			EntityMountEvent* entityMountEv = GetEntityEvent<EntityMountEvent*>(ev);
			if (!entityMountEv) return false;

			PtEntity* entity = findPtEntById(entityMountEv->entityId);
			PtEntity* mount = findPtEntById(entityMountEv->mountId);
			if (entity && mount)
			{
				if (entity->GetType() == PtEntity::PlayerEntity && mount->GetType() == PtEntity::MountEntity)
				{
					PtMountEntity* m = static_cast<PtMountEntity*>(mount);
					if (entityMountEv->mount) 
						m->Mount(entity);
					else
						m->UnMount(entity);
				}
			}


			return true;
		}

		void EntityManager::delAllEntities()
		{
			ownent = 0;
			for (size_t i = 0; i < entities.GetSize(); i++)
			{
				pl->RemoveEntity(entities[i]->GetCelEntity());
				entities.DeleteIndex(i);
			}
		}

		void EntityManager::DrUpdateOwnEntity()
		{
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

	} // Entity namespace 
} // PT namespace 
