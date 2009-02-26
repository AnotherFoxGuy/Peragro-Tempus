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
#include <boost/lexical_cast.hpp>

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

namespace Client
{
  namespace Entity
  {

    EntityManager::EntityManager () : ::PT::Entity::EntityManager()
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
      primaryId = 0;

      // Create a default sector for entities to be added in.
      defaultSector = engine->CreateSector("Default_Sector");

      // Load the door animations.
      csRef<iThreadedLoader >loader = csQueryRegistry<iThreadedLoader> (obj_reg);
      if (!loader) return Report(PT::Error, "Failed to locate Loader!");
      csRef<iThreadReturn> tr = loader->LoadLibraryFile("/peragro/xml/animations/doorquests.xml");
      tr->Wait();

      PT_SETUP_HANDLER
      PT_REGISTER_LISTENER(EntityManager, GetEntityEvents, "entity.add")
      PT_REGISTER_LISTENER(EntityManager, GetEntityEvents, "entity.remove")
      PT_REGISTER_LISTENER(EntityManager, GetEntityEvents, "entity.equip")
      PT_REGISTER_LISTENER(EntityManager, GetEntityEvents, "entity.mount")
      PT_REGISTER_LISTENER(EntityManager, GetEntityEvents, "entity.unmount")
      PT_REGISTER_LISTENER(EntityManager, GetEntityEvents, "entity.pose")
      PT_REGISTER_LISTENER(EntityManager, GetEntityEvents, "entity.stat.add.player")

      PT_REGISTER_LISTENER(EntityManager, SetOwnId, "state.play")
      PT_REGISTER_LISTENER(EntityManager, OnInteract, "input.Interact")
      PT_REGISTER_LISTENER(EntityManager, WorldLoaded, "world.loaded")

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
        if (primaryId != 0)
        {
          if (evmgr->IsKindOf(id, "entity.add"))
            AddEntity(*ev);
        }
        bool statePlay = (PointerLibrary::getInstance()->getStateManager()->GetState() == PT::STATE_PLAY);
        if (world_loaded && statePlay)
        {
          if (evmgr->IsKindOf(id, "entity.remove"))
            RemoveEntity(*ev);
          else if (evmgr->IsKindOf(id, "entity.equip"))
            Equip(*ev);
          else if (evmgr->IsKindOf(id, "entity.mount"))
            Mount(*ev);
          else if (evmgr->IsKindOf(id, "entity.umount"))
            UnMount(*ev);
          else if (evmgr->IsKindOf(id, "entity.pose"))
            EntityPose(*ev);
          else if (::PT::Entity::PlayerEntity::Instance() && evmgr->IsKindOf(id, "entity.stat.list.player"))
            evmgr->AddEvent(ev);
        }
        if (world_loaded && statePlay && !::PT::Entity::PlayerEntity::Instance())
            unhandledEvents.Push(ev);
      } // while

