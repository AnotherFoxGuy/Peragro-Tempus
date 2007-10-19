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

#include "client/event/eventmanager.h"
#include "client/event/interfaceevent.h"
#include "client/event/entityevent.h"
#include "client/event/inputevent.h"
#include "common/network/entitymessages.h"
#include "client/network/network.h"
#include "client/gui/guimanager.h"
#include "client/gui/chat-gui.h"

//These defines should probably go to configuration file
#define WALK_PPS 0.01f
#define WALK_PITCH_RANGE 0.02f
#define RUN_PPS 0.06f
#define RUN_PITCH_RANGE 0.01f

namespace PT
{

  namespace Entity
  {

    PlayerEntity::PlayerEntity(const Events::EntityAddEvent& ev) : PcEntity(ev)
    {
      Create();

      walk = 0;
      turn = 0;
      run = false;
      jump = false;
      ready = true; //This should be replaced by some external StateManager. We already have things like this in several places
      cameraDistance = 3.0;
      pitchDirection = 1;
      pitchPerSecond = WALK_PPS;
      pitchRange = WALK_PITCH_RANGE;
      currentPitch = 0.0f;

      //Register actions for events
      using namespace PT::Events;
      // Register listener for ActionForward.
      EventHandler<PlayerEntity>* cbActionForward = new EventHandler<PlayerEntity>(&PlayerEntity::ActionForward, this);
      PointerLibrary::getInstance()->getEventManager()->AddListener("input.ACTION_FORWARD", cbActionForward);

      // Register listener for ActionBackward.
      EventHandler<PlayerEntity>* cbActionBackward = new EventHandler<PlayerEntity>(&PlayerEntity::ActionBackward, this);
      PointerLibrary::getInstance()->getEventManager()->AddListener("input.ACTION_BACKWARD", cbActionBackward);

      // Register listener for ActionLeft.
      EventHandler<PlayerEntity>* cbActionLeft = new EventHandler<PlayerEntity>(&PlayerEntity::ActionLeft, this);
      PointerLibrary::getInstance()->getEventManager()->AddListener("input.ACTION_LEFT", cbActionLeft);

      // Register listener for ActionRight.
      EventHandler<PlayerEntity>* cbActionRight = new EventHandler<PlayerEntity>(&PlayerEntity::ActionRight, this);
      PointerLibrary::getInstance()->getEventManager()->AddListener("input.ACTION_RIGHT", cbActionRight);

      // Register listener for ActionToggleWalk.
      EventHandler<PlayerEntity>* cbActionToggleWalk = new EventHandler<PlayerEntity>(&PlayerEntity::ActionToggleWalk, this);
      PointerLibrary::getInstance()->getEventManager()->AddListener("input.ACTION_TOGGLEWALK", cbActionToggleWalk);

      // Register listener for ActionToggleRun.
      EventHandler<PlayerEntity>* cbActionToggleRun = new EventHandler<PlayerEntity>(&PlayerEntity::ActionToggleRun, this);
      PointerLibrary::getInstance()->getEventManager()->AddListener("input.ACTION_TOGGLERUN", cbActionToggleRun);

      // Register listener for ActionPanUp.
      EventHandler<PlayerEntity>* cbActionPanUp = new EventHandler<PlayerEntity>(&PlayerEntity::ActionPanUp, this);
      PointerLibrary::getInstance()->getEventManager()->AddListener("input.ACTION_PANUP", cbActionPanUp);

      // Register listener for ActionPanDown.
      EventHandler<PlayerEntity>* cbActionPanDown = new EventHandler<PlayerEntity>(&PlayerEntity::ActionPanDown, this);
      PointerLibrary::getInstance()->getEventManager()->AddListener("input.ACTION_PANDOWN", cbActionPanDown);

      // Register listener for ActionToggleCamera.
      EventHandler<PlayerEntity>* cbActionToggleCamera = new EventHandler<PlayerEntity>(&PlayerEntity::ActionToggleCamera, this);
      PointerLibrary::getInstance()->getEventManager()->AddListener("input.ACTION_TOGGLECAMERA", cbActionToggleCamera);

      // Register listener for ActionToggleDistClipping.
      EventHandler<PlayerEntity>* cbActionToggleDistClipping = new EventHandler<PlayerEntity>(&PlayerEntity::ActionToggleDistClipping, this);
      PointerLibrary::getInstance()->getEventManager()->AddListener("input.ACTION_TOGGLEDISTCLIP", cbActionToggleDistClipping);

      // Register listener for ActionActivateWeapon.
      EventHandler<PlayerEntity>* cbActionActivateWeapon = new EventHandler<PlayerEntity>(&PlayerEntity::ActionActivateWeapon, this);
      PointerLibrary::getInstance()->getEventManager()->AddListener("input.ACTION_ACTIVATEWEAPON", cbActionActivateWeapon);

      // Register listener for ActionZoomIn.
      EventHandler<PlayerEntity>* cbActionZoomIn = new EventHandler<PlayerEntity>(&PlayerEntity::ActionZoomIn, this);
      PointerLibrary::getInstance()->getEventManager()->AddListener("input.ACTION_ZOOMIN", cbActionZoomIn);

      // Register listener for ActionZoomOut.
      EventHandler<PlayerEntity>* cbActionZoomOut = new EventHandler<PlayerEntity>(&PlayerEntity::ActionZoomOut, this);
      PointerLibrary::getInstance()->getEventManager()->AddListener("input.ACTION_ZOOMOUT", cbActionZoomOut);

      // Register listener for ActionJump.
      EventHandler<PlayerEntity>* cbActionJump = new EventHandler<PlayerEntity>(&PlayerEntity::ActionJump, this);
      PointerLibrary::getInstance()->getEventManager()->AddListener("input.ACTION_JUMP", cbActionJump);
    }

