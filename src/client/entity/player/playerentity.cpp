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
#include <ivideo/graph2d.h>
#include <iutil/plugin.h>

#include <physicallayer/pl.h>
#include <physicallayer/propfact.h>
#include <physicallayer/propclas.h>
#include <propclass/defcam.h>
#include <propclass/linmove.h>

#include "common/reporter/reporter.h"
#include "client/pointer/pointer.h"

#include "common/eventcs/eventmanager.h"
#include "common/eventcs/interfaceevent.h"
#include "common/eventcs/entityevent.h"

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


    PlayerEntity::PlayerEntity() : PcEntity()
    {
      type = Common::Entity::PlayerEntityType;

      iObjectRegistry* object_reg =
        PointerLibrary::getInstance()->getObjectRegistry();

      pl->CreatePropertyClass(celEntity, "pccamera.old");
      camera = CEL_QUERY_PROPCLASS_ENT(celEntity, iPcDefaultCamera);
      if (camera.IsValid())
      {
        //We will draw the camera as needed, in order for it to not overwrite
        //the GUI.
        camera->SetAutoDraw(false);
        camera->SetMode(iPcDefaultCamera::thirdperson, true);
        camera->SetPitch(-0.18f);

        csRef<iGraphics3D> g3d = csQueryRegistry<iGraphics3D> (object_reg);
        if (g3d)
        {
          iGraphics2D* g2d = g3d->GetDriver2D();
          camera->GetCamera()->SetViewportSize(g2d->GetWidth(), g2d->GetHeight());
        }
      }
      else
        Report(PT::Error, "Failed to get PcDefaultCamera for %s!(%d)", name.c_str(), id);



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
      PointerLibrary::getInstance()->setPlayer(0);
      delete instance;
    }

    void DontDelete(PlayerEntity*){}

    boost::shared_ptr<PlayerEntity> PlayerEntity::Instance()
    {
      //If the instance already exists, and we're not
      //requesting a reinitialization.
      if (instance) return boost::shared_ptr<PlayerEntity>(instance, DontDelete);

      instance = new PlayerEntity();
      return boost::shared_ptr<PlayerEntity>(instance, DontDelete);
    }

    void PlayerEntity::Initialize(const iEvent& ev)
    {
      PcEntity::Initialize(ev);

      Report(PT::Notify, "PlayerEntity::Initialize");
      celEntity->SetName("player");

      csRef<iObjectRegistry> obj_reg =
        PointerLibrary::getInstance()->getObjectRegistry();
      csRef<iWorld> world =  csQueryRegistry<iWorld> (obj_reg);
      world->EnterWorld(GetPosition());
    }

    iCamera* GetCam(iPcDefaultCamera* camera)
    {
      if (!camera) return 0;
      return camera->GetCamera();
    }

    iSector* PlayerEntity::GetSector()
    {
      iCamera* cam = GetCam(camera);
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

    void PlayerEntity::Teleport(const WFMath::Point<3>& pos,
                                float rotation,
                                const std::string& sector)
    {
      if (!celEntity.IsValid()) return;

      csRef<iObjectRegistry> obj_reg = PointerLibrary::getInstance()->getObjectRegistry();
      csRef<iWorld> world =  csQueryRegistry<iWorld> (obj_reg);

      world->EnterWorld(pos);
      this->SetFullPosition(pos,rotation,sector);
    }

  } //Entity namespace
} //PT namespace
