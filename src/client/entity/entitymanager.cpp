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
#include "client/data/sector/sectordatamanager.h"
#include "client/data/sector/sector.h"

#include <iengine/sector.h>
#include <iengine/movable.h>
#include <iengine/mesh.h>
#include <iutil/objreg.h>
#include <imap/loader.h>

#include <physicallayer/pl.h>
#include <physicallayer/propfact.h>
#include <physicallayer/propclas.h>
#include <propclass/mesh.h>
#include <propclass/linmove.h>
#include <propclass/prop.h>

#include "client/network/network.h"

#include "common/network/netmessage.h"

#include "common/event/eventmanager.h"

#include "client/state/statemanager.h"

#include "common/reporter/reporter.h"

#include "client/cursor/cursor.h"

namespace PT
{
  namespace Entity
  {

    EntityManager::EntityManager ()
    {
      this->obj_reg = PointerLibrary::getInstance()->getObjectRegistry();

      engine =  csQueryRegistry<iEngine> (obj_reg);

      vfs =  csQueryRegistry<iVFS> (obj_reg);

      stringset = csQueryRegistryTagInterface<iStringSet> (obj_reg,
        "crystalspace.shared.stringset");

      vc =  csQueryRegistry<iVirtualClock> (obj_reg);

      pl =  csQueryRegistry<iCelPlLayer> (obj_reg);
    }

    EntityManager::~EntityManager ()
    {
    }

    bool EntityManager::Initialize ()
    {
      world_loaded = false;
      playerId = 0;

      // Create a default sector for entities to be added in.
      defaultSector = engine->CreateSector("Default_Sector");
      if (!defaultSector.IsValid()) printf("AAAARRRRR\n");

      using namespace PT::Events;

      EventHandler<EntityManager>* cb = new EventHandler<EntityManager>(&EntityManager::GetEntityEvents, this);
      // Register listener for EntityAddEvent.
      PointerLibrary::getInstance()->getEventManager()->AddListener("entity.add", cb);
      // Register listener for EntityRemoveEvent.
      PointerLibrary::getInstance()->getEventManager()->AddListener("entity.remove", cb);
      // Register listener for EntityEquipEvent.
      PointerLibrary::getInstance()->getEventManager()->AddListener("entity.equip", cb);
      // Register listener for EntityMountEvent.
      PointerLibrary::getInstance()->getEventManager()->AddListener("entity.mount", cb);
      // Register listener for EntityPoseEvent.
      PointerLibrary::getInstance()->getEventManager()->AddListener("entity.pose", cb);
      // Register listener for entity.stat.
      PointerLibrary::getInstance()->getEventManager()->AddListener("entity.stat.add.player", cb);

      // Register listener for state.play.
      EventHandler<EntityManager>* cbPlay = new EventHandler<EntityManager>(&EntityManager::SetOwnId, this);
      PointerLibrary::getInstance()->getEventManager()->AddListener("state.play", cbPlay);

      // Register listener for ActionInteract.
      EventHandler<EntityManager>* cbInteract = new EventHandler<EntityManager>(&EntityManager::OnInteract, this);
      PointerLibrary::getInstance()->getEventManager()->AddListener("input.Interact", cbInteract);

      // Register listener for WorldLoaded.
      EventHandler<EntityManager>* cbWorldLoaded = new PT::Events::EventHandler<EntityManager>(&EntityManager::WorldLoaded, this);
      PointerLibrary::getInstance()->getEventManager()->AddListener("world.loaded", cbWorldLoaded);

      return true;
    }

