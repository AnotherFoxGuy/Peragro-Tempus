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
#include "common/data/sectordatamanager.h"
#include "common/data/sector.h"

#include <iutil/objreg.h>
#include <imap/loader.h>

#include "client/network/network.h"

#include "common/network/netmessage.h"

#include "client/event/eventmanager.h"

#include "client/reporter/reporter.h"

#include "client/cursor.h"

namespace PT
{
  namespace Entity
  {

    EntityManager::EntityManager ()
    {
      this->obj_reg = PointerLibrary::getInstance()->getObjectRegistry();

      movementManager = new MovementManager(obj_reg);

      engine =  csQueryRegistry<iEngine> (obj_reg);

      vfs =  csQueryRegistry<iVFS> (obj_reg);

      stringset = csQueryRegistryTagInterface<iStringSet> (obj_reg,
        "crystalspace.shared.stringset");

      vc =  csQueryRegistry<iVirtualClock> (obj_reg);

      pl =  csQueryRegistry<iCelPlLayer> (obj_reg);

    }

    EntityManager::~EntityManager ()
    {
      delete movementManager;
    }

    bool EntityManager::Initialize ()
    {
      world_loaded = false;
      playing = false;
      playerId = 0;

      movementManager->Initialize();

      // Create a default sector for entities to be added in.
      engine->CreateSector("Default_Sector");

      using namespace PT::Events;

      EventHandler<EntityManager>* cb = new EventHandler<EntityManager>(&EntityManager::GetEntityEvents, this);
      // Register listener for EntityAddEvent.
      PointerLibrary::getInstance()->getEventManager()->AddListener("entity.add", cb);
      // Register listener for EntityRemoveEvent.
      PointerLibrary::getInstance()->getEventManager()->AddListener("entity.remove", cb);
      // Register listener for EntityEquipEvent.
      PointerLibrary::getInstance()->getEventManager()->AddListener("entity.equip", cb);
      // Register listener for EntityEquipEvent.
      PointerLibrary::getInstance()->getEventManager()->AddListener("entity.mount", cb);
      // Register listener for EntityPoseEvent.
      PointerLibrary::getInstance()->getEventManager()->AddListener("entity.pose", cb);

      // Register listener for EntityEquipEvent.
      PointerLibrary::getInstance()->getEventManager()->AddListener("state.play", cb);

      // Register listener for ActionInteract.
      EventHandler<EntityManager>* cbInteract = new EventHandler<EntityManager>(&EntityManager::OnInteract, this);
      PointerLibrary::getInstance()->getEventManager()->AddListener("input.ACTION_INTERACT", cbInteract);

      return true;
    }

    void EntityManager::ProcessEvents()
    {
      using namespace PT::Events;

      for (size_t i = 0; i < events.GetSize(); i++)
      {
        Eventp ev = events.Pop();
        if (ev->GetEventID().compare("entity.add") == 0)
          AddEntity(ev);
        else if (ev->GetEventID().compare("entity.remove") == 0)
          RemoveEntity(ev);
        else if (ev->GetEventID().compare("entity.equip") == 0)
          Equip(ev);
        else if (ev->GetEventID().compare("entity.mount") == 0)
          Mount(ev);
        else if (ev->GetEventID().compare("entity.pose") == 0)
          EntityPose(ev);

        else if (ev->GetEventID().compare("state.play") == 0)
          SetOwnId(ev);
      } // for
    }

    void EntityManager::Handle ()
    {
      if (!world_loaded || !playing) return;

      movementManager->Handle();

      ProcessEvents();
    }

    Entity* EntityManager::findPtEntById(unsigned int id)
    {
      // TODO if id == 0 return 0;
      for (size_t i = 0; i < entities.GetSize(); i++)
      {
        if (entities.Get(i)->GetId() == id)
        {
          return entities.Get(i);
        }
      }
      return 0;
    }