      events = unhandledEvents;
    }

    bool EntityManager::WorldLoaded(iEvent& ev)
    {

      std::vector<Common::Entity::Entityp>::iterator i;
      for (i = entities.begin();  i < entities.end();  i++)
      {
        if (*i) (*i)->SetFullPosition();
      }
  
      return true;
    }

    void EntityManager::ProcessLostEntities()
    {
      iSector* defsector = engine->FindSector("Default_Sector");
      if (!defsector) return;

      iMeshList* list = defsector->GetMeshes();
      for (size_t i = 0; i < (size_t)list->GetCount(); i++)
      {
        iMeshWrapper* mesh = list->Get((int)i);
        if (!mesh) continue;
        iCelEntity* entity = pl->FindAttachedEntity(mesh->QueryObject());
        if (!entity) continue;
        csRef<iPcProperties> pcprop = CEL_QUERY_PROPCLASS_ENT(entity, iPcProperties);
        if (!pcprop) continue;
        Common::Entity::Entityp ptent = findEntById(pcprop->GetPropertyLong(pcprop->GetPropertyIndex("Entity ID")));
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

    bool EntityManager::GetEntityEvents(iEvent& ev)
    {
      using namespace PT::Events;

      events.Push(&ev);

      return true;
    }

    bool EntityManager::SetOwnId(iEvent& ev)
    {
      using namespace PT::Events;

      ev.Retrieve("entityId", primaryId);

      return true;
    }

    bool EntityManager::AddEntity(iEvent& ev)
    {
      using namespace PT::Events;

      EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();

      unsigned int id = EntityHelper::GetEntityID(&ev);
      std::string name = Helper::GetString(&ev, "entityName");
      unsigned int type = EntityHelper::GetEntityType(&ev, evmgr);

      if ( findEntById(id) )
      {
        Report(PT::Warning, "AddEntity: Skipping already existing entity '%s(%d)'", name.c_str(), id);
        return true;
      }

      PT::Entity::Entity* entity = NULL;
      boost::shared_ptr<PT::Entity::Entity> entityp;

      if (type == Common::Entity::DoorEntityType) entity = new PT::Entity::DoorEntity(ev);
      else if (type == Common::Entity::ItemEntityType) entity = new PT::Entity::ItemEntity(ev);
      else if (type == Common::Entity::MountEntityType) entity = new PT::Entity::MountEntity(ev);
      else if (type == Common::Entity::NPCEntityType) entity = new PT::Entity::NpcEntity(ev);
      else if (type == Common::Entity::PCEntityType && primaryId == id) 
        entityp = PT::Entity::PlayerEntity::Instance(&ev);
      else if (type == Common::Entity::PCEntityType) entity = new PT::Entity::PcEntity(ev);
      else
      {
        Report(PT::Error, "Invalid entity type: %d !", type);
        return true;
      }
      if (!entityp) entityp = boost::shared_ptr<PT::Entity::Entity>(entity);
      entity = entityp.get();

      // set the entity's pl
      ((Entity*)entity)->pl = pl;

      Report(PT::Notify, "Adding Entity '%s(%d)' at %s%s.", entity->GetName().c_str(), entity->GetId(), entity->GetPositionStr().c_str(), (primaryId == id) ? " as me":"");
      // Add our entity to the list.
      entities.push_back(entityp);

      return true;
    }

    bool EntityManager::RemoveEntity(iEvent& ev)
    {
      unsigned int id = PT::Events::EntityHelper::GetEntityID(&ev);

      std::string a = "";
      std::vector<Common::Entity::Entityp>::iterator i;

      for (i = entities.begin();  i < entities.end();  i++)
      {
          a += boost::lexical_cast<std::string>((*i)->GetId());
          a += " ";
      }
      Report(PT::Debug, "Showing entities before removal: '%s'.", a.c_str());

      //Display the entity being removed
      for (i = entities.begin();  i < entities.end();  i++)
      {
          if ((*i)->GetId() == id)
              Report(PT::Debug, "Removing Entity '%s(%d)'.", (*i)->GetName().c_str(), id);
      }
     
      // Mark all entities for removal where entity->GetId() == id
      std::vector<Common::Entity::Entityp>::iterator new_end = std::remove_if(
          entities.begin(), entities.end(), Common::Entity::CompareId(id));

      // Perform the removal
      entities.erase(new_end, entities.end());

      //Display the results
      a = "";
      for (i = entities.begin();  i < entities.end();  i++)
      {
          a += boost::lexical_cast<std::string>((*i)->GetId()) + " ";
      }
      Report(PT::Debug, "Showing entities after removal: '%s'.", a.c_str());
      return true;
    }

    bool EntityManager::Equip(iEvent& ev)
    {
      using namespace PT::Events;

      unsigned int id = EntityHelper::GetEntityID(&ev);

      unsigned int itemId = -1;
      ev.Retrieve("itemId", itemId);

      //unsigned int variation = -1;
      //ev.Retrieve("variation", variation);

      unsigned int slotId = -1;
      ev.Retrieve("slotId", slotId);

      const char* mesh = "(Null)";
      ev.Retrieve("meshName", mesh);

      const char* file = "(Null)";
      ev.Retrieve("fileName", file);

      Report(PT::Debug, "Equip for '%d': item %d in slot %d with mesh %s from file %s", id, itemId, slotId, mesh, file);

      Common::Entity::Entityp entity = findEntById(id);
      if (entity)
      {
        if (entity->GetType() == Common::Entity::PlayerEntityType ||
            entity->GetType() == Common::Entity::PCEntityType ||
            entity->GetType() == Common::Entity::NPCEntityType)
        {
          if (!itemId == 0)
          {
            ((PT::Entity::PcEntity*) entity.get())->GetEquipment().Equip(slotId, itemId, mesh, file);
          }
          else
          {
            ((PT::Entity::PcEntity*) entity.get())->GetEquipment().UnEquip(slotId);
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

      bool canControl = false;
      ev.Retrieve("canControl", canControl);

      Common::Entity::Entityp entity = findEntById(entityId);
      Common::Entity::Entityp mount = findEntById(mountId);
      if (entity && mount)
      {
        if ((entity->GetType() == Common::Entity::PlayerEntityType ||
          entity->GetType() == Common::Entity::PCEntityType) &&
          mount->GetType() == Common::Entity::MountEntityType)
        {
          PT::Entity::MountEntity* m = static_cast<PT::Entity::MountEntity*>(mount.get());
          m->Mount(static_cast<Entity*>(entity.get()));
          //Set camera to follow the mount after mounting. Only done for player's mount.
          //TODO: This is just a temporary solution. It will not work nicely with boats etc.
          if (primaryId == entity->GetId()) PT::Entity::PlayerEntity::Instance()->GetCamera()->SetFollowEntity(m->GetCelEntity());
        }
      }

      return true;
    }

    bool EntityManager::UnMount(iEvent& ev)
    {
      using namespace PT::Events;

      unsigned int entityId = EntityHelper::GetEntityID(&ev);

      unsigned int mountId = -1;
      ev.Retrieve("mountId", mountId);

      Common::Entity::Entityp entity = findEntById(entityId);
      Common::Entity::Entityp mount = findEntById(mountId);
      if (entity && mount)
      {
        if ((entity->GetType() == Common::Entity::PlayerEntityType ||
          entity->GetType() == Common::Entity::PCEntityType) &&
          mount->GetType() == Common::Entity::MountEntityType)
        {
          PT::Entity::MountEntity* m = static_cast<PT::Entity::MountEntity*>(mount.get());
          m->UnMount(static_cast<Entity*>(entity.get()));
          //Set camera to follow the player. See the TODO comment above.
          if (primaryId == entity->GetId()) PT::Entity::PlayerEntity::Instance()->GetCamera()->SetFollowEntity(PT::Entity::PlayerEntity::Instance()->GetCelEntity());
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
        Entity* ptEnt = dynamic_cast<Entity*> (findEntById(id).get());
        if (!ptEnt) return false;

        ptEnt->Interact();
      }

      return true;
    }

    void EntityManager::DrUpdateOwnEntity()
    {
      DrUpdateRequestMessage drmsg;

      if (!PT::Entity::PlayerEntity::Instance()) return;

      csWeakRef<iCelEntity> playerCelEnt = PT::Entity::PlayerEntity::Instance()->GetCelEntity();
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
          drmsg.setPos(pos.x, pos.y, pos.z);
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

      Entity* entity = dynamic_cast<Entity*>(findEntById(id).get());
      if (entity) entity->Pose(poseId);

      return true;
    }

  } // Entity namespace
} // Client namespace
