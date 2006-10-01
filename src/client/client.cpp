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

#include "client.h"

#include "cursor.h"

#include "CEGUI.h"
#include "CEGUIWindowManager.h" 
#include "CEGUILogger.h"

#include "csgeom/path.h"
#include "cstool/initapp.h"
#include "cstool/csview.h"
#include "csutil/cmdhelp.h"
#include "csutil/cmdline.h"
#include "csutil/csstring.h"
#include "csutil/csshlib.h"
#include "csutil/event.h"
#include "csutil/sysfunc.h"
#include "csutil/syspath.h"
#include "iengine/camera.h"
#include "iengine/campos.h"
#include "iengine/mesh.h"
#include "iengine/sector.h"
#include "iengine/texture.h"
#include "iengine/material.h"
#include "iengine/scenenode.h"
#include "iengine/movable.h"
#include "imesh/object.h"
#include "imesh/spritecal3d.h"
#include "imesh/sprite2d.h"
#include "imesh/genmesh.h"
//#include "imesh/objmodel.h"
#include "iutil/databuff.h"
#include "iutil/event.h"
#include "iutil/eventq.h"
#include "iutil/object.h"
#include "iutil/vfs.h"
#include "ivaria/collider.h"
#include "ivideo/graph2d.h"
#include "ivideo/natwin.h"
#include "ivideo/txtmgr.h"
#include "ivideo/material.h"

#include "physicallayer/pl.h"
#include "physicallayer/propfact.h"
#include "physicallayer/propclas.h"
#include "physicallayer/entity.h"
#include "physicallayer/persist.h"
#include "celtool/initapp.h"
#include "celtool/persisthelper.h"
#include "celtool/stdparams.h"
#include "behaviourlayer/bl.h"
#include "propclass/test.h"
#include "propclass/mesh.h"
#include "propclass/meshsel.h"
#include "propclass/solid.h"
#include "propclass/inv.h"
#include "propclass/chars.h"
#include "propclass/move.h"
#include "propclass/prop.h"
#include "propclass/tooltip.h"
#include "propclass/defcam.h"
#include "propclass/gravity.h"
#include "propclass/timer.h"
#include "propclass/region.h"
#include "propclass/input.h"
#include "propclass/navgraph.h"
#include "propclass/linmove.h"
#include "propclass/actormove.h"
#include "propclass/quest.h"
#include "propclass/trigger.h"
#include "propclass/zone.h"
#include "propclass/sound.h"
#include <propclass/colldet.h>

#include "client/network/network.h"
#include "client/gui/gui.h"
#include "client/gui/guimanager.h"
#include "client/effects/effectsmanager.h"
#include "client/combat/combatmanager.h"
#include "client/item/itemmanager.h"

//#include "common/entity/entity.h"

#include "common/util/wincrashdump.h"

CS_IMPLEMENT_APPLICATION

Client::Client() : playing(false)
{
  SetApplicationName ("Client");
  state = STATE_INITIAL;
  walk = 0;
  turn = 0;
  timer = 0;
  limitFPS = 0;
  last_sleep = 0;
  world_loaded = false;
  cameradistance = 3;

  network = 0;
  cursor = 0;

  last_seen = 0;
}

Client::~Client()
{
  delete combatmanager;
  delete entitymanager;
  delete effectsmanager;
  delete guimanager;
  delete network;
  delete cursor;
  delete itemmanager;
}

void Client::PreProcessFrame()
{
  csTicks ticks = vc->GetElapsedTicks();
  timer += ticks;

  effectsmanager->HandleEffects(ticks);

  if (limitFPS > 0)
  {
    if (ticks < 1000.0f/limitFPS)
      csSleep((int)1000.0f/limitFPS - ticks);
  }

  if (timer > 1000)
  {
    timer = 0;
    if (state == STATE_PLAY)
    {
      entitymanager->DrUpdateOwnEntity();
    }
  }
}

void Client::ProcessFrame()
{
  handleStates();
}

void Client::PostProcessFrame()
{
  // Paint the interface over the engine
  guimanager->Render ();
  cursor->Draw();
}

void Client::FinishFrame()
{
  g3d->FinishDraw();
  g3d->Print(0);
}