    iCelEntity* EntityManager::findCelEntById(unsigned int id)
    {
      // TODO if id == 0 return 0;
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

      playerId = playEv->ownEntityId;

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

      Entity* entity;

      if (entityAddEv->entityType == DoorEntityType) entity = new DoorEntity(*entityAddEv);
      else if (entityAddEv->entityType == ItemEntityType) entity = new ItemEntity(*entityAddEv);
      else if (entityAddEv->entityType == MountEntityType) entity = new MountEntity(*entityAddEv);
      else if (entityAddEv->entityType == NPCEntityType) entity = new NpcEntity(*entityAddEv);
      else if (entityAddEv->entityType == PCEntityType && playerId == entityAddEv->entityId) entity = PlayerEntity::Instance(entityAddEv);
      else if (entityAddEv->entityType == PCEntityType) entity = new PcEntity(*entityAddEv);
      else
      {
        Report(PT::Error, "Invalid entity type: %d !", entityAddEv->entityType);
        return true;
      }

      Report(PT::Notify, "Adding Entity '%s(%d)' at %s%s.", entity->GetName().c_str(), entity->GetId(), entityAddEv->position.Description().GetData(), (playerId == entityAddEv->entityId) ? " as me":"");
      // Add our entity to the list.
      entities.Push(entity);

      return true;
    }

