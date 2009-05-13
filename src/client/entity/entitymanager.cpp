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

    EntityManager::EntityManager () : Common::Entity::EntityManager(), world_loaded(false)
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
      defaultSector = engine->CreateSector("World");

      // Load the door animations.
      csRef<iThreadedLoader >loader = csQueryRegistry<iThreadedLoader> (obj_reg);
      if (!loader) return Report(PT::Error, "Failed to locate Loader!");
      if (!loader->LoadLibraryFileWait("/peragro/xml/animations/", "doorquests.xml")
        ->WasSuccessful())
      {
        return Report(PT::Error, "Failed to load doorquests.xml!");
      }

      PT_SETUP_HANDLER
      PT_REGISTER_LISTENER(EntityManager, AddEntity, "entity.add")
      PT_REGISTER_LISTENER(EntityManager, RemoveEntity, "entity.remove")
      PT_REGISTER_LISTENER(EntityManager, Equip, "entity.equip")
      PT_REGISTER_LISTENER(EntityManager, Mount, "entity.mount")
      PT_REGISTER_LISTENER(EntityManager, UnMount, "entity.unmount")
      PT_REGISTER_LISTENER(EntityManager, EntityPose, "entity.pose")

      PT_REGISTER_LISTENER(EntityManager, Resource, "entity.resource.list")

      PT_REGISTER_LISTENER(EntityManager, SetOwnId, "state.play")
      PT_REGISTER_LISTENER(EntityManager, OnInteract, "input.Interact")
      PT_REGISTER_LISTENER(EntityManager, WorldLoaded, "world.loaded")

      // Create the player.
      PT::Entity::PlayerEntity::Instance();

      return true;
    }

    bool EntityManager::Resource(iEvent& ev)
    {
      using namespace PT::Events;
      EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
      if (!PT::Entity::PlayerEntity::Instance()->GetResourcesReady())
      {
        Report(PT::Notify, "EntityManager::Resource");
        evmgr->AddEvent(&ev);
      }
      return false;
    }

    bool EntityManager::WorldLoaded(iEvent& ev)
    {
      /* TODO: Why does this crash?? :/
      ConstIterator it = entities.begin();
      for (;  it != entities.end();  it++)
      {
        //if (it->second) it->second->SetFullPosition();
      }
      */
      /*
      if (::PT::Entity::PlayerEntity::Instance())
      {
        ::PT::Entity::PlayerEntity::Instance()->SetFullPosition();
      }
      */
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

      if (primaryId == Common::Entity::Entity::NoEntity)
      {
        evmgr->AddEvent(&ev);
        return false;
      }

      unsigned int id = EntityHelper::GetEntityID(&ev);
      std::string name = Helper::GetString(&ev, "entityName");
      unsigned int type = EntityHelper::GetEntityType(&ev, evmgr);

      if ( FindById(id) )
      {
        Report(PT::Warning, "AddEntity: Skipping already existing entity '%s(%d)'", name.c_str(), id);
        return true;
      }

      PT::Entity::Entity* entity = 0;
      boost::shared_ptr<PT::Entity::Entity> entityp;

      if (type == Common::Entity::DoorEntityType) entity = new PT::Entity::DoorEntity();
      else if (type == Common::Entity::ItemEntityType) entity = new PT::Entity::ItemEntity();
      else if (type == Common::Entity::MountEntityType) entity = new PT::Entity::MountEntity();
      else if (type == Common::Entity::NPCEntityType) entity = new PT::Entity::NpcEntity();
      else if (type == Common::Entity::PCEntityType && primaryId == id)
        entityp = PT::Entity::PlayerEntity::Instance();
      else if (type == Common::Entity::PCEntityType) entity = new PT::Entity::PcEntity();
      else
      {
        Report(PT::Error, "Invalid entity type: %d !", type);
        return true;
      }

      if (!entityp) entityp = boost::shared_ptr<PT::Entity::Entity>(entity);
      entity = entityp.get();

      // set the entity's pl
      ((::Client::Entity::Entity*)entity)->pl = pl;

      ((::Client::Entity::Entity*)entity)->Initialize(ev);

      Report(PT::Notify, "Adding Entity '%s(%d)' at %s%s.", entity->GetName().c_str(), entity->GetId(), entity->GetPositionStr().c_str(), (primaryId == id) ? " as me":"");

      // Add our entity to the list.
      Add(entityp);

      return true;
    }

    bool EntityManager::RemoveEntity(iEvent& ev)
    {
      unsigned int id = PT::Events::EntityHelper::GetEntityID(&ev);

      Remove(id);

      return true;
    }

    bool EntityManager::Equip(iEvent& ev)
    {
      using namespace PT::Events;

      unsigned int id = EntityHelper::GetEntityID(&ev);

      unsigned int itemId = -1;
      ev.Retrieve("itemId", itemId);

      unsigned int slotId = -1;
      ev.Retrieve("slotId", slotId);

      const char* mesh = "(Null)";
      ev.Retrieve("meshName", mesh);

      const char* file = "(Null)";
      ev.Retrieve("fileName", file);

      Report(PT::Debug, "Equip for '%d': item %d in slot %d with mesh %s from file %s", id, itemId, slotId, mesh, file);

      Common::Entity::Entityp entity = FindById(id);
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
      PT::Events::Helper::DisplayEvent(&ev);
      using namespace PT::Events;

      unsigned int entityId = -1;
      entityId = PT::Events::Helper::GetUInt(&ev,"playerEntityId");

      unsigned int mountId = -1;
      mountId = PT::Events::Helper::GetUInt(&ev,"mountEntityId");

      bool canControl = false;
      canControl = PT::Events::Helper::GetBool(&ev,"canControl");

      Common::Entity::Entityp entity = FindById(entityId);
      Common::Entity::Entityp mount = FindById(mountId);
      if (entity && mount)
      {
        if ((entity->GetType() == Common::Entity::PlayerEntityType ||
          entity->GetType() == Common::Entity::PCEntityType) &&
          mount->GetType() == Common::Entity::MountEntityType)
        {
          PT::Entity::MountEntity* m = static_cast<PT::Entity::MountEntity*>(mount.get());
          m->Mount(static_cast< ::Client::Entity::Entity*>(entity.get()));
          //Set camera to follow the mount after mounting. Only done for player's mount.
          //TODO: This is just a temporary solution. It will not work nicely with boats etc.
          if (primaryId == entity->GetId()) PT::Entity::PlayerEntity::Instance()->GetCamera()->SetFollowEntity(m->GetCelEntity());
          iPcDefaultCamera* PcCam = PT::Entity::PlayerEntity::Instance()->GetCamera();
        }
      }
      return true;
    }

    bool EntityManager::UnMount(iEvent& ev)
    {
      using namespace PT::Events;

      unsigned int entityId = -1;
      entityId = entityId = PT::Events::Helper::GetUInt(&ev,"playerEntityId");

      unsigned int mountId = -1;
      mountId = PT::Events::Helper::GetUInt(&ev,"mountEntityId");

      Common::Entity::Entityp entity = FindById(entityId);
      Common::Entity::Entityp mount = FindById(mountId);
      if (entity && mount)
      {
        if ((entity->GetType() == Common::Entity::PlayerEntityType ||
          entity->GetType() == Common::Entity::PCEntityType) &&
          mount->GetType() == Common::Entity::MountEntityType)
        {
          PT::Entity::MountEntity* m = static_cast<PT::Entity::MountEntity*>(mount.get());
          m->UnMount(static_cast< ::Client::Entity::Entity*>(entity.get()));
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
        ::Client::Entity::Entity* ptEnt = dynamic_cast< ::Client::Entity::Entity*> (FindById(id).get());
        if (!ptEnt) return false;

        ptEnt->Interact();
      }

      return true;
    }

    iCelEntity* EntityManager::findCelEntById(unsigned int id)
    {
      Entity* entity = dynamic_cast<Entity*>(FindById(id).get());
      return entity ? entity->GetCelEntity() : 0;
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
          drmsg.setPosition(pos.x, pos.y, pos.z);

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

      ::Client::Entity::Entity* entity = dynamic_cast< ::Client::Entity::Entity*>(FindById(id).get());
      if (entity) entity->Pose(poseId);

      return true;
    }

  } // Entity namespace
} // PT namespace