bool Client::OnInitialize(int argc, char* argv[])
{
  setenv("APPDIR", csInstallationPathsHelper::GetAppDir(argv[0]), true);

  if (!csInitializer::SetupConfigManager(GetObjectRegistry(),
    "/peragro/config/client.cfg", GetApplicationName()))
    return ReportError("Failed to initialize configuration manager!");

  if (!celInitializer::RequestPlugins(GetObjectRegistry(),
    CS_REQUEST_REPORTER,
    CS_REQUEST_REPORTERLISTENER,
    CS_REQUEST_END))
    return ReportError("Failed to initialize plugins!");

  csBaseEventHandler::Initialize(GetObjectRegistry());

  return true;
}

void Client::OnExit()
{
}

bool Client::Application()
{
  if (!OpenApplication(GetObjectRegistry()))
    return ReportError("Error opening system!");

  vfs = CS_QUERY_REGISTRY(GetObjectRegistry(), iVFS);
  if (!vfs) return ReportError("Failed to locate VFS!");

  g3d = CS_QUERY_REGISTRY(GetObjectRegistry(), iGraphics3D);
  if (!g3d) return ReportError("Failed to locate 3D renderer!");

  // Create and Initialize the Network. 
  network = new Network (this);
  network->init();

  // Create and Initialize the ItemManager.
  itemmanager = new ItemMGR (GetObjectRegistry());
  if (!itemmanager->Initialize())
    return false;

  // Create and Initialize the GUImanager.
  guimanager = new GUIManager (this);
  if (!guimanager->Initialize (GetObjectRegistry()))
    return false;

  if (!RegisterQueue(GetObjectRegistry(), csevAllEvents(GetObjectRegistry())))
    return ReportError("Failed to set up event handler!");

  engine = CS_QUERY_REGISTRY(GetObjectRegistry(), iEngine);
  if (!engine) return ReportError("Failed to locate 3D engine!");

  vc = CS_QUERY_REGISTRY(GetObjectRegistry(), iVirtualClock);
  if (!vc) return ReportError("Failed to locate Virtual Clock!");

  //kbd = CS_QUERY_REGISTRY(GetObjectRegistry(), iKeyboardDriver);
  //if (!kbd) return ReportError("Failed to locate Keyboard Driver!");

  cmdline = CS_QUERY_REGISTRY(GetObjectRegistry(), iCommandLineParser);
  if (!cmdline) return ReportError("Failed to locate CommandLineParser plugin");

  sndrenderer = CS_QUERY_REGISTRY(GetObjectRegistry(), iSndSysRenderer);
  if (!sndrenderer) return ReportError("Failed to locate sound renderer!");

  sndloader = CS_QUERY_REGISTRY(GetObjectRegistry(), iSndSysLoader);
  if (!sndloader) return ReportError("Failed to locate sound loader!");

  app_cfg = CS_QUERY_REGISTRY (GetObjectRegistry(), iConfigManager);
  if (!app_cfg) return ReportError("Can't find the config manager!");

  iNativeWindow* nw = g3d->GetDriver2D()->GetNativeWindow ();
  if (nw) nw->SetTitle ("Peragro Tempus");

  InitializeCEL();

  // Create the cursor.
  cursor = new Cursor(this);

  // Create and Initialize the EntityManager.
  entitymanager = new ptEntityManager (GetObjectRegistry(), this);
  if (!entitymanager->Initialize())
    return false;

  // Create and Initialize the Effectsmanager.
  effectsmanager = new EffectsManager (GetObjectRegistry());
  if (!effectsmanager->Initialize())
    return false;

  // Create and Initialize the Combatmanager.
  combatmanager = new CombatMGR (this);
  if (!combatmanager->Initialize())
    return false;

  view.AttachNew(new csView(engine, g3d));

  // intro sound
    const char* fname = "/peragro/art/audio/music/intro/peragrotempus.ogg";

  csRef<iDataBuffer> soundbuf = vfs->ReadFile (fname);
  if (!soundbuf)
    return ReportError ("Can't load file '%s'!", fname);

  csRef<iSndSysData> snddata = sndloader->LoadSound (soundbuf);
  if (!snddata)
    return ReportError ("Can't load sound '%s'!", fname);

  sndstream = sndrenderer->CreateStream (snddata,
  	CS_SND3D_ABSOLUTE);
  if (!sndstream)
    return ReportError ("Can't create stream for '%s'!", fname);

  sndsource = sndrenderer->CreateSource (sndstream);
  if (!sndsource)
    return ReportError ("Can't create source for '%s'!", fname);
  sndsource3d = SCF_QUERY_INTERFACE (sndsource, iSndSysSourceSoftware3D);

  sndsource3d->SetPosition (csVector3(0,0,0));
  sndsource3d->SetVolume (1.0f);

  sndstream->SetLoopState (CS_SNDSYS_STREAM_LOOP);
  sndstream->Unpause ();

  // end intro sound
  csRef<iLoader >loader = CS_QUERY_REGISTRY(GetObjectRegistry(), iLoader);
  if (!loader) return ReportError("Failed to locate Loader!");
  loader->LoadLibraryFile("/peragro/xml/quests/doorquests.xml");

  Run();

  return true;
}