    bool EntityManager::RemoveEntity(PT::Events::Eventp ev)
    {
      using namespace PT::Events;

      EntityRemoveEvent* entityRemoveEv = GetEntityEvent<EntityRemoveEvent*>(ev);
      if (!entityRemoveEv) return false;

      unsigned int id = entityRemoveEv->entityId;

      for (size_t i = 0; i < entities.GetSize(); i++)
      {
        if (entities[i]->GetId() == id)
        {
          movementManager->RemoveMoveTos(id);
          entities[i]->Reset();
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

      Entity* entity = findPtEntById(entityEquipEv->entityId);
      if (entity)
      {
        if (entity->GetType() == PCEntityType)
        {
          if (!entityEquipEv->itemId == 0)
            {
            ((PcEntity*) entity)->GetEquipment().Equip(entityEquipEv->slotId, entityEquipEv->itemId);
            }
          else
            {
            ((PcEntity*) entity)->GetEquipment().UnEquip(entityEquipEv->slotId);
            }
        }
      }

      return true;
    }

    bool EntityManager::Mount(PT::Events::Eventp ev)
    {
      using namespace PT::Events;

      EntityMountEvent* entityMountEv = GetEntityEvent<EntityMountEvent*>(ev);
      if (!entityMountEv) return false;

      Entity* entity = findPtEntById(entityMountEv->entityId);
      Entity* mount = findPtEntById(entityMountEv->mountId);
      if (entity && mount)
      {
        if (entity->GetType() == PCEntityType && mount->GetType() == MountEntityType)
        {
          MountEntity* m = static_cast<MountEntity*>(mount);
          if (entityMountEv->mount)
          {
            m->Mount(entity);
            //Set camera to follow the mount after mounting. Only done for player's mount.
            //TODO: This is just a temporary solution. It will not work nicely with boats etc.
            if (playerId == entity->GetId()) PlayerEntity::Instance()->GetCamera()->SetFollowEntity(m->GetCelEntity());
          }
          else
          {
            m->UnMount(entity);
            //Set camera to follow the player. See the TODO comment above.
            if (playerId == entity->GetId()) PlayerEntity::Instance()->GetCamera()->SetFollowEntity(PlayerEntity::Instance()->GetCelEntity());
          }
        }
      }

      return true;
    }

    bool EntityManager::OnInteract(PT::Events::Eventp ev)
    {
      using namespace PT::Events;

      InputEvent* inputEv = GetInputEvent<InputEvent*>(ev);
      if (!inputEv) return false;

      if (!inputEv->released)
      {
        csRef<iCelEntity> ent = PointerLibrary::getInstance()->getCursor()->GetSelectedEntity();
        if (!ent) return false;
        csRef<iPcProperties> pcprop = CEL_QUERY_PROPCLASS_ENT(ent, iPcProperties);
        if (!pcprop) return false;

        unsigned int id = pcprop->GetPropertyLong(pcprop->GetPropertyIndex("Entity ID"));
        Entity* ptEnt = findPtEntById(id);
        if (!ptEnt) return false;

        ptEnt->Interact();
      }

      return true;
    }

    void EntityManager::delAllEntities()
    {
      while (0 < entities.GetSize())
      {
        pl->RemoveEntity(entities[0]->GetCelEntity());
        entities.DeleteIndex(0);
      }
      Report(PT::Debug, "Deleted all Entities! %d remain...", pl->GetEntityCount() );
    }

    void EntityManager::DrUpdateOwnEntity()
    {
      DrUpdateRequestMessage drmsg;

      // TODO this should be >0;
     // if (playerId != -1)
      {
        if (!PlayerEntity::Instance()) return;
        csWeakRef<iCelEntity> playerCelEnt = PlayerEntity::Instance()->GetCelEntity();
        if (playerCelEnt.IsValid())
        {
          csRef<iPcLinearMovement> pclinmove = CEL_QUERY_PROPCLASS_ENT(playerCelEnt, iPcLinearMovement);
          if (pclinmove.IsValid())
          {
            bool on_ground;
            float speed, rot, avel;
            csVector3 pos, vel, wvel;
            iSector* sector = 0;

            pclinmove->GetDRData(on_ground, speed, pos, rot, sector, vel, wvel, avel);
            //printf("Send DR: %.2f, <%.2f,%.2f,%.2f>, %.2f\n", speed, pos.x, pos.y, pos.z, rot);

            pclinmove->GetYRotation();

            // Don't want <0,1,0> when on horse!
            iMovable* mov = pclinmove->GetAnchor()->GetMesh()->GetMovable();
            pos = mov->GetFullPosition();
            csVector3 rotv = mov->GetFullTransform ().GetT2O() * csVector3(0,0,1);

            if (rotv.z > 1.0f )  rotv.z = 1.0f;
            if (rotv.z < -1.0f ) rotv.z = -1.0f;

            rot = acos (rotv.z);
            if (rotv.x < 0.0f) rot = 2.0f * PI - rot;            

            //if (vel.Norm() > 0 || avel > 0) return; // Don't update while moving!

            drmsg.setRotation(rot);
            drmsg.setPos(pos.x,pos.y,pos.z);
            if (sector && sector->QueryObject()->GetName())
            {
              PT::Data::SectorDataManager* sectorDataMgr = PointerLibrary::getInstance()->getSectorDataManager();
              PT::Data::Sector* dataSector = sectorDataMgr->GetSectorByName(sector->QueryObject()->GetName());
              if (!dataSector)
              {
                Report(PT::Debug, "Couldn't find sector %s", sector->QueryObject()->GetName());
                return;
              }
              Report(PT::Debug, "SECTOR %s (%d)", sector->QueryObject()->GetName(), dataSector->GetId());
              drmsg.setSectorId(dataSector->GetId());
            }
            else
            {
              return; // no sector? something odd, isn't it?
              //drmsg.setSectorId(0);
            }

            PointerLibrary::getInstance()->getNetwork()->send(&drmsg);
          }
        }
      }
    }

    bool EntityManager::EntityPose(PT::Events::Eventp ev)
    {
      using namespace PT::Events;
      
      EntityPoseEvent* entPoseEvent = GetEntityEvent<EntityPoseEvent*>(ev);
      if (!entPoseEvent) return false;

      Report(PT::Debug, "EntityPose: Recieved Pose %d for entity %d.", 
              entPoseEvent->poseId, entPoseEvent->entityId);
      Entity* entity = findPtEntById(entPoseEvent->entityId);
      if (entity) entity->Pose(entPoseEvent->poseId);

      return true;
    }

  } // Entity namespace
} // PT namespace