    void EntityManager::ProcessEvents()
    {
      using namespace PT::Events;
      EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();

      csRefArray<iEvent> unhandledEvents;

      while (!events.IsEmpty())
      {
        csRef<iEvent> ev = events.Pop();
        csStringID id = ev->GetName();
        if (playerId != 0)
        {
          if (evmgr->IsKindOf(id, "entity.add"))
            AddEntity(*ev);
        }
        bool statePlay = (PointerLibrary::getInstance()->getStateManager()->GetState() == STATE_PLAY);
        if (world_loaded && statePlay)
        {
          if (evmgr->IsKindOf(id, "entity.remove"))
            RemoveEntity(*ev);
          else if (evmgr->IsKindOf(id, "entity.equip"))
            Equip(*ev);
          else if (evmgr->IsKindOf(id, "entity.mount"))
            Mount(*ev);
          else if (evmgr->IsKindOf(id, "entity.pose"))
            EntityPose(*ev);
          else if (PlayerEntity::Instance() && evmgr->IsKindOf(id, "entity.stat.add.player"))
            evmgr->AddEvent(ev);
        }
        if (world_loaded && statePlay && !PlayerEntity::Instance())
            unhandledEvents.Push(ev);
      } // while

      events = unhandledEvents;
    }

    bool EntityManager::WorldLoaded(iEvent& ev)
    {

      for (size_t i = 0; i < entities.GetSize(); i++)
      {
        if (entities.Get(i))
        {
          entities.Get(i)->SetFullPosition();
        }
      }

      return true;
    }

    void EntityManager::ProcessLostEntities()
    {
      // TODO move this to the region load callback.
      iSector* defsector = engine->FindSector("Default_Sector");
      if (!defsector) return;

      iMeshList* list = defsector->GetMeshes();
      for (size_t i = 0; i < (size_t)list->GetCount(); i++)
      {
        iMeshWrapper* mesh = list->Get(i);
        if (!mesh) continue;
        iCelEntity* entity = pl->FindAttachedEntity(mesh->QueryObject());
        if (!entity) continue;
        csRef<iPcProperties> pcprop = CEL_QUERY_PROPCLASS_ENT(entity, iPcProperties);
        if (!pcprop) continue;
        Entity* ptent = findPtEntById(pcprop->GetPropertyLong(pcprop->GetPropertyIndex("Entity ID")));
        if (!ptent) continue;

        iSector* sector = engine->FindSector(ptent->GetSectorName().c_str());
        if (!sector) continue;

        ptent->SetFullPosition();
        mesh->GetMovable()->GetSectors()->Remove(defsector);
        Report(PT::Debug, "Lost entity '%s' relocated", ptent->GetName().c_str());
      }
    }