void Client::handleStates()
{
  switch(state)
  {
    case STATE_INITIAL: // Initial state. Load intro sector and go to STATE_INTRO.
    {
      // Load introduction sector, draw it once for this frame, and switch to STATE_INTRO
      const char* path = cmdline->GetOption("intro");

      if (!path) 
      {
        path = "/intro";
      }
      else
      {
        if (!strncmp(path,"void", 4)) path = 0;
      }

      printf("handleStates: Loading Intro: %s\n", path);

      bool hasIntro = false;
      csRef<iPcRegion> pcregion = 0;

      if (path) 
      {
        csRef<iCelEntity> entity = pl->CreateEntity();
        entity->SetName("ptIntroWorld");
        pl->CreatePropertyClass(entity, "pcregion");
        pcregion = CEL_QUERY_PROPCLASS_ENT(entity, iPcRegion);
        pcregion->SetRegionName("world");
        pcregion->SetWorldFile (path, "world");
        hasIntro = pcregion->Load();
        printf("handleStates: Loading Intro %s\n", hasIntro?"succeeded":"failed");
      }

      if (hasIntro)
      {
        csVector3 sPos = pcregion->GetStartPosition();
        view->GetCamera()->SetSector(pcregion->GetStartSector());
        view->GetCamera()->GetTransform().Translate(sPos);
        const char* secName = pcregion->GetStartSector()->QueryObject()->GetName();
        printf("handleStates: Setting up Camera at %s <%.2f,%.2f,%.2f>\n", secName, sPos.x, sPos.y, sPos.z);
      }
      else
      {
        printf("handleStates: Clearing screen due to missing intro\n");
        engine->SetClearScreen(true);
      }

      // Create the connection and option window.
      guimanager->CreateConnectWindow ();
      guimanager->CreateOptionsWindow ();
      guimanager->CreateWhisperWindow();

      //guimanager->CreateStatusWindow ();
      //guimanager->CreateInventoryWindow ();
      //guimanager->GetInventoryWindow()->ShowWindow();
      //playing = true;
      //guimanager->CreateHUDWindow();
      //guimanager->CreateBuddyWindow();
      //guimanager->CreateSelectCharWindow();
      //guimanager->GetSelectCharWindow()->ShowWindow();
      guimanager->CreateNpcDialogWindow();

      if (cmdline)
      {
        const char* host = cmdline->GetOption("host");
        if (host)
        {
          ConnectRequestMessage msg;
          SocketAddress addr = Socket::getSocketAddress(host, 12345);
          network->setServerAddress(addr);
          network->send(&msg);
        }
      }

      state = STATE_INTRO;
      break;
    }
    case STATE_INTRO: // Introduction screen already loaded. Once user connects switch to STATE_CONNECTED.
    case STATE_CONNECTED: // Just connected. Wait asking for login and switch to STATE_LOGGED_IN
    case STATE_LOGGED_IN: // Login completed. Load items and switch to STATE_SELECTING_CHAR
    case STATE_SELECTING_CHAR: // Wait till user selects his character. Then create player mesh and switch to STATE_PLAY.
    {
      checkConnection();
      g3d->BeginDraw (engine->GetBeginDrawFlags () | CSDRAW_3DGRAPHICS);
      view->Draw();
      break;
    }
    case STATE_PLAY:
    {
      checkConnection();
      loadRegion();
      entitymanager->Handle();

      chat();
      break;
    }
  }
}

void Client::checkConnection()
{
  if (last_seen > 0 && csGetTicks() - last_seen > 10000)
  {
    last_seen = 0;
    printf("Disconnect!\n");
    // 10 seconds of no response... disconnect!
  }
}

