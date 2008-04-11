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

#include "client/reporter/reporter.h"
#include "client/pointer/pointer.h"

#include "client/cursor.h"
#include "client/effect/effectsmanager.h"
#include "client/event/eventmanager.h"
#include "client/event/interfaceevent.h"
#include "client/event/entityevent.h"
#include "client/event/inputevent.h"
#include "common/network/entitymessages.h"
#include "client/network/network.h"
#include "client/gui/guimanager.h"
#include "client/gui/chat-gui.h"

#include "client/entity/statmanager.h"

#include "client/world/world.h"

#include "client/data/sector.h"
#include "client/data/sectordatamanager.h"

#include <csgeom/vector3.h>

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
      walk = 0;
      turn = 0;
      run = false;
      jump = false;
      //This should be replaced by some external StateManager. We already have
      //things like this in several places
      ready = true;
      cameraDistance = 3.0f;

      viewBobEffect.base = HEAD_HEIGHT;
      viewBobEffect.offset = 0.0f;
      viewBobEffect.period = WALK_PERIOD;
      viewBobEffect.range = WALK_OFFSET_RANGE;

      backwardReverse = false;
      invertYAxis = false;
      minFPS = 20.0f;
      maxFPS = 60.0f;
      minDistance = 50.0f;

      Create();

      //Register actions for events
      using namespace PT::Events;

      PT::Events::EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();

      // Register listener for ActionForward.
      csRef<EventHandlerCallback> cbActionForward;
      cbActionForward.AttachNew(new EventHandler<PlayerEntity>(&PlayerEntity::ActionForward, this));
      evmgr->AddListener("input.ACTION_FORWARD", cbActionForward);
      eventHandlers.Push(cbActionForward);

      // Register listener for ActionBackward.
      csRef<EventHandlerCallback> cbActionBackward;
        cbActionBackward.AttachNew(new EventHandler<PlayerEntity>(&PlayerEntity::ActionBackward, this));
      evmgr->AddListener("input.ACTION_BACKWARD", cbActionBackward);
      eventHandlers.Push(cbActionBackward);

      // Register listener for ActionLeft.
      csRef<EventHandlerCallback> cbActionLeft;
        cbActionLeft.AttachNew(new EventHandler<PlayerEntity>(&PlayerEntity::ActionLeft, this));
      evmgr->AddListener("input.ACTION_LEFT", cbActionLeft);
      eventHandlers.Push(cbActionLeft);

      // Register listener for ActionRight.
      csRef<EventHandlerCallback> cbActionRight;
        cbActionRight.AttachNew(new EventHandler<PlayerEntity>(&PlayerEntity::ActionRight, this));
      evmgr->AddListener("input.ACTION_RIGHT", cbActionRight);
      eventHandlers.Push(cbActionRight);

      // Register listener for ActionToggleWalk.
      csRef<EventHandlerCallback> cbActionToggleWalk;
        cbActionToggleWalk.AttachNew(new EventHandler<PlayerEntity>(&PlayerEntity::ActionToggleWalk, this));
      evmgr->AddListener("input.ACTION_TOGGLEWALK", cbActionToggleWalk);
      eventHandlers.Push(cbActionToggleWalk);

      // Register listener for ActionToggleRun.
      csRef<EventHandlerCallback> cbActionToggleRun;
        cbActionToggleRun.AttachNew(new EventHandler<PlayerEntity>(&PlayerEntity::ActionToggleRun, this));
      evmgr->AddListener("input.ACTION_TOGGLERUN", cbActionToggleRun);
      eventHandlers.Push(cbActionToggleRun);

      // Register listener for ActionPanUp.
      csRef<EventHandlerCallback> cbActionPanUp;
        cbActionPanUp.AttachNew(new EventHandler<PlayerEntity>(&PlayerEntity::ActionPanUp, this));
      evmgr->AddListener("input.ACTION_PANUP", cbActionPanUp);
      eventHandlers.Push(cbActionPanUp);

      // Register listener for ActionPanDown.
      csRef<EventHandlerCallback> cbActionPanDown;
        cbActionPanDown.AttachNew(new EventHandler<PlayerEntity>(&PlayerEntity::ActionPanDown, this));
      evmgr->AddListener("input.ACTION_PANDOWN", cbActionPanDown);
      eventHandlers.Push(cbActionPanDown);

      // Register listener for ActionToggleCamera.
      csRef<EventHandlerCallback> cbActionToggleCamera;
        cbActionToggleCamera.AttachNew(new EventHandler<PlayerEntity>(&PlayerEntity::ActionToggleCamera, this));
      evmgr->AddListener("input.ACTION_TOGGLECAMERA", cbActionToggleCamera);
      eventHandlers.Push(cbActionToggleCamera);

      // Register listener for ActionToggleDistClipping.
      csRef<EventHandlerCallback> cbActionToggleDistClipping;
        cbActionToggleDistClipping.AttachNew(new EventHandler<PlayerEntity>(&PlayerEntity::ActionToggleDistClipping, this));
      evmgr->AddListener("input.ACTION_TOGGLEDISTCLIP", cbActionToggleDistClipping);
      eventHandlers.Push(cbActionToggleDistClipping);

      // Register listener for ActionActivateWeapon.
      csRef<EventHandlerCallback> cbActionActivateWeapon;
        cbActionActivateWeapon.AttachNew(new EventHandler<PlayerEntity>(&PlayerEntity::ActionActivateWeapon, this));
      evmgr->AddListener("input.ACTION_ACTIVATEWEAPON", cbActionActivateWeapon);
      eventHandlers.Push(cbActionActivateWeapon);

      // Register listener for ActionZoomIn.
      csRef<EventHandlerCallback> cbActionZoomIn;
        cbActionZoomIn.AttachNew(new EventHandler<PlayerEntity>(&PlayerEntity::ActionZoomIn, this));
      evmgr->AddListener("input.ACTION_ZOOMIN", cbActionZoomIn);
      eventHandlers.Push(cbActionZoomIn);

      // Register listener for ActionZoomOut.
      csRef<EventHandlerCallback> cbActionZoomOut;
        cbActionZoomOut.AttachNew(new EventHandler<PlayerEntity>(&PlayerEntity::ActionZoomOut, this));
      evmgr->AddListener("input.ACTION_ZOOMOUT", cbActionZoomOut);
      eventHandlers.Push(cbActionZoomOut);

      // Register listener for ActionJump.
      csRef<EventHandlerCallback> cbActionJump;
        cbActionJump.AttachNew(new EventHandler<PlayerEntity>(&PlayerEntity::ActionJump, this));
      evmgr->AddListener("input.ACTION_JUMP", cbActionJump);
      eventHandlers.Push(cbActionJump);

      // Register listener for ActionMoveTo.
      csRef<EventHandlerCallback> cbActionMoveTo;
        cbActionMoveTo.AttachNew(new EventHandler<PlayerEntity>(&PlayerEntity::ActionMoveTo, this));
      evmgr->AddListener("input.ACTION_MOVETO", cbActionMoveTo);
      eventHandlers.Push(cbActionMoveTo);

      // Register listener for InterfaceOptionsEvent.
      csRef<EventHandlerCallback> cbUpdateOptions;
        cbUpdateOptions.AttachNew(new EventHandler<PlayerEntity>(&PlayerEntity::UpdateOptions, this));
      evmgr->AddListener("interface.options", cbUpdateOptions);
      eventHandlers.Push(cbUpdateOptions);

      app_cfg = csQueryRegistry<iConfigManager> (PointerLibrary::getInstance()->getClient()->GetObjectRegistry());
      if (!app_cfg)
      {
        Report(PT::Error, "Can't find the config manager!");
        return;
      }

      vfs = csQueryRegistry<iVFS> (PointerLibrary::getInstance()->getClient()->GetObjectRegistry());
      if (!vfs)
      {
        Report(PT::Error, "Can't find the vfs!");
        return;
      }

      UpdateOptions();
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

    bool PlayerEntity::ActionForward(iEvent& ev)
    {
      using namespace PT::Events;

      if (ready)
      {
        if (InputHelper::GetButtonDown(&ev)) walk = 1;
        else walk = 0;

#ifdef _MOVEMENT_DEBUG_CHARACTER_
        csRef<iPcActorMove> pcactormove = CEL_QUERY_PROPCLASS_ENT(other_self, iPcActorMove);
        pcactormove->SetMovementSpeed(4);
        pcactormove->Forward(walk != 0);
#endif
      }
      PerformMovementAction();

      return true;
    }

    bool PlayerEntity::ActionBackward(iEvent& ev)
    {
      using namespace PT::Events;

      if (ready)
      {
        if (InputHelper::GetButtonDown(&ev)) walk = -1;
        else walk = 0;

#ifdef _MOVEMENT_DEBUG_CHARACTER_
        csRef<iPcActorMove> pcactormove = CEL_QUERY_PROPCLASS_ENT(other_self, iPcActorMove);
        pcactormove->SetMovementSpeed(4);
        pcactormove->Backward(walk != 0);
#endif
      }
      PerformMovementAction();

      return true;
    }

    bool PlayerEntity::ActionLeft(iEvent& ev)
    {
      using namespace PT::Events;

      if (ready)
      {
        if (InputHelper::GetButtonDown(&ev)) turn = -1;
        else turn = 0;

#ifdef _MOVEMENT_DEBUG_CHARACTER_
        csRef<iPcActorMove> pcactormove = CEL_QUERY_PROPCLASS_ENT(other_self, iPcActorMove);
        pcactormove->SetRotationSpeed(3);
        pcactormove->RotateLeft(turn != 0);
#endif
      }
      PerformMovementAction();

      return true;
    }

    bool PlayerEntity::ActionRight(iEvent& ev)
    {
      using namespace PT::Events;

      if (ready)
      {
        if (InputHelper::GetButtonDown(&ev)) turn = 1;
        else turn = 0;

#ifdef _MOVEMENT_DEBUG_CHARACTER_
        csRef<iPcActorMove> pcactormove = CEL_QUERY_PROPCLASS_ENT(other_self, iPcActorMove);
        pcactormove->SetRotationSpeed(3);
        pcactormove->RotateRight(turn != 0);
#endif
      }
      PerformMovementAction();

      return true;
    }

    bool PlayerEntity::ActionToggleWalk(iEvent& ev)
    {
      using namespace PT::Events;

      if (ready)
      {
        if (InputHelper::GetButtonDown(&ev)) (walk == 0) ? walk = 1 : walk = 0;
      }
      PerformMovementAction();

      return true;
    }

    bool PlayerEntity::ActionToggleRun(iEvent& ev)
    {
      using namespace PT::Events;

      if (ready)
      {
        if (InputHelper::GetButtonDown(&ev))
        {
          if (run==false)
          {
            run = true;
            //Setup values needed for bobbing during run
            viewBobEffect.range = RUN_OFFSET_RANGE;
            viewBobEffect.period = RUN_PERIOD;
          }
          else
          {
            run = false;
            //Setup values needed for bobbing during walk
            viewBobEffect.range = WALK_OFFSET_RANGE;
            viewBobEffect.period = WALK_PERIOD;
          }
        }
      }
      PerformMovementAction();

      return true;
    }

  bool PlayerEntity::ActionPanUp(iEvent& ev)
    {
      using namespace PT::Events;

      if (!celEntity) return false;

      if (ready)
      {
        if (InputHelper::GetButtonDown(&ev))
        {
          if (invertYAxis) camera->SetPitchVelocity(-1.0f);
          else camera->SetPitchVelocity(1.0f);
        }
        else
        {
            camera->SetPitchVelocity(0.0f);
        }
      }

      return true;
    }

  bool PlayerEntity::ActionPanDown(iEvent& ev)
    {
      using namespace PT::Events;

      if (!celEntity) return false;

      if (ready)
      {
        if (InputHelper::GetButtonDown(&ev))
        {
          if (invertYAxis) camera->SetPitchVelocity(1.0f);
          else camera->SetPitchVelocity(-1.0f);
        }
        else
        {
            camera->SetPitchVelocity(0.0f);
        }
      }

      return true;
    }

  bool PlayerEntity::ActionToggleCamera(iEvent& ev)
    {
      using namespace PT::Events;

      if (ready)
      {
        if (InputHelper::GetButtonDown(&ev))
        {
          csRef<iPcActorMove> pcactormove =
            CEL_QUERY_PROPCLASS_ENT(celEntity, iPcActorMove);
          if (!pcactormove) return false;
          pcactormove->ToggleCameraMode();
        }
      }

      return true;
    }

  bool PlayerEntity::ActionToggleDistClipping(iEvent& ev)
    {
      using namespace PT::Events;

      if (ready)
      {
        if (InputHelper::GetButtonDown(&ev))
        {

          if (!celEntity) return false;

          PointerLibrary::getInstance()->getGUIManager()->GetChatWindow()->
            AddMessage("Toggled Distance Clipping.");

          if (camera->UseDistanceClipping()) camera->DisableDistanceClipping();
          else camera->EnableAdaptiveDistanceClipping(minFPS, maxFPS, minDistance);
        }
      }

      return true;
    }

    bool PlayerEntity::ActionActivateWeapon(iEvent& ev)
    {
      using namespace PT::Events;

      if (ready)
      {
        if (InputHelper::GetButtonDown(&ev))
        {
          PlayAnimation("attack_sword_s");
        }
      }

      return true;
    }

    bool PlayerEntity::ActionZoomIn(iEvent& ev)
    {
      using namespace PT::Events;

      if (InputHelper::GetButtonDown(&ev))
      {
        if (!celEntity) return false;
        cameraDistance -= 0.5;
        if (camera.IsValid()) camera->SetDistance(cameraDistance);
      }
      return true;
    }

    bool PlayerEntity::ActionZoomOut(iEvent& ev)
    {
      using namespace PT::Events;

      if (InputHelper::GetButtonDown(&ev))
      {
        if (!celEntity) return false;
        cameraDistance += 0.5;
        if (camera.IsValid()) camera->SetDistance(cameraDistance);
      }
      return true;
    }

    bool PlayerEntity::ActionJump(iEvent& ev)
    {
      using namespace PT::Events;

      if (InputHelper::GetButtonDown(&ev)) jump=true;

      PerformMovementAction();
      return true;
    }

    bool PlayerEntity::PerformMovementAction()
    {
      using namespace PT::Events;

      // Local movement option
      if(local_movement && instance && !static_cast<PcEntity*>(instance)->GetHasMount())
      {
        PT::Events::EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
        csRef<iEvent> entityEvent = evmgr->CreateEvent("entity.move");
        entityEvent->Add("entityId", id);
        entityEvent->Add("walkDirection", float(PointerLibrary::getInstance()->getStatManager()->GetStat("Speed")*walk*(char(run)+1)));
        entityEvent->Add("turnDirection", float(walk == -1 && backwardReverse ? -turn : turn));
        entityEvent->Add("run", run);
        entityEvent->Add("jump", jump);
        entityEvent->Add("local", true);
        evmgr->AddEvent(entityEvent);
      }

      MoveRequestMessage msg;

      msg.setWalk(walk+1);

      if (walk == -1 && backwardReverse) msg.setTurn(-turn+1);
      else msg.setTurn(turn+1);
      msg.setRun(run);
      msg.setJump(jump);
      jump=false; //Jumping is not a constant action, so we need to turn it off once we've
                  //sent the request.
      PointerLibrary::getInstance()->getNetwork()->send(&msg);

      //When we move, we turn off sitting.
        // Shouldn't this be done for mouse walk too?
      if (sitting)
      {
        PoseRequestMessage poseMsg;
        poseMsg.setPoseId(0); //TODO: do a posemanager lookup for "idle"!
        PointerLibrary::getInstance()->getNetwork()->send(&poseMsg);
      }

      return true;
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

    bool PlayerEntity::ActionMoveTo(iEvent& ev)
    {
      using namespace PT::Events;

      if (InputHelper::GetButtonDown(&ev))
      {
        if (!instance) return false;
        csRef<iPcDefaultCamera> pccamera = GetCamera();
        if (!pccamera) return false;
        csRef<iCamera> cam = pccamera->GetCamera();
        if (!cam) return false;

        csVector3 isect, untransfCoord;
        iSector* sector = 0;
        Cursor* cursor = PointerLibrary::getInstance()->getCursor();
        csRef<iMeshWrapper> mesh = cursor->Get3DPointFrom2D(cam, &isect, &untransfCoord, &sector);

        if (mesh)
        {
          PT::Effect::EffectsManager* effectsmanager = PointerLibrary::getInstance()->getEffectsManager();
          effectsmanager->CreateEffect("MoveMarker", isect+csVector3(0,0.01f,0), sector);
          //effectsmanager->CreateDecal(isect+csVector3(0,0.25,0));

          csRef<iCelEntity> ownent = GetCelEntity();
          if (!ownent) return false;
          csRef<iPcLinearMovement> pclinmove = CEL_QUERY_PROPCLASS_ENT(ownent, iPcLinearMovement);
          if (!pclinmove) return false;

          MoveToRequestMessage msg;
          msg.setTo(isect.x, isect.y, isect.z);
          msg.setRun(run);
          PointerLibrary::getInstance()->getNetwork()->send(&msg);

          Report(PT::Debug, "OnMouseDown: position: %s", isect.Description().GetData());
        }
        else
        {
          Report(PT::Warning, "OnMouseDown: Failed to find mesh!");
        }
      }

      return true;
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

    bool PlayerEntity::UpdateOptions(iEvent& ev)
    {
      return UpdateOptions();
    } // end UpdateOptions()

    bool PlayerEntity::UpdateOptions()
    {
      backwardReverse = app_cfg->GetBool("Client.backwardreverse", backwardReverse);
      local_movement = app_cfg->GetBool("Client.local_movement", false);
      invertYAxis = app_cfg->GetBool("Client.invertYAxis", invertYAxis);
      minFPS = app_cfg->GetFloat("Client.minFPS", minFPS);
      maxFPS = app_cfg->GetFloat("Client.maxFPS", maxFPS);
      minDistance = app_cfg->GetFloat("Client.minDistance", minDistance);

      return true;
    } // end UpdateOptions()

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
