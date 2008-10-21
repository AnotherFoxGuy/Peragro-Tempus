/*
    Copyright (C) 2007 Development Team of Peragro Tempus

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

#include "playerentity.h"

#include <iengine/engine.h>
#include <iengine/sector.h>
#include <iengine/camera.h>
#include <iutil/plugin.h>

#include <physicallayer/pl.h>
#include <physicallayer/propfact.h>
#include <physicallayer/propclas.h>
#include <propclass/defcam.h>
#include <propclass/linmove.h>

#include "common/reporter/reporter.h"
#include "client/pointer/pointer.h"

#include "common/event/eventmanager.h"
#include "common/event/interfaceevent.h"
#include "common/event/entityevent.h"

#include "include/world.h"

#include "client/data/sector/sector.h"
#include "client/data/sector/sectordatamanager.h"

#include "client/component/componentmanager.h"
#include "include/client/component/entity/input/playercontrols.h"
#include "include/client/component/entity/input/viewcontrols.h"
#include "include/client/component/entity/move/viewbob.h"

//#define _MOVEMENT_DEBUG_CHARACTER_

namespace PT
{

  namespace Entity
  {

    PlayerEntity* PlayerEntity::instance = 0;


    PlayerEntity::PlayerEntity(const iEvent& ev) : PcEntity(ev)
    {
      type = Common::Entity::PlayerEntityType;

      Create();

      iObjectRegistry* object_reg =
        PointerLibrary::getInstance()->getObjectRegistry();

      vfs = csQueryRegistry<iVFS> (object_reg);
      if (!vfs)
      {
        Report(PT::Error, "Can't find the vfs!");
        return;
      }

      PT::Component::ComponentManager* componentManager =
        PointerLibrary::getInstance()->getComponentManager();

      ADD_COMPONENT(componentManager, iPlayerControls,
        "peragro.entity.input.playercontrols")
      ADD_COMPONENT(componentManager, iViewControls,
        "peragro.entity.input.viewcontrols")
      ADD_COMPONENT(componentManager, iViewBob, "peragro.entity.move.viewbob")

      PointerLibrary::getInstance()->setPlayer(this);
    }

    PlayerEntity::~PlayerEntity()
    {
      instance=0;
      PointerLibrary::getInstance()->setPlayer(0);
    }

    PlayerEntity* PlayerEntity::Instance(const iEvent* ev)
    {
      //If the instance already exists, and we're not
      //requesting a reinitialization.
      if (instance && !ev) return instance;
      //If the instance already exists, and we're requesting
      //a reinitialization
      else if (instance && ev)
      {
        instance->ReInit(*ev);
        return instance;
      }

      //We can't initialize without a valid event
      if (!ev) return 0;

      instance = new PlayerEntity(*ev);
      return instance;
    }

    void PlayerEntity::ReInit(const iEvent& ev)
    {
      // Reset the Id.
      ev.Retrieve("entityId", id);

      // TODO: Clear the inventory.
      // TODO: Clear the skills and skillbar.

      equipment.ClearAll();

      //Add the equipment
      using namespace Events;
      csRef<iEvent> evequipment = EntityHelper::GetEquipment(&ev);
      if (evequipment)
      {
        csRef<iEventAttributeIterator> items = evequipment->GetAttributeIterator();
        while (items->HasNext())
        {
          csRef<iEvent> item; evequipment->Retrieve(items->Next(), item);
          unsigned int slotId, itemId;
          std::string mesh, file;
          item->Retrieve("slotId", slotId);
          item->Retrieve("itemId", itemId);
          mesh = Helper::GetString(item, "meshName");
          file = Helper::GetString(item, "fileName");
          equipment.Equip(slotId, itemId, mesh, file);
        }
      }
      else
        Report(PT::Error, "PlayerEntity: failed to get equipment!");

      unsigned int sectorId = -1;
      ev.Retrieve("sectorId", sectorId);
      ///@todo This is an ugly hack. The server seems to send some impossible
      ///sector id from time to time.
      PT::Data::Sector* sector = PointerLibrary::getInstance()->
        getSectorDataManager()->GetSectorById(sectorId);
      if (sector) sectorName = sector->GetName();
      //End of ugly hack

      position = PT::Events::EntityHelper::GetPosition(&ev);
      ev.Retrieve("rotation", rot);

      csRef<iObjectRegistry> obj_reg = PointerLibrary::getInstance()->getObjectRegistry();
      csRef<iWorld> world =  csQueryRegistry<iWorld> (obj_reg);

      world->EnterWorld(position.x, position.z);
      SetFullPosition(position, rot, sectorName);
    }

    void PlayerEntity::Create()
    {
      Report(PT::Notify, "CREATING PLAYER");
      celEntity->SetName("player");

      csRef<iObjectRegistry> obj_reg = PointerLibrary::getInstance()->getObjectRegistry();
      csRef<iCelPlLayer> pl =  csQueryRegistry<iCelPlLayer> (obj_reg);

      //At this time PlayerEntity's Create() method takes care of the
      //appropriate stuff, so we only need to setup the camera.
      pl->CreatePropertyClass(celEntity, "pccamera.old");

      camera = CEL_QUERY_PROPCLASS_ENT(celEntity, iPcDefaultCamera);
      if (camera.IsValid())
      {
        //We will draw the camera as needed, in order for it to not overwrite
        //the GUI.
        camera->SetAutoDraw(false);
        camera->SetMode(iPcDefaultCamera::thirdperson, true);
        camera->SetPitch(-0.18f);
      }
      else
        Report(PT::Error, "Failed to get PcDefaultCamera for %s!(%d)", name.c_str(), id);

      csRef<iWorld> world =  csQueryRegistry<iWorld> (obj_reg);

      world->EnterWorld(position.x, position.z);
      SetFullPosition(position, rot, sectorName.c_str());

#ifdef _MOVEMENT_DEBUG_CHARACTER_
      other_self = pl->CreateEntity();

      pl->CreatePropertyClass(other_self, "pcobject.mesh");
      pl->CreatePropertyClass(other_self, "pcmove.solid");
      pl->CreatePropertyClass(other_self, "pcmove.actor.standard");
      pl->CreatePropertyClass(other_self, "pcmove.linear");

      csRef<iPcMesh> pcmesh = CEL_QUERY_PROPCLASS_ENT(other_self, iPcMesh);

      pcmesh->SetMesh(meshName.c_str(), fileName.c_str());

      csRef<iEngine> engine =  csQueryRegistry<iEngine> (obj_reg);
      csRef<iSector> sector = engine->FindSector(sectorName.c_str());

      csRef<iMovable> mov = pcmesh->GetMesh()->GetMovable();
      if (mov.IsValid())
      {
        mov->SetSector(sector);
        mov->SetPosition(pos);
        mov->GetTransform ().SetO2T (csYRotMatrix3(rot));
        mov->UpdateMove();
      }
#endif
    }

    iSector* PlayerEntity::GetSector()
    {
      if (!camera.IsValid()) return 0;
      iCamera* cam = camera->GetCamera();
      if (!cam) return 0;
      return cam->GetSector();
    }

    void PlayerEntity::CameraDraw()
    {
      camera->Draw();
    }

    void PlayerEntity::Interact()
    {
      PT::Events::EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
      csRef<iEvent> interfaceEvent = evmgr->CreateEvent("interface.interact");
      interfaceEvent->Add("entityId", id);
      std::string actions = "Attack, Inventory, Stats";
      interfaceEvent->Add("actions", actions.c_str());
      evmgr->AddEvent(interfaceEvent);
    }

    void PlayerEntity::Teleport(const csVector3& pos,
                                float rotation,
                                const std::string& sector)
    {
      if (!celEntity.IsValid()) return;

      csRef<iObjectRegistry> obj_reg = PointerLibrary::getInstance()->getObjectRegistry();
      csRef<iWorld> world =  csQueryRegistry<iWorld> (obj_reg);

      world->EnterWorld(pos.x, pos.z);
      this->SetFullPosition(pos, rotation, sector.c_str());
    }

    void PlayerEntity::SetFullPosition(const csVector3& pos,
                                      float rotation,
                                      const std::string& sector)
    {
      csRef<iObjectRegistry> obj_reg =
        PointerLibrary::getInstance()->getObjectRegistry();
      csRef<iEngine> engine =  csQueryRegistry<iEngine> (obj_reg);

      if (celEntity.IsValid())
      {
        csRef<iPcDefaultCamera> camera = CEL_QUERY_PROPCLASS_ENT(celEntity, iPcDefaultCamera);
        csRef<iSector> sec = engine->FindSector(sector.c_str());

        if (!sec.IsValid())
        {
          sec = engine->FindSector("Default_Sector");
          if (!sec.IsValid()) printf("AAAARRRGG\n");
        }

        if (camera.IsValid() && camera->GetCamera() && sec.IsValid())
        {
          csVector3 offset = pos - static_cast<const csVector3&>(this->position);
          camera->GetCamera()->SetSector(sec);
          camera->GetCamera()->Move(offset, false);
          camera->UpdateCamera();
        }
      }

      Entity::SetFullPosition(pos, rotation, sector);

    } // end SetFullPosition()

  } //Entity namespace
} //PT namespace