void Client::connected ()
{
  guimanager->CreateLoginWindow ();
  guimanager->GetConnectWindow ()->HideWindow ();
  guimanager->GetLoginWindow ()->ShowWindow ();

  state = STATE_CONNECTED;

  if (cmdline)
  {
    char* user = (char* )cmdline->GetOption("user", 0);
    char* pass = (char* )cmdline->GetOption("pass", 0);

    if (user && pass)
    {
      if (cmdline->GetBoolOption("register", false))
      {
        RegisterRequestMessage reg_msg;
        reg_msg.setUsername(ptString(user, strlen(user)));
        reg_msg.setPassword(pass);
        network->send(&reg_msg);
      }

      LoginRequestMessage answer_msg;
      answer_msg.setUsername(ptString(user, strlen(user)));
      answer_msg.setPassword(pass);
      //this->name = user;
      network->send(&answer_msg);
    }
  }

  printf("Connected!!\n");
}


bool Client::OnKeyboard(iEvent& ev)
{
  csKeyEventType eventtype = csKeyEventHelper::GetEventType(&ev);
  if (eventtype == csKeyEventTypeDown)
  {
    utf32_char code = csKeyEventHelper::GetCookedCode(&ev);
    if (code == CSKEY_ESC)
    {
      csRef<iEventQueue> q = csQueryRegistry<iEventQueue> (GetObjectRegistry());
      if (q.IsValid()) q->GetEventOutlet()->Broadcast(csevQuit(GetObjectRegistry()));
    }

    if (playing)
    {
      if (csKeyEventHelper::GetAutoRepeat(&ev)) return false;

      utf32_char code = csKeyEventHelper::GetCookedCode(&ev);
  
      if (code == CSKEY_UP)
      {
        walk = 1;
      }
      else if (code == CSKEY_DOWN)
      {
        walk = -1;
      }
      else if (code == CSKEY_LEFT)
      {
        turn = -1;
      }
      else if (code == CSKEY_RIGHT)
      {
        turn = 1;
      }
      else if (code == CSKEY_SPACE)
      {
        (walk == 0) ? walk = 1 : walk = 0;
      }
      else if (code == CSKEY_PGUP)
      {
        iCelEntity* entity = entitymanager->getOwnEntity();
        if (!entity) return false;
        csRef<iPcDefaultCamera> pccamera = CEL_QUERY_PROPCLASS_ENT(entity, iPcDefaultCamera);  
        pccamera->SetPitch(pccamera->GetPitch()-0.1f);
      }
      else if (code == CSKEY_PGDN)
      {
        iCelEntity* entity = entitymanager->getOwnEntity();
        if (!entity) return false;
        csRef<iPcDefaultCamera> pccamera = CEL_QUERY_PROPCLASS_ENT(entity, iPcDefaultCamera);
        pccamera->SetPitch(pccamera->GetPitch()+0.1f);
      }
      else if (code == 'c')
      {
        iPcActorMove* pcactormove = getPcActorMove();
        if (!pcactormove) return false;
        pcactormove->ToggleCameraMode();
      }
      else if (code == 'd')
      {
        iCelEntity* entity = entitymanager->getOwnEntity();
        if (!entity) return false;
        csRef<iPcDefaultCamera> pccamera = CEL_QUERY_PROPCLASS_ENT(entity, iPcDefaultCamera);
        guimanager->GetChatWindow()->AddMessage("Toggled Distance Clipping.");
        pccamera->UseDistanceClipping() ? 
          pccamera->DisableDistanceClipping() 
          : pccamera->EnableAdaptiveDistanceClipping(95, 100, 50);
      }
      else if (code == 'g')
      {
        guimanager->GetInventoryWindow()->AddItem(2, 3);
      }
      else if (code == 'f')
      {
        iCelEntity* entity = cursor->getSelectedEntity();
        if (!entity) return false;
        csRef<iPcMesh> pcmesh = CEL_QUERY_PROPCLASS_ENT(entity, iPcMesh);
        if (!pcmesh) return false;
        csRef<iMeshWrapper> parent = pcmesh->GetMesh();
        if (!parent) return false;

        entitymanager->SetMaskColor(parent, "decalcolor", csVector4(0,0,1,1));
      }
      else if (code == 'h')
      {
        combatmanager->hit (entitymanager->GetOwnId(), 20);
      }
      else if (code == 'j')
      {
        // Activate the skill
        csRef<iCelEntity> ent = cursor->getSelectedEntity();
        csRef<iPcProperties> pcprop;
        if (ent) pcprop = CEL_QUERY_PROPCLASS_ENT(ent, iPcProperties);
        if (!pcprop) return false;
        if (pcprop->GetPropertyLong(pcprop->GetPropertyIndex("Entity Type")) == Entity::PlayerEntity)
        {
          combatmanager->levelup(pcprop->GetPropertyLong(pcprop->GetPropertyIndex("Entity ID")));
        }
        // Animate the player.
        iCelEntity* entity = entitymanager->getOwnEntity();
        if (!entity) return false;
        csRef<iPcMesh> pcmesh = CEL_QUERY_PROPCLASS_ENT(entity, iPcMesh);
        if (!pcmesh) return false;
        csRef<iMeshWrapper> parent = pcmesh->GetMesh();
        if (!parent) return false;
        csRef<iSpriteCal3DState> cal3dstate = SCF_QUERY_INTERFACE (parent->GetMeshObject(), iSpriteCal3DState);
        if (!cal3dstate) return false;
        cal3dstate->SetAnimAction("cast_summon", 0.0f, 0.0f);
      }
      else if (code == 'k')
      {
        iCelEntity* entity = entitymanager->getOwnEntity();
        if (!entity) return false;
        csRef<iPcMesh> pcmesh = CEL_QUERY_PROPCLASS_ENT(entity, iPcMesh);
        if (!pcmesh) return false;
        csRef<iMeshWrapper> parent = pcmesh->GetMesh();
        if (!parent) return false;
        csRef<iSpriteCal3DState> cal3dstate = SCF_QUERY_INTERFACE (parent->GetMeshObject(), iSpriteCal3DState);
        if (!cal3dstate) return false;
        cal3dstate->SetAnimAction("attack_sword_s", 0.0f, 0.0f);
      }
      else
      {
        return false;
      }
      MoveEntityRequestMessage msg;
      msg.setWalk(walk);
      msg.setRot(turn);
      network->send(&msg);
    }
  }
  else if (eventtype == csKeyEventTypeUp)
  {
    if (playing)
    {
      utf32_char code = csKeyEventHelper::GetCookedCode(&ev);
      if (code == CSKEY_UP)
      {
        walk = 0;
      }
      else if (code == CSKEY_DOWN)
      {
        walk = 0;
      }
      else if (code == CSKEY_LEFT)
      {
        turn = 0;
      }
      else if (code == CSKEY_RIGHT)
      {
        turn = 0;
      }
      else
      {
        return false;
      }
      MoveEntityRequestMessage msg;
      msg.setWalk(walk);
      msg.setRot(turn);
      network->send(&msg);
    }
  }

  return false;
}

