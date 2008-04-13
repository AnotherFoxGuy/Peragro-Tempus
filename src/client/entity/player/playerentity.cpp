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

#include <iengine/sector.h>
#include <iengine/camera.h>
#include <csgeom/vector3.h>
#include <iutil/plugin.h>

#include <physicallayer/pl.h>
#include <physicallayer/propfact.h>
#include <physicallayer/propclas.h>
#include <propclass/defcam.h>
#include <propclass/linmove.h>

#include "client/reporter/reporter.h"
#include "client/pointer/pointer.h"

#include "client/event/eventmanager.h"
#include "client/event/interfaceevent.h"
#include "client/event/entityevent.h"

#include "client/entity/statmanager.h"

#include "client/world/world.h"

#include "client/data/sector.h"
#include "client/data/sectordatamanager.h"

#include "include/client/component/entity/input/playercontrols.h"

// These defines should probably go to configuration file.
// PERIOD defines were calculated from the player character animations.
// OFFSET_RANGE defines were made up.
#define WALK_PERIOD 499
#define WALK_OFFSET_RANGE 0.03f
#define RUN_PERIOD 416
#define RUN_OFFSET_RANGE 0.08f
#define HEAD_HEIGHT 1.55f

//#define _MOVEMENT_DEBUG_CHARACTER_

namespace PT
{

  namespace Entity
  {

    PlayerEntity* PlayerEntity::instance = 0;


    PlayerEntity::PlayerEntity(const iEvent& ev) : PcEntity(ev)
    {
      viewBobEffect.base = HEAD_HEIGHT;
      viewBobEffect.offset = 0.0f;
      viewBobEffect.period = WALK_PERIOD;
      viewBobEffect.range = WALK_OFFSET_RANGE;

      Create();

      iObjectRegistry* object_reg = PointerLibrary::getInstance()->getObjectRegistry();

      vfs = csQueryRegistry<iVFS> (object_reg);
      if (!vfs)
      {
        Report(PT::Error, "Can't find the vfs!");
        return;
      }

      csRef<iPluginManager> plugin_mgr = csQueryRegistry<iPluginManager> (object_reg);

      // @TODO: Move to ComponentManager or something.
      csRef<ComponentFactoryInterface> fact = csLoadPlugin<ComponentFactoryInterface> (plugin_mgr, "peragro.entity.input.playercontrols");
      csRef<ComponentInterface> playerControlsInt = fact->CreateComponent("peragro.entity.input.playercontrols");
      csRef<iPlayerControls> playerControls = scfQueryInterface<iPlayerControls> (playerControlsInt);
      if(playerControls.IsValid())
        playerControls->Initialize(PointerLibrary::getInstance(), this);
      else
        Report(PT::Error, "Failed to load the playerControls!");
      components.Push(playerControls);

    }