    PlayerEntity* PlayerEntity::Instance(const Events::EntityAddEvent* ev)
    {
    static PlayerEntity* instance=0;

    //If the instance already exists, and we're not
    //requesting a reinitialization.
    if (instance && !ev) return instance;
    //If the instance already exists, and we're requesting
    //a reinitialization
    else if (instance && ev) delete instance;
    //We can't initialize without a valid event
    if (!ev) return 0;

    instance = new PlayerEntity(*ev);
    return instance;
    }

    void PlayerEntity::Create()
    {
      csRef<iObjectRegistry> obj_reg = PointerLibrary::getInstance()->getObjectRegistry();
      csRef<iCelPlLayer> pl =  csQueryRegistry<iCelPlLayer> (obj_reg);

      //At this time PlayerEntity's Create() method takes care of the appropriate
      //stuff, so we only need to setup the camera.
      pl->CreatePropertyClass(celentity, "pccamera.old");

      camera = CEL_QUERY_PROPCLASS_ENT(celentity, iPcDefaultCamera);
      if (camera.IsValid())
      {
        camera->SetAutoDraw(false); //We will draw the camera as needed, in order for it to not overwrite the GUI
        camera->SetMode(iPcDefaultCamera::thirdperson, true);
        camera->SetPitch(-0.18f);
      }
      else
        Report(PT::Error, "Failed to get PcDefaultCamera for %s!(%d)", name.GetData(), id);
    }

    bool PlayerEntity::ActionForward(PT::Events::Eventp ev)
    {
      using namespace PT::Events;

      if (ready)
      {
        InputEvent* inputEv = GetInputEvent<InputEvent*>(ev);
        if (!inputEv) return false;

        if (!inputEv->released) walk = 1;
        else walk = 0;
      }
      PerformMovementAction();

      return true;
    }

    bool PlayerEntity::ActionBackward(PT::Events::Eventp ev)
    {
      using namespace PT::Events;

      if (ready)
      {
        InputEvent* inputEv = GetInputEvent<InputEvent*>(ev);
        if (!inputEv) return false;

        if (!inputEv->released) walk = -1;
        else walk = 0;
      }
      PerformMovementAction();

      return true;
    }

    bool PlayerEntity::ActionLeft(PT::Events::Eventp ev)
    {
      using namespace PT::Events;

      if (ready)
      {
        InputEvent* inputEv = GetInputEvent<InputEvent*>(ev);
        if (!inputEv) return false;

        if (!inputEv->released) turn = -1;
        else turn = 0;
      }
      PerformMovementAction();

      return true;
    }

    bool PlayerEntity::ActionRight(PT::Events::Eventp ev)
    {
      using namespace PT::Events;

      if (ready)
      {
        InputEvent* inputEv = GetInputEvent<InputEvent*>(ev);
        if (!inputEv) return false;

        if (!inputEv->released) turn = 1;
        else turn = 0;
      }
      PerformMovementAction();

      return true;
    }

    bool PlayerEntity::ActionToggleWalk(PT::Events::Eventp ev)
    {
      using namespace PT::Events;

      if (ready)
      {
        InputEvent* inputEv = GetInputEvent<InputEvent*>(ev);
        if (!inputEv) return false;

        if (!inputEv->released) (walk == 0) ? walk = 1 : walk = 0;
      }
      PerformMovementAction();

      return true;
    }

    bool PlayerEntity::ActionToggleRun(PT::Events::Eventp ev)
    {
      using namespace PT::Events;

      if (ready)
      {
        InputEvent* inputEv = GetInputEvent<InputEvent*>(ev);
        if (!inputEv) return false;

        if (!inputEv->released)
        {
          if (run==false)
          {
            run = true;
            //Setup values needed for hopping during run
            pitchRange = RUN_PITCH_RANGE;
            pitchPerSecond = RUN_PPS;
          }
          else
          {
            run = false;
            //Setup values needed for hopping during walk
            pitchRange = WALK_PITCH_RANGE;
            pitchPerSecond = WALK_PPS;
          }
        }
      }
      PerformMovementAction();

      return true;
    }

  bool PlayerEntity::ActionPanUp(PT::Events::Eventp ev)
    {
      using namespace PT::Events;

      if (ready)
      {
        InputEvent* inputEv = GetInputEvent<InputEvent*>(ev);
        if (!inputEv) return false;

        if (!inputEv->released)
        {
          if (!celentity) return false;
          camera->SetPitch(camera->GetPitch()-0.1f);
        }
      }

      return true;
    }