bool Client::OnMouseDown(iEvent& ev)
{
  if (playing)
  {
    csMouseEventType mouseevent = csMouseEventHelper::GetEventType(&ev);
    if (mouseevent == csMouseEventTypeDown)
    {
      switch(csMouseEventHelper::GetButton(&ev))
      {
      case csmbLeft:
        {
          csRef<iCamera> cam = entitymanager->getOwnCamera();
          if (!cam) return false;

          csVector3 isect, untransfCoord;
          csRef<iMeshWrapper> mesh = cursor->Get3DPointFrom2D(csMouseEventHelper::GetX(&ev), 
            csMouseEventHelper::GetY(&ev), 
            cam, &isect, &untransfCoord);

          if (mesh)
          {
            effectsmanager->CreateEffect(EffectsManager::MoveMarker, isect+csVector3(0,0.25,0));

            csRef<iCelEntity> ownent = entitymanager->getOwnEntity();
            if (!ownent) return false;
            csRef<iPcLinearMovement> pclinmove = CEL_QUERY_PROPCLASS_ENT(ownent, iPcLinearMovement);
            if (!pclinmove) return false;

            MoveEntityToRequestMessage msg;
            msg.setPos(isect.x, isect.y, isect.z);
            network->send(&msg);

            printf("OnMouseDown: position: %s\n", isect.Description().GetData());
          }
          else
          {
            printf("OnMouseDown: Failed to find mesh!\n");
          }
          return true;
          break;
        }

      case csmbRight:
        {
          csRef<iCelEntity> ent = cursor->getSelectedEntity();

          csRef<iPcProperties> pcprop;
          if (ent) pcprop = CEL_QUERY_PROPCLASS_ENT(ent, iPcProperties);
          if (!pcprop) return false;

          // If it's an item, request a pickup.
          if (pcprop->GetPropertyLong(pcprop->GetPropertyIndex("Entity Type")) == Entity::ItemEntity)
          {
            PickEntityRequestMessage msg;
            msg.setTargetId(pcprop->GetPropertyLong(pcprop->GetPropertyIndex("Entity ID")));
            printf("OnMouseDown: Requisting picking up entity: %d \n", msg.getTargetId());
            network->send(&msg);
          }
          // If it's a door, request to open.
          else if (pcprop->GetPropertyLong(pcprop->GetPropertyIndex("Entity Type")) == Entity::DoorEntity)
          {
            if (pcprop->GetPropertyBool(pcprop->GetPropertyIndex("Door Open")))
            {
              CloseDoorRequestMessage msg;
              msg.setDoorId(pcprop->GetPropertyLong(pcprop->GetPropertyIndex("Entity ID")));
              printf("OnMouseDown: Requesting closing door: %d \n", msg.getDoorId());
              network->send(&msg);
            }
            else
            {
              OpenDoorRequestMessage msg;
              msg.setDoorId(pcprop->GetPropertyLong(pcprop->GetPropertyIndex("Entity ID")));
              printf("OnMouseDown: Requesting opening door: %d \n", msg.getDoorId());
              network->send(&msg);
            }
          }
          // If it's a player, attack it.
          else if (pcprop->GetPropertyLong(pcprop->GetPropertyIndex("Entity Type")) == Entity::PlayerEntity)
          {
            combatmanager->RequestSkillUsageStart (ent, guimanager->GetHUDWindow()->GetActiveSkillId());
          }
          // If it's a npc, open a dialog.
          else if (pcprop->GetPropertyLong(pcprop->GetPropertyIndex("Entity Type")) == Entity::NPCEntity)
          {
            NpcStartDialogMessage msg;
            msg.setNpcId(pcprop->GetPropertyLong(pcprop->GetPropertyIndex("Entity ID")));
            printf("OnMouseDown: Requesting dialog with: %d \n", msg.getNpcId());
            network->send(&msg);
          }
          else
          {
            printf("OnMouseDown: Unknown entity type!\n");
          }
          return true;
          break;
        }

      case csmbMiddle:
        {
          break;
        }
      case csmbWheelUp:
        {
          iCelEntity* entity = entitymanager->getOwnEntity();
          if (!entity) return false;
          csRef<iPcDefaultCamera> pccamera = CEL_QUERY_PROPCLASS_ENT(entity, iPcDefaultCamera);
          cameradistance -= 0.5;
          pccamera->SetDistance(cameradistance);
          return false;
          break;
        }
      case csmbWheelDown:
        {
          iCelEntity* entity = entitymanager->getOwnEntity();
          if (!entity) return false;
          csRef<iPcDefaultCamera> pccamera = CEL_QUERY_PROPCLASS_ENT(entity, iPcDefaultCamera);
          cameradistance += 0.5;
          pccamera->SetDistance(cameradistance);
          return false;
          break;
        }
      }
    }
  }

  return false;
}