    void EntityManager::Handle ()
    {
      ProcessEvents();

      ProcessLostEntities();
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

    bool EntityManager::GetEntityEvents(iEvent& ev)
    {
      using namespace PT::Events;

      events.Push(&ev);

      return true;
    }

    bool EntityManager::SetOwnId(iEvent& ev)
    {
      using namespace PT::Events;

      ev.Retrieve("ownEntityId", playerId);

      return true;
    }

    bool EntityManager::AddEntity(iEvent& ev)
    {
      using namespace PT::Events;

      unsigned int id = EntityHelper::GetEntityID(&ev);
      std::string name = PT::Events::EntityHelper::GetString(&ev, "entityName");
      unsigned int type = EntityHelper::GetEntityType(&ev);

      if ( findPtEntById(id) )
      {
        Report(PT::Warning, "AddEntity: Skipping already existing entity '%s(%d)'", name.c_str(), id);
        return true;
      }

      Entity* entity;

      if (type == DoorEntityType) entity = new DoorEntity(ev);
      else if (type == ItemEntityType) entity = new ItemEntity(ev);
      else if (type == MountEntityType) entity = new MountEntity(ev);
      else if (type == NPCEntityType) entity = new NpcEntity(ev);
      else if (type == PCEntityType && playerId == id) entity = PlayerEntity::Instance(&ev);
      else if (type == PCEntityType) entity = new PcEntity(ev);
      else
      {
        Report(PT::Error, "Invalid entity type: %d !", type);
        return true;
      }

      Report(PT::Notify, "Adding Entity '%s(%d)' at %s%s.", entity->GetName().c_str(), entity->GetId(), entity->GetPosition().Description().GetData(), (playerId == id) ? " as me":"");
      // Add our entity to the list.
      entities.Push(entity);

      return true;
    }

    bool EntityManager::RemoveEntity(iEvent& ev)
    {
      unsigned int id = PT::Events::EntityHelper::GetEntityID(&ev);

      for (size_t i = 0; i < entities.GetSize(); i++)
      {
        if (entities[i]->GetId() == id)
        {
          Report(PT::Notify, "Removing Entity '%s(%d)'.", entities[i]->GetName().c_str(), id);
          entities[i]->Reset();
          pl->RemoveEntity(entities[i]->GetCelEntity());
          entities.DeleteIndex(i);
        }
      }

      return true;
    }

    bool EntityManager::Equip(iEvent& ev)
    {
      using namespace PT::Events;

      unsigned int id = EntityHelper::GetEntityID(&ev);

      unsigned int itemId = -1;
      ev.Retrieve("itemId", itemId);

      //unsigned int variationId = -1;
      //ev.Retrieve("variationId", variationId);

      unsigned int slotId = -1;
      ev.Retrieve("slotId", slotId);

      const char* mesh = "(Null)";
      ev.Retrieve("mesh", mesh);

      const char* file = "(Null)";
      ev.Retrieve("file", file);

      Report(PT::Debug, "Equip for '%d': item %d in slot %d with mesh %s from file %s", id, itemId, slotId, mesh, file);

      Entity* entity = findPtEntById(id);
      if (entity)
      {
        if (entity->GetType() == PlayerEntityType ||
            entity->GetType() == PCEntityType ||
            entity->GetType() == NPCEntityType)
        {
          if (!itemId == 0)
          {
            ((PcEntity*) entity)->GetEquipment().Equip(slotId, itemId, mesh, file);
          }
          else
          {
            ((PcEntity*) entity)->GetEquipment().UnEquip(slotId);
          }
        }
      }

      return true;
    }

    bool EntityManager::Mount(iEvent& ev)
    {
      using namespace PT::Events;

      unsigned int entityId = EntityHelper::GetEntityID(&ev);

      unsigned int mountId = -1;
      ev.Retrieve("mountId", mountId);

      bool mounting = false;
      ev.Retrieve("mount", mounting);

      Entity* entity = findPtEntById(entityId);
      Entity* mount = findPtEntById(mountId);
      if (entity && mount)
      {
        if ((entity->GetType() == PlayerEntityType ||
            entity->GetType() == PCEntityType) &&
            mount->GetType() == MountEntityType)
        {
          MountEntity* m = static_cast<MountEntity*>(mount);
          if (mounting)
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

    bool EntityManager::OnInteract(iEvent& ev)
    {
      using namespace PT::Events;

      if (InputHelper::GetButtonDown(&ev))
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
      while (!entities.IsEmpty())
      {
        Entity* ent = entities.Pop();
        if (ent->GetId() != playerId)
        {
          pl->RemoveEntity(ent->GetCelEntity());
          delete ent;
        }
      }
      Report(PT::Debug, "Deleted all Entities! %d remain...", pl->GetEntityCount() );
    }

    void EntityManager::DrUpdateOwnEntity()
    {
      DrUpdateRequestMessage drmsg;

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
            //Report(PT::Debug, "SECTOR %s (%d)", sector->QueryObject()->GetName(), dataSector->GetId());
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

    bool EntityManager::EntityPose(iEvent& ev)
    {
      using namespace PT::Events;

      unsigned int id = EntityHelper::GetEntityID(&ev);

      unsigned int poseId = -1;
      ev.Retrieve("poseId", poseId);

      Entity* entity = findPtEntById(id);
      if (entity) entity->Pose(poseId);

      return true;
    }

  } // Entity namespace
} // PT namespace