    PlayerEntity::~PlayerEntity()
    {
      instance=0;
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
      EntityHelper::EquipmentData* evequipment = EntityHelper::GetEquipment(&ev);
      if (evequipment)
      {
        for(size_t i = 0; i < evequipment->GetSize(); i++)
          equipment.Equip(evequipment->Get(i).slotId, evequipment->Get(i).itemId);
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

      pos = PT::Events::EntityHelper::GetPosition(&ev);
      ev.Retrieve("rotation", rot);

      PointerLibrary::getInstance()->getWorld()->EnterWorld(pos.x, pos.z);
      SetFullPosition(pos, rot, sectorName);
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
        csVector3 offset(0.0f, viewBobEffect.base, 0.0f);
        camera->SetFirstPersonOffset(offset);
      }
      else
        Report(PT::Error, "Failed to get PcDefaultCamera for %s!(%d)", name.c_str(), id);

      PointerLibrary::getInstance()->getWorld()->EnterWorld(pos.x, pos.z);
      SetFullPosition(pos, rot, sectorName.c_str());

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

    void PlayerEntity::CameraDraw(csTicks elapsedTicks)
    {
      if (!camera.IsValid()) return;

      if (elapsedTicks > 0)
      {
        bool offsetChanged;
        if (camera->GetMode() == iPcDefaultCamera::firstperson &&
            celEntity.IsValid())
        {
          // TODO: Make the effect work with mounts.
          csRef<iPcLinearMovement> pclinmove =
            CEL_QUERY_PROPCLASS_ENT(celEntity, iPcLinearMovement);
          if (pclinmove && !pclinmove->GetVelocity().IsZero() &&
              pclinmove->IsOnGround())
          {
            // In first person mode, moving and on the ground.
            offsetChanged = viewBobEffect.Move(elapsedTicks);
            if (!offsetChanged) Report(PT::Error,
              "Camera offset change too small for bobbing effect, elapsed ticks: %lu",
              elapsedTicks);
          }
          else
          {
            // In first person mode, not moving or in the air.
            offsetChanged = viewBobEffect.Reset(false, elapsedTicks);
          }
        }
        else
        {
          // Not in first person mode, reset the height now.
          offsetChanged = viewBobEffect.Reset(true);
        }

        // If offset was changed, shift the camera.
        if (offsetChanged)
        {
          csVector3 offset(0.0f, viewBobEffect.base+viewBobEffect.offset, 0.0f);
          camera->SetFirstPersonOffset(offset);
        }
      }

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

      PointerLibrary::getInstance()->getWorld()->EnterWorld(pos.x, pos.z);
      this->SetFullPosition(pos, rotation, sector.c_str());
    }

    void PlayerEntity::SetFullPosition(const csVector3& pos,
                                      float rotation,
                                      const std::string& sector)
    {
      csRef<iObjectRegistry> obj_reg =
        PointerLibrary::getInstance()->getObjectRegistry();
      csRef<iEngine> engine =  csQueryRegistry<iEngine> (obj_reg);

      printf("PlayerEntity::SetFullPosition\n");

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
          // TODO: How to stop the camera from tracking,
          // so that the relocation is instantaneous??
          csVector3 offset = pos - this->pos;
          camera->GetCamera()->SetSector(sec);
          camera->GetCamera()->Move(offset, false);
          camera->UpdateCamera();
        }
      }

      Entity::SetFullPosition(pos, rotation, sector);

    } // end SetFullPosition()

    bool PlayerEntity::ViewBobEffect::Move(float elapsedTicks)
    {
      // 4 parts of the cycle, increases and decreases through + and -.
      float offsetChange = elapsedTicks / (period/4) * range;
      if (offsetChange < 0.00001f && offsetChange > -0.00001f) return false;

      if (upwards)
      {
        // If the offset is above the range, change the direction.
        if (offset + offsetChange >= range)
        {
          offset = range;
          upwards = false;
        }
        else
        {
            offset += offsetChange;
        }
      }
      else
      {
        // If the offset is below the range, change the direction.
        if (offset - offsetChange <= -1.0f * range)
        {
          offset = -1.0f * range;
          upwards = true;
        }
        else
        {
          offset -= offsetChange;
        }
      }
      return true;
    } // end ViewBobEffect::Move()

    bool PlayerEntity::ViewBobEffect::Reset(bool hard, float elapsedTicks)
    {
      if (offset < 0.00001f && offset > -0.00001f) return false;
      if (hard)
      {
        offset = 0.0f;
        return true;
      }

      // 4 parts of the cycle, increases and decreases through + and -.
      float offsetChange = elapsedTicks / (period/4) * range;
      // Above the base height, change down toward it.
      if (offset > 0.0f)
      {
        if (offset - offsetChange <= 0.0f)
        {
          offset = 0.0f;
          upwards = true;
        }
        else
        {
          offset -= offsetChange;
        }
      }
      // Below the base height, change up toward it.
      else if (offset < 0.0f)
      {
        if (offset + offsetChange >= 0.0f)
        {
          offset = 0.0f;
          upwards = true;
        }
        else
        {
          offset += offsetChange;
        }
      }
      return true;
    } // end ViewBobEffect::Reset()

  } //Entity namespace
} //PT namespace