bool Client::OnMouseMove(iEvent& e)
{
  csRef<iCamera> cam = entitymanager->getOwnCamera();
  if (!cam) return false;
  cursor->MouseMove(pl, cam, csMouseEventHelper::GetX(&e), csMouseEventHelper::GetY(&e));
  return false;
}

bool Client::InitializeCEL()
{
  iObjectRegistry* object_reg = this->GetObjectRegistry();

  pl = CS_QUERY_REGISTRY (object_reg, iCelPlLayer);
  if (!pl) return ReportError("Failed to load CEL Physical Layer");

  // Loading property class factories
  if (!pl->LoadPropertyClassFactory ("cel.pcfactory.region"))
    return ReportError("Failed to load CEL Region Factory");

  if (!pl->LoadPropertyClassFactory ("cel.pcfactory.mesh"))
    return ReportError("Failed to load CEL Mesh Factory");

  if (!pl->LoadPropertyClassFactory ("cel.pcfactory.solid"))
    return ReportError("Failed to load CEL Solid Factory");

  if (!pl->LoadPropertyClassFactory ("cel.pcfactory.defaultcamera"))
    return ReportError("Failed to load CEL Camera Factory");

  if (!pl->LoadPropertyClassFactory ("cel.pcfactory.linmove"))
    return ReportError("Failed to load CEL Linmove Factory");

  if (!pl->LoadPropertyClassFactory ("cel.pcfactory.quest"))
    return ReportError("Failed to load CEL Quest Factory");

  if (!pl->LoadPropertyClassFactory ("cel.pcfactory.colldet"))
    return ReportError("Failed to load CEL Colldet Factory");

  if (!pl->LoadPropertyClassFactory ("cel.pcfactory.actormove"))
    return ReportError("Failed to load CEL Actormove Factory");

  if (!pl->LoadPropertyClassFactory ("cel.pcfactory.npcmove"))
    return ReportError("Failed to load CEL Npcmove Factory");

  if (!pl->LoadPropertyClassFactory ("cel.pcfactory.trigger"))
    return ReportError("Failed to load CEL Trigger Factory");

  if (!pl->LoadPropertyClassFactory ("cel.pcfactory.tooltip"))
    return ReportError("Failed to load CEL Tooltip Factory");

  if (!pl->LoadPropertyClassFactory ("cel.pcfactory.pccommandinput"))
    return false;

  if (!pl->LoadPropertyClassFactory ("cel.pcfactory.properties"))
    return false;

  if (!pl->LoadPropertyClassFactory ("cel.pcfactory.zonemanager"))
    return false;

  #if 1
  // HACK HACK
  // Fixes the CEGUI partial drawing.
  // If you cross a portal it works for some reason.
  // (Camera in one sector, the character in the other.)
  csRef<iCelEntity> entity = pl->CreateEntity();
  pl->CreatePropertyClass(entity, "pctooltip");
  csRef<iPcTooltip> nametag = CEL_QUERY_PROPCLASS_ENT(entity, iPcTooltip);
  nametag->SetText(" ");
  nametag->SetJustify(CEL_TOOLTIP_CENTER);
  nametag->SetBackgroundColor(-1, -1, -1);
  nametag->Show(0, 0);
  #endif

  return true;
}