  bool PlayerEntity::ActionPanDown(PT::Events::Eventp ev)
    {
      using namespace PT::Events;

      if (ready)
      {
        InputEvent* inputEv = GetInputEvent<InputEvent*>(ev);
        if (!inputEv) return false;

        if (!inputEv->released)
        {
          if (!celentity) return false;
          camera->SetPitch(camera->GetPitch()+0.1f);
        }
      }

      return true;
    }

  bool PlayerEntity::ActionToggleCamera(PT::Events::Eventp ev)
    {
      using namespace PT::Events;

      if (ready)
      {
        InputEvent* inputEv = GetInputEvent<InputEvent*>(ev);
        if (!inputEv) return false;

        if (!inputEv->released)
        {
          csRef<iPcActorMove> pcactormove = CEL_QUERY_PROPCLASS_ENT(celentity, iPcActorMove);
          if (!pcactormove) return false;
          pcactormove->ToggleCameraMode();
        }
      }

      return true;
    }

  bool PlayerEntity::ActionToggleDistClipping(PT::Events::Eventp ev)
    {
      using namespace PT::Events;

      if (ready)
      {
        InputEvent* inputEv = GetInputEvent<InputEvent*>(ev);
        if (!inputEv) return false;

        if (!inputEv->released)
        {

          if (!celentity) return false;

          PointerLibrary::getInstance()->getGUIManager()->GetChatWindow()->AddMessage("Toggled Distance Clipping.");

          if (camera->UseDistanceClipping()) camera->DisableDistanceClipping();
          else camera->EnableAdaptiveDistanceClipping(95, 100, 50);
        }
      }

      return true;
    }

    bool PlayerEntity::ActionActivateWeapon(PT::Events::Eventp ev)
    {
      using namespace PT::Events;

      if (ready)
      {
        InputEvent* inputEv = GetInputEvent<InputEvent*>(ev);
        if (!inputEv) return false;

        if (!inputEv->released)
        {
          PlayAnimation("attack_sword_s");
        }
      }

      return true;
    }

    bool PlayerEntity::ActionZoomIn(PT::Events::Eventp ev)
    {
      using namespace PT::Events;

      InputEvent* inputEv = GetInputEvent<InputEvent*>(ev);
      if (!inputEv) return false;

      if (!inputEv->released)
      {
        if (!celentity) return false;
        cameraDistance -= 0.5;
        if (camera.IsValid()) camera->SetDistance(cameraDistance);
      }
      return true;
    }

    bool PlayerEntity::ActionZoomOut(PT::Events::Eventp ev)
    {
      using namespace PT::Events;
      InputEvent* inputEv = GetInputEvent<InputEvent*>(ev);
      if (!inputEv) return false;

      if (!inputEv->released)
      {
        if (!celentity) return false;
        cameraDistance += 0.5;
        if (camera.IsValid()) camera->SetDistance(cameraDistance);
      }
      return true;
    }

    bool PlayerEntity::ActionJump(PT::Events::Eventp ev)
    {
      using namespace PT::Events;
      InputEvent* inputEv = GetInputEvent<InputEvent*>(ev);

      if (!inputEv) return false;

      if (!inputEv->released) jump=true;

      PerformMovementAction();
      return true;
    }

    bool PlayerEntity::PerformMovementAction()
    {
      MoveRequestMessage msg;

      msg.setWalk(walk+1);

      if (walk == -1) msg.setTurn(-turn+1);
      else msg.setTurn(turn+1);

      msg.setRun(run);
      msg.setJump(jump);
      //Jumping is not a constant action, so we need to turn it off once we've
      //sent the request.
      jump=false;
      PointerLibrary::getInstance()->getNetwork()->send(&msg);

      //When we move, we turn off sitting.
      if (sitting)
      {
        PoseRequestMessage poseMsg;
        poseMsg.setPoseId(0); //TODO: do a posemanager lookup for "idle"!
        PointerLibrary::getInstance()->getNetwork()->send(&poseMsg);
      }

      return true;
    }
    void PlayerEntity::CameraDraw(unsigned int fpsLimit)
    {
      if (!camera.IsValid()) return;

      //If moving in first person view, add the hopping effect
      //TODO: It would be nice to have the screen move a bit to left and to right as well
      //TODO: It would be nice to implement our own camera mode that will perform this.
      if (camera->GetMode()==iPcDefaultCamera::firstperson && walk!=0)
      {
        if (fpsLimit==0) fpsLimit=1000; //divide by zero problem
        //Our pitch moves in range [-pitchRange; pitchRange]. When we break
        //through those values, change the direction of pitch change.
        if (currentPitch >= pitchRange) pitchDirection = -1;
        else if (currentPitch <= -pitchRange) pitchDirection = 1;

        //Pitch up or pitch down, depending on pitch direction.
        currentPitch+=pitchDirection*pitchPerSecond/(float) fpsLimit;

        //Update the camera pitch finally
        camera->SetPitch(currentPitch);
      }
      camera->Draw();
    }

    void PlayerEntity::Interact()
    {
    }

  } //Entity namespace
} //PT namespace