void Client::loadRegion(const char* name)
{
  load_region.AttachNew(new scfString(name));
}

void Client::loggedIn()
{
  guimanager->CreateSelectCharWindow ();
  guimanager->GetLoginWindow ()->HideWindow ();
  guimanager->GetSelectCharWindow ()->ShowWindow ();
  guimanager->CreateInventoryWindow ();
  guimanager->CreateStatusWindow ();
  guimanager->CreateBuddyWindow();

  state = STATE_LOGGED_IN;

  if (cmdline)
  {
    const char* character = cmdline->GetOption("char");
    if (character)
    {
      CharSelectRequestMessage answer_msg;
      answer_msg.setCharId(atoi(character));
      network->send(&answer_msg);
    }
  }
}

void Client::OnCommandLineHelp()
{
  const size_t NUM_OPTIONS = 5;

  const csOptionDescription pt_config_options [NUM_OPTIONS] =
  {
    { 0, "host", "Hostname the client will connect to (peragro.org)", CSVAR_STRING },
    { 1, "useCD", "Enable/Disable Collision Detection on the client", CSVAR_BOOL },
    { 2, "user", "Automatic Login with the given Username", CSVAR_STRING },
    { 3, "pass", "Automatic Login with the given Password", CSVAR_STRING },
    { 4, "char", "Automatic selection of the character with the given ID", CSVAR_LONG },
  };

  for (int i = 0; i<NUM_OPTIONS ; i++)
  {
    csString opt;
    csStringFast<80> desc;
    switch (pt_config_options[i].type)
    {
      case CSVAR_BOOL:
      {
        opt.Format ("  -[no]%s", pt_config_options[i].name);
        desc.Format ("%s ", pt_config_options[i].description);
        break;
      }
      case CSVAR_CMD:
      {
        opt.Format ("  -%s", pt_config_options[i].name);
        desc = pt_config_options[i].description;
        break;
      }
      case CSVAR_FLOAT:
      {
        opt.Format ("  -%s=<val>", pt_config_options[i].name);
        desc.Format ("%s", pt_config_options[i].description);
        break;
      }
      case CSVAR_LONG:
      {
        opt.Format ("  -%s=<val>", pt_config_options[i].name);
        desc.Format ("%s", pt_config_options[i].description);
        break;
      }
      case CSVAR_STRING:
      {
        opt.Format ("  -%s=<val>", pt_config_options[i].name);
        desc.Format ("%s", pt_config_options[i].description);
        break;
      }
    }
    csPrintf ("%-21s%s\n", opt.GetData(), desc.GetData());
  }
}

void Client::loadRegion()
{
  iCelEntity* ent = pl->FindEntity("ptIntroWorld");
  if (ent)
  {
    csRef<iPcRegion> pcreg = CEL_QUERY_PROPCLASS_ENT(ent, iPcRegion);
    pcreg->Unload();
    pl->RemoveEntity(ent);
  }

  playing = true;
  entitymanager->setPlaying(playing);

  if (!load_region.IsValid()) return;

  guimanager->GetSelectCharWindow ()->HideWindow();
  guimanager->GetOptionsWindow ()->HideWindow();
  guimanager->CreateChatWindow ();
  guimanager->CreateHUDWindow ();
  
  const char* regionname = cmdline->GetOption("world");

  if (!regionname) 
  {
    regionname = load_region->GetData();
    printf("loadRegion: Using default world.\n");
  }

  csRef<iCelEntity> entity = pl->CreateEntity();
  pl->CreatePropertyClass(entity, "pczonemanager");

  csRef<iPcZoneManager> pczonemgr = CEL_QUERY_PROPCLASS_ENT (entity,
  	iPcZoneManager);

  entity->SetName("ptworld");
  pczonemgr->SetLoadingMode(CEL_ZONE_LOADALL);
  world_loaded = pczonemgr->Load("/peragro/art/world/", "regions.xml");
  iCelRegion* region = pczonemgr->FindRegion(regionname);
  if (region) pczonemgr->ActivateRegion(region);

  entitymanager->setWorldloaded(world_loaded);
  load_region = 0;

  // Stop the intro music.
  sndstream->Pause ();

  // Remove the portal.
  bool ref = app_cfg->GetBool("Client.waterreflections");
  if (!ref)
  {
    iMeshWrapper* portalMesh = engine->FindMeshObject("portal");
    if (portalMesh) engine->RemoveObject(portalMesh->QueryObject());
    printf("loadRegion: Waterportal removed!\n");
  }

}

void Client::chat(unsigned char type, const char* msg, const char* other)
{
  mutex.lock();

  ChatMessage chatmsg;
  chatmsg.type = type;
  chatmsg.msg = msg;
  chatmsg.nick = other;

  chat_msgs.Push(chatmsg);

  mutex.unlock();
}

void Client::chat()
{
  if (!chat_msgs.GetSize()) return;
  mutex.lock();

  ChatMessage chatmsg = chat_msgs.Pop();

  if(chatmsg.type == 0)
  {
    //guimanager->GetChatWindow ()->AddChatMessage (chatmsg.nick, chatmsg.msg);
    guimanager->GetChatWindow ()->AddChatMessage (chatmsg.nick.GetData(), chatmsg.msg.GetData());
  }
  else if(chatmsg.type == 1)
  {
    guimanager->GetWhisperWindow()->AddWhisper(chatmsg.nick.GetData(), chatmsg.msg.GetData());
  }
  else
    printf("Client: ERROR: Unknown chattype %s", chatmsg.type);

  mutex.unlock();
}

void Client::sawServer()
{
  last_seen = csGetTicks();
}

iPcActorMove* Client::getPcActorMove()
{
  iCelEntity* entity = entitymanager->getOwnEntity();
  if (entity == 0) 
    return 0;
  csRef<iPcActorMove> pcactormove = CEL_QUERY_PROPCLASS_ENT(entity, iPcActorMove);
  return pcactormove;
}


/*---------------*
* Main function
*---------------*/
int main (int argc, char* argv[])
{
#ifdef __TIMESTAMP__
  printf("Peragro Tempus - Client\n$Revision$\tBuild-Date: %s\n", __TIMESTAMP__);
#else
  printf("Peragro Tempus - Client\n$Revision$\tnBuild-Data: %s %s\n", __DATE__, __TIME__);
#endif
  setWinCrashDump(argv[0]);

#ifdef CS_STATIC_LINKED
  //Overwrite existing env-vars for static linked client
  setenv("CRYSTAL", ".", true);
  setenv("CEL", ".", true);
#endif

  return csApplicationRunner<Client>::Run (argc, argv);
}
