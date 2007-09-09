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

#include <csgeom/path.h>
#include <cstool/initapp.h>
#include <cstool/csview.h>
#include <csutil/cmdhelp.h>
#include <csutil/cmdline.h>
#include <csutil/csstring.h>
#include <csutil/csshlib.h>
#include <csutil/event.h>
#include <csutil/sysfunc.h>
#include <csutil/syspath.h>
#include <iengine/camera.h>
#include <iengine/campos.h>
#include <iengine/mesh.h>
#include <iengine/sector.h>
#include <iengine/texture.h>
#include <iengine/material.h>
#include <iengine/scenenode.h>
#include <iengine/movable.h>
#include <imesh/object.h>
#include <imesh/spritecal3d.h>
#include <imesh/sprite2d.h>
#include <imesh/genmesh.h>
#include <iutil/databuff.h>
#include <iutil/event.h>
#include <iutil/eventq.h>
#include <iutil/object.h>
#include <iutil/vfs.h>
#include <ivaria/collider.h>
#include <ivideo/graph2d.h>
#include <ivideo/natwin.h>
#include <ivideo/txtmgr.h>
#include <ivideo/material.h>

#include "client/reporter/reporter.h"

#include "cursor.h"
#include "client/network/network.h"
#include "client/gui/gui.h"
#include "client/gui/guimanager.h"
#include "client/data/effect/effectsmanager.h"
#include "client/combat/combatmanager.h"
#include "client/data/item/itemmanager.h"
#include "client/data/sector/sectormanager.h"
#include "client/entity/ptentitymanager.h"
#include "client/console/console.h"
#include "client/chat/chatmanager.h"

#include "client/trade/trademanager.h"

#include "common/util/wincrashdump.h"

CS_IMPLEMENT_APPLICATION

namespace PT
{

  Client::Client() : playing(false)
  {
    SetApplicationName ("Client");
    state = STATE_INITIAL;
    walk = 0;
    run = false;
    turn = 0;
    timer = 0;
    limitFPS = 0;
    last_sleep = 0;
    world_loaded = false;
    cameradistance = 3;
    last_seen = 0;

    reporter = 0;
    network = 0;
    guimanager = 0;
    effectsmanager = 0;
    entitymanager = 0;
    combatmanager = 0;
    itemmanager = 0;
    cursor = 0;
    inputMgr = 0;
    last_seen = 0;
    ptconsole = 0;

    eventmanager = 0;
    chatmanager = 0;
    trademanager = 0;
    sectormanager = 0;
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
    delete inputMgr;
    delete reporter;
    delete trademanager;
    delete sectormanager;
  }

  void Client::PreProcessFrame()
  {
    csTicks ticks = vc->GetElapsedTicks();
    timer += ticks;

    effectsmanager->HandleEffects(ticks);

    eventmanager->Handle();

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
    g3d->BeginDraw (engine->GetBeginDrawFlags () | CSDRAW_3DGRAPHICS);

    handleStates();

    // Draw the player camera manually.
    csRef<iPcDefaultCamera> cam = entitymanager->getOwnCamera();
    if (cam.IsValid()) cam->Draw();

    if (state == STATE_PLAY) 
    {
      if (entitymanager)
      {
        PtCharacterEntity *character = (PtCharacterEntity *) entitymanager->getOwnPtEntity();
        if (character) 
        {
          int currentStamina = character->getCurrentStamina();
          int maxStamina = character->getMaxStamina();
          float ratio = (float)currentStamina / (float)maxStamina;
          if (guimanager)
          {
            guimanager->GetHUDWindow()->SetHP(ratio);
            char buffer[40];
            sprintf(buffer, "            %d/%d\0", currentStamina, maxStamina);
            guimanager->GetHUDWindow()->SetText("PlayerHUD/Name", buffer);
          }
        }
      }
    }

    // Paint the interface over the engine
    if (guimanager) guimanager->Render ();
    if (cursor) cursor->Draw();
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

    cmdline = csQueryRegistry<iCommandLineParser> (GetObjectRegistry());
    if (!cmdline) return ReportError("Failed to locate CommandLineParser plugin");

#ifdef CS_STATIC_LINKED
    cmdline->AddOption("silent", "true");
#endif

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
    if (network) network->stop();
  }

  bool Client::Application()
  {
    if (!OpenApplication(GetObjectRegistry()))
      return ReportError("Error opening system!");

    // Create and Initialize the Reporter. 
    reporter= new Reporter (GetObjectRegistry());
    if (!reporter) return ReportError("Error loading Reporter!");
    reporter->Initialize();
    //pointerlib.setReporter(reporter);

#ifdef CS_STATIC_LINKED
    reporter->SetLoggingLevel(PT::Errors);
#else
    reporter->SetLoggingLevel(PT::Insane);
#endif


    vfs = csQueryRegistry<iVFS> (GetObjectRegistry());
    if (!vfs) return Report(PT::Error, "Failed to locate VFS!");

    g3d = csQueryRegistry<iGraphics3D> (GetObjectRegistry());
    if (!g3d) return Report(PT::Error, "Failed to locate 3D renderer!");

    pointerlib.setObjectRegistry(GetObjectRegistry());
    pointerlib.setClient(this);

    // Create and Initialize the Network. 
    network = new Network (this);
    if (!network) 
      return Report(PT::Error, "Failed to create Network object!");
    network->init();
    pointerlib.setNetwork(network);

    // Create and Initialize the InputManager.
    inputMgr = new InputManager();
    if(!inputMgr->Initialize()) 
      return Report(PT::Error, "Failed to create InputManager object!");

    // Create and Initialize the EventManager.
    eventmanager = new PT::Events::EventManager();
    if (!eventmanager->Initialize())
      return Report(PT::Error, "Failed to initialize EventManager!");
    pointerlib.setEventManager(eventmanager);

    // Create and Initialize the ItemManager.
    itemmanager = new PT::Data::ItemManager (GetObjectRegistry());
    if (!itemmanager->Initialize())
      return Report(PT::Error, "Failed to initialize ItemManager!");
    pointerlib.setItemManager(itemmanager);

    // Create and Initialize the ItemManager.
    sectormanager = new PT::Data::SectorManager (GetObjectRegistry());
    if (!sectormanager->Initialize())
      return Report(PT::Error, "Failed to initialize SectorManager!");
    pointerlib.setSectorManager(sectormanager);

    // Create and Initialize the GUImanager.
    guimanager = new GUIManager ();
    if (!guimanager->Initialize ())
      return Report(PT::Error, "Failed to initialize GUIManager!");
    pointerlib.setGUIManager(guimanager);

    if (!RegisterQueue(GetObjectRegistry(), csevAllEvents(GetObjectRegistry())))
      return Report(PT::Error, "Failed to set up event handler!");

    engine = csQueryRegistry<iEngine> (GetObjectRegistry());
    if (!engine) return Report(PT::Error, "Failed to locate 3D engine!");

    vc = csQueryRegistry<iVirtualClock> (GetObjectRegistry());
    if (!vc) return Report(PT::Error, "Failed to locate Virtual Clock!");

    //kbd = csQueryRegistry<iKeyboardDriver> (GetObjectRegistry());
    //if (!kbd) return ReportError("Failed to locate Keyboard Driver!");

    sndrenderer = csQueryRegistry<iSndSysRenderer> (GetObjectRegistry());
    if (!sndrenderer) return Report(PT::Error, "Failed to locate sound renderer!");

    sndloader = csQueryRegistry<iSndSysLoader> (GetObjectRegistry());
    if (!sndloader) return Report(PT::Error, "Failed to locate sound loader!");

    app_cfg = csQueryRegistry<iConfigManager> (GetObjectRegistry());
    if (!app_cfg) return Report(PT::Error, "Can't find the config manager!");

    iNativeWindow* nw = g3d->GetDriver2D()->GetNativeWindow ();
    if (nw) nw->SetTitle ("Peragro Tempus");

    // Disable the lighting cache.
    engine->SetLightingCacheMode (0);

    // Let the engine prepare all lightmaps for use and also free all images 
    // that were loaded for the texture manager.
    engine->Prepare ();


    InitializeCEL();

    // Create the cursor.
    cursor = new Cursor(this);
    if (!cursor) return Report(PT::Error, "Can't create cursor!");

    // Create and Initialize the EntityManager.
    entitymanager = new PT::Entity::EntityManager (GetObjectRegistry());
    if (!entitymanager->Initialize())
      return Report(PT::Error, "Failed to initialize EntityManager!");
    pointerlib.setEntityManager(entitymanager);

    // Create and Initialize the Effectsmanager.
    effectsmanager = new PT::Data::EffectsManager (GetObjectRegistry());
    if (!effectsmanager->Initialize())
      return Report(PT::Error, "Failed to initialize EffectsManager!");
    pointerlib.setEffectsManager(effectsmanager);

    // Create and Initialize the Combatmanager.
    combatmanager = new CombatMGR ();
    if (!combatmanager->Initialize())
      return Report(PT::Error, "Failed to initialize CombatManager!");
    pointerlib.setCombatManager(combatmanager);

    // Create and Initialize the ChatManager.
    chatmanager = new PT::Chat::ChatManager (GetObjectRegistry());
    if (!chatmanager->Initialize())
      return Report(PT::Error, "Failed to initialize ChatManager!");
    pointerlib.setChatManager(chatmanager);

    // Create and Initialize the ChatManager.
    trademanager = new PT::Trade::TradeManager (GetObjectRegistry());
    if (!trademanager->Initialize())
      return Report(PT::Error, "Failed to initialize TradeManager!");
    //pointerlib.setTradeManager(trademanager);

    /*
    // Create and Initialize the PTConsole.
    ptconsole = new PtConsole ();
    if (!ptconsole->Initialize())
    return ReportError ("Failed to initialize PT Console!");
    */
    view.AttachNew(new csView(engine, g3d));

    // intro sound
    const char* fname = "/peragro/art/audio/music/intro/peragrotempus.ogg";

    csRef<iDataBuffer> soundbuf = vfs->ReadFile (fname);
    if (!soundbuf)
      return Report(PT::Error, "Can't load file '%s'!", fname);

    csRef<iSndSysData> snddata = sndloader->LoadSound (soundbuf);
    if (!snddata)
      return Report(PT::Error,"Can't load sound '%s'!", fname);

    sndstream = sndrenderer->CreateStream (snddata,
      CS_SND3D_ABSOLUTE);
    if (!sndstream)
      return Report(PT::Error,"Can't create stream for '%s'!", fname);

    sndsource = sndrenderer->CreateSource (sndstream);
    if (!sndsource)
      return Report(PT::Error,"Can't create source for '%s'!", fname);
    sndsource3d = scfQueryInterface<iSndSysSourceSoftware3D> (sndsource);

    sndsource3d->SetPosition (csVector3(0,0,0));
    sndsource3d->SetVolume (1.0f);

    sndstream->SetLoopState (CS_SNDSYS_STREAM_LOOP);
    sndstream->Unpause ();

    // end intro sound
    csRef<iLoader >loader = csQueryRegistry<iLoader> (GetObjectRegistry());
    if (!loader) return Report(PT::Error, "Failed to locate Loader!");
    loader->LoadLibraryFile("/peragro/xml/quests/doorquests.xml");

    //Listen for events.
    using namespace PT::Events;

    // Register listener for StateLoggedInEvent.
    EventHandler<Client>* cbConnected = new EventHandler<Client>(&Client::Connected, this);
    PointerLibrary::getInstance()->getEventManager()->AddListener("StateConnectedEvent", cbConnected);

    // Register listener for StateLoggedInEvent.
    EventHandler<Client>* cbLoggedIn = new EventHandler<Client>(&Client::loggedIn, this);
    PointerLibrary::getInstance()->getEventManager()->AddListener("StateLoggedInEvent", cbLoggedIn);

    // Register listener for RegionLoadEvent.
    EventHandler<Client>* cbLoad = new EventHandler<Client>(&Client::LoadRegion, this);
    PointerLibrary::getInstance()->getEventManager()->AddListener("RegionLoadEvent", cbLoad);

    //Actions

    // Register listener for ActionForward.
    EventHandler<Client>* cbActionForward = new EventHandler<Client>(&Client::ActionForward, this);
    PointerLibrary::getInstance()->getEventManager()->AddListener("ActionEvent", cbActionForward);

    // Register listener for ActionBackward.
    EventHandler<Client>* cbActionBackward = new EventHandler<Client>(&Client::ActionBackward, this);
    PointerLibrary::getInstance()->getEventManager()->AddListener("ActionEvent", cbActionBackward);

    // Register listener for ActionLeft.
    EventHandler<Client>* cbActionLeft = new EventHandler<Client>(&Client::ActionLeft, this);
    PointerLibrary::getInstance()->getEventManager()->AddListener("ActionEvent", cbActionLeft);

    // Register listener for ActionRight.
    EventHandler<Client>* cbActionRight = new EventHandler<Client>(&Client::ActionRight, this);
    PointerLibrary::getInstance()->getEventManager()->AddListener("ActionEvent", cbActionRight);

    // Register listener for ActionToggleWalk.
    EventHandler<Client>* cbActionToggleWalk = new EventHandler<Client>(&Client::ActionToggleWalk, this);
    PointerLibrary::getInstance()->getEventManager()->AddListener("ActionEvent", cbActionToggleWalk);

    // Register listener for ActionToggleRun.
    EventHandler<Client>* cbActionToggleRun = new EventHandler<Client>(&Client::ActionToggleRun, this);
    PointerLibrary::getInstance()->getEventManager()->AddListener("ActionEvent", cbActionToggleRun);

    // Register listener for ActionPanUp.
    EventHandler<Client>* cbActionPanUp = new EventHandler<Client>(&Client::ActionPanUp, this);
    PointerLibrary::getInstance()->getEventManager()->AddListener("ActionEvent", cbActionPanUp);

    // Register listener for ActionPanDown.
    EventHandler<Client>* cbActionPanDown = new EventHandler<Client>(&Client::ActionPanDown, this);
    PointerLibrary::getInstance()->getEventManager()->AddListener("ActionEvent", cbActionPanDown);

    // Register listener for ActionToggleCamera.
    EventHandler<Client>* cbActionToggleCamera = new EventHandler<Client>(&Client::ActionToggleCamera, this);
    PointerLibrary::getInstance()->getEventManager()->AddListener("ActionEvent", cbActionToggleCamera);

    // Register listener for ActionToggleDistClipping.
    EventHandler<Client>* cbActionToggleDistClipping = new EventHandler<Client>(&Client::ActionToggleDistClipping, this);
    PointerLibrary::getInstance()->getEventManager()->AddListener("ActionEvent", cbActionToggleDistClipping);

    // Register listener for ActionHit.
    EventHandler<Client>* cbActionHit = new EventHandler<Client>(&Client::ActionHit, this);
    PointerLibrary::getInstance()->getEventManager()->AddListener("ActionEvent", cbActionHit);

    // Register listener for ActionActivateSkill.
    EventHandler<Client>* cbActionActivateSkill = new EventHandler<Client>(&Client::ActionActivateSkill, this);
    PointerLibrary::getInstance()->getEventManager()->AddListener("ActionEvent", cbActionActivateSkill);

    // Register listener for ActionActivateWeapon.
    EventHandler<Client>* cbActionActivateWeapon = new EventHandler<Client>(&Client::ActionActivateWeapon, this);
    PointerLibrary::getInstance()->getEventManager()->AddListener("ActionEvent", cbActionActivateWeapon);

    // Register listener for ActionQuit.
    EventHandler<Client>* cbActionQuit = new EventHandler<Client>(&Client::ActionQuit, this);
    PointerLibrary::getInstance()->getEventManager()->AddListener("ActionEvent", cbActionQuit);

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
          path = 0;
          iCEGUI* cegui = guimanager->GetCEGUI();

          // load the background
          vfs->ChDir ("/peragro/skin/");
          cegui->GetImagesetManagerPtr()->createImagesetFromImageFile("Background", "background1600.jpg");

          // Get root window.
          CEGUI::Window* root = cegui->GetWindowManagerPtr ()->getWindow("Root");
          CEGUI::Window* image = cegui->GetWindowManagerPtr ()->createWindow("Peragro/StaticImage", "Background");
          root->addChildWindow(image);
          image->setPosition(CEGUI::UVector2(CEGUI::UDim(0,0), CEGUI::UDim(0,0)));
          image->setSize(CEGUI::UVector2(CEGUI::UDim(1,0), CEGUI::UDim(1,0)));

          // set the background image
          image->setProperty("Image", "set:Background image:full_image");
          image->setProperty("BackgroundEnabled", "True");
          image->setProperty("FrameEnabled", "False");
          image->setProperty("Disabled", "True");
          image->moveToBack();
        }
        else if (!strncmp(path,"/intro", 5))
        {
          path = "/intro";
        }
        else if (!strncmp(path,"empty", 4))
        {
          path = 0;
        }

        Report(PT::Notify, "handleStates: Loading Intro: %s", path? path : "empty");

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
          Report(PT::Notify, "handleStates: Loading Intro '%s'.", hasIntro?"succeeded":"failed");
        }

        if (hasIntro)
        {
          csVector3 sPos = pcregion->GetStartPosition();
          view->GetCamera()->SetSector(pcregion->GetStartSector());
          view->GetCamera()->GetTransform().Translate(sPos);
          const char* secName = pcregion->GetStartSector()->QueryObject()->GetName();
          Report(PT::Notify, "handleStates: Setting up Camera at %s <%.2f,%.2f,%.2f>.", secName, sPos.x, sPos.y, sPos.z);
        }
        else
        {
          Report(PT::Notify, "handleStates: Clearing screen due to missing intro.");
          engine->SetClearScreen(true);
        }

        // Show the connect window.
        guimanager->GetConnectWindow ()->ShowWindow ();

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
        view->Draw();
        break;
      }
    case STATE_PLAY:
      {
        checkConnection();
        entitymanager->Handle();
        break;
      }
    case STATE_RECONNECTED:
      {
        entitymanager->Handle();
        break;
      }
    }
  }

  void Client::checkConnection()
  {
    //Report(PT::Notify, "Saw server %d ms ago.", csGetTicks() - last_seen);
    if (last_seen > 0 && csGetTicks() - last_seen > 10000)
    {
      // 10 seconds of no response... disconnect!
      if (state == STATE_PLAY)
      {
        last_seen = 0;
        Report(PT::Warning, "Disconnect!");
        guimanager->CreateOkWindow()->SetText("Disconnect!\n Please restart the client!");
        //entitymanager->delAllEntities();
        //ConnectRequestMessage msg;
        //network->send(&msg);
      }
    }
  }

  bool Client::Connected (PT::Events::Eventp ev)
  {
    using namespace PT::Events;

    StateConnectedEvent* stateEv = GetStateEvent<StateConnectedEvent*>(ev);
    if (!stateEv) return false;

    if (state == STATE_PLAY)
    {
      state = STATE_RECONNECTED;
      login(user, pass);
    }
    else
    {
      guimanager->GetConnectWindow ()->HideWindow ();
      guimanager->GetLoginWindow ()->ShowWindow ();

      state = STATE_CONNECTED;

      if (cmdline)
      {
        user = cmdline->GetOption("user", 0);
        pass = cmdline->GetOption("pass", 0);

        if (user && pass)
        {
          if (cmdline->GetBoolOption("register", false))
          {
            RegisterRequestMessage reg_msg;
            reg_msg.setUsername(ptString(user, strlen(user)));
            reg_msg.setPassword(pass);
            network->send(&reg_msg);
          }
          login(user, pass);
        }
      }
    }

    Report(PT::Notify, "Connected!");
    return true;
  }

  void Client::login(csString user, csString pass)
  {
    this->user = user;
    this->pass = pass;

    LoginRequestMessage answer_msg;
    answer_msg.setUsername(ptString(user, strlen(user)));
    answer_msg.setPassword(pass);
    network->send(&answer_msg);
  }

  bool Client::ActionForward(PT::Events::Eventp ev) 
  {
    using namespace PT::Events;

    if (playing) 
    {
      ActionEvent* actionEv = GetActionEvent<ActionEvent*>(ev);
      if (!actionEv) return false;

      if (!actionEv->Compare("ACTION_FORWARD")) return false;

      if (!actionEv->released) 
        walk = 1; 
      else 
        walk = 0;
    }

    DoAction();
    return true;
  }

  bool Client::ActionBackward(PT::Events::Eventp ev) 
  {
    using namespace PT::Events;

    if (playing) 
    {
      ActionEvent* actionEv = GetActionEvent<ActionEvent*>(ev);
      if (!actionEv) return false;

      if (!actionEv->Compare("ACTION_BACKWARD")) return false;

      if (!actionEv->released)
        walk = -1;
      else 
        walk = 0;
    }

    DoAction();
    return true;
  }

  bool Client::ActionLeft(PT::Events::Eventp ev) 
  {
    using namespace PT::Events;

    if (playing) 
    {
      ActionEvent* actionEv = GetActionEvent<ActionEvent*>(ev);
      if (!actionEv) return false;

      if (!actionEv->Compare("ACTION_LEFT")) return false;

      if (!actionEv->released)
        turn = -1;
      else 
        turn = 0;
    }

    DoAction();
    return true;
  }

  bool Client::ActionRight(PT::Events::Eventp ev) 
  {
    using namespace PT::Events;

    if (playing) 
    {
      ActionEvent* actionEv = GetActionEvent<ActionEvent*>(ev);
      if (!actionEv) return false;

      if (!actionEv->Compare("ACTION_RIGHT")) return false;

      if (!actionEv->released)
        turn = 1;
      else 
        turn = 0;
    }

    DoAction();
    return true;
  }

  bool Client::ActionToggleWalk(PT::Events::Eventp ev) 
  {
    using namespace PT::Events;

    if (playing) 
    {
      ActionEvent* actionEv = GetActionEvent<ActionEvent*>(ev);
      if (!actionEv) return false;

      if (!actionEv->Compare("ACTION_TOGGLEWALK")) return false;

      if (!actionEv->released) 
      {
        (walk == 0) ? walk = 1 : walk = 0;
      }
    }

    DoAction();
    return true;
  }
  
  bool Client::ActionToggleRun(PT::Events::Eventp ev) 
  {
    using namespace PT::Events;

    if (playing) 
    {
      ActionEvent* actionEv = GetActionEvent<ActionEvent*>(ev);
      if (!actionEv) return false;

      if (!actionEv->Compare("ACTION_TOGGLERUN")) return false;

      if (!actionEv->released) 
      {
        (run == 0) ? run = 1 : run = 0;
      }
    }

    DoAction();
    return true;
  }

  bool Client::ActionPanUp(PT::Events::Eventp ev) 
  {
    using namespace PT::Events;

    if (playing) 
    {
      ActionEvent* actionEv = GetActionEvent<ActionEvent*>(ev);
      if (!actionEv) return false;

      if (!actionEv->Compare("ACTION_PANUP")) return false;

      if (!actionEv->released) 
      {
        iCelEntity* entity = entitymanager->getOwnCelEntity();
        if (!entity) return false;
        csRef<iPcDefaultCamera> pccamera = CEL_QUERY_PROPCLASS_ENT(entity, iPcDefaultCamera);  
        pccamera->SetPitch(pccamera->GetPitch()-0.1f);
      }
    }

    return true;
  }

  bool Client::ActionPanDown(PT::Events::Eventp ev) 
  {
    using namespace PT::Events;

    if (playing) 
    {
      ActionEvent* actionEv = GetActionEvent<ActionEvent*>(ev);
      if (!actionEv) return false;

      if (!actionEv->Compare("ACTION_PANDOWN")) return false;

      if (!actionEv->released) 
      {
        iCelEntity* entity = entitymanager->getOwnCelEntity();
        if (!entity) return false;
        csRef<iPcDefaultCamera> pccamera = CEL_QUERY_PROPCLASS_ENT(entity, iPcDefaultCamera);  
        pccamera->SetPitch(pccamera->GetPitch()+0.1f);
      }
    }

    return true;
  }

  bool Client::ActionToggleCamera(PT::Events::Eventp ev) 
  {
    using namespace PT::Events;

    if (playing) 
    {
      ActionEvent* actionEv = GetActionEvent<ActionEvent*>(ev);
      if (!actionEv) return false;

      if (!actionEv->Compare("ACTION_TOGGLECAMERA")) return false;

      if (!actionEv->released) 
      {
        iPcActorMove* pcactormove = getPcActorMove();
        if (!pcactormove) return false;
        pcactormove->ToggleCameraMode();
      }
    }

    return true;
  }

  bool Client::ActionToggleDistClipping(PT::Events::Eventp ev) 
  {
    using namespace PT::Events;

    if (playing) 
    {
      ActionEvent* actionEv = GetActionEvent<ActionEvent*>(ev);
      if (!actionEv) return false;

      if (!actionEv->Compare("ACTION_TOGGLEDISTCLIP")) return false;

      if (!actionEv->released) 
      {
        iCelEntity* entity = entitymanager->getOwnCelEntity();
        if (!entity) return false;
        csRef<iPcDefaultCamera> pccamera = CEL_QUERY_PROPCLASS_ENT(entity, iPcDefaultCamera);
        guimanager->GetChatWindow()->AddMessage("Toggled Distance Clipping.");
        pccamera->UseDistanceClipping() ? 
          pccamera->DisableDistanceClipping() 
          : pccamera->EnableAdaptiveDistanceClipping(95, 100, 50);
      }
    }

    return true;
  }

  bool Client::ActionHit(PT::Events::Eventp ev) 
  {
    using namespace PT::Events;

    if (playing) 
    {
      ActionEvent* actionEv = GetActionEvent<ActionEvent*>(ev);
      if (!actionEv) return false;

      if (!actionEv->Compare("ACTION_HIT")) return false;

      if (!actionEv->released) 
      {
        combatmanager->hit (entitymanager->GetOwnId(), 20);
      }
    }

    return true;
  }

  bool Client::ActionActivateSkill(PT::Events::Eventp ev) 
  {
    using namespace PT::Events;

    if (playing) 
    {
      ActionEvent* actionEv = GetActionEvent<ActionEvent*>(ev);
      if (!actionEv) return false;

      if (!actionEv->Compare("ACTION_ACTIVATESKILL")) return false;

      if (!actionEv->released) 
      {
        // Activate the skill
        csRef<iCelEntity> ent = cursor->getSelectedEntity();
        csRef<iPcProperties> pcprop;
        if (ent) pcprop = CEL_QUERY_PROPCLASS_ENT(ent, iPcProperties);
        if (!pcprop) return false;
        if (pcprop->GetPropertyLong(pcprop->GetPropertyIndex("Entity Type")) == PtEntity::PlayerEntity)
        {
          combatmanager->levelup(pcprop->GetPropertyLong(pcprop->GetPropertyIndex("Entity ID")));
        }
        // Animate the player.
        iCelEntity* entity = entitymanager->getOwnCelEntity();
        if (!entity) return false;
        csRef<iPcMesh> pcmesh = CEL_QUERY_PROPCLASS_ENT(entity, iPcMesh);
        if (!pcmesh) return false;
        csRef<iMeshWrapper> parent = pcmesh->GetMesh();
        if (!parent) return false;
        csRef<iSpriteCal3DState> cal3dstate = 
          scfQueryInterface<iSpriteCal3DState> (parent->GetMeshObject());
        if (!cal3dstate) return false;
        cal3dstate->SetAnimAction("cast_summon", 0.0f, 0.0f);
      }
    }

    return true;
  }

  bool Client::ActionActivateWeapon(PT::Events::Eventp ev) 
  {
    using namespace PT::Events;

    if (playing) 
    {
      ActionEvent* actionEv = GetActionEvent<ActionEvent*>(ev);
      if (!actionEv) return false;

      if (!actionEv->Compare("ACTION_ACTIVATEWEAPON")) return false;

      if (!actionEv->released) 
      {
        iCelEntity* entity = entitymanager->getOwnCelEntity();
        if (!entity) return false;
        csRef<iPcMesh> pcmesh = CEL_QUERY_PROPCLASS_ENT(entity, iPcMesh);
        if (!pcmesh) return false;
        csRef<iMeshWrapper> parent = pcmesh->GetMesh();
        if (!parent) return false;
        csRef<iSpriteCal3DState> cal3dstate = 
          scfQueryInterface<iSpriteCal3DState> (parent->GetMeshObject());
        if (!cal3dstate) return false;
        cal3dstate->SetAnimAction("attack_sword_s", 0.0f, 0.0f);
      }
    }

    return true;
  }

  bool Client::ActionQuit(PT::Events::Eventp ev) 
  {
    using namespace PT::Events;

    ActionEvent* actionEv = GetActionEvent<ActionEvent*>(ev);
    if (!actionEv) return false;

    if (!actionEv->Compare("ACTION_QUIT")) return false;

    csRef<iEventQueue> q = csQueryRegistry<iEventQueue> (GetObjectRegistry());
    if (q.IsValid()) q->GetEventOutlet()->Broadcast(csevQuit(GetObjectRegistry()));

    return true;
  }

  bool Client::DoAction() 
  {
    MoveRequestMessage msg;
    msg.setWalk(walk+1);
    if (walk == -1) {
      msg.setTurn(-turn+1);
    } else {
      msg.setTurn(turn+1);
    }
    msg.setRun(run);
    network->send(&msg);

    return true;
  }
  bool Client::OnKeyboard(iEvent& ev)
  {
    return inputMgr->ProcessEvent(ev);
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
            if (!entitymanager) return false;
            csRef<iPcDefaultCamera> pccamera = entitymanager->getOwnCamera();
            if (!pccamera) return false;
            csRef<iCamera> cam = pccamera->GetCamera();
            if (!cam) return false;

            csVector3 isect, untransfCoord;
            csRef<iMeshWrapper> mesh = cursor->Get3DPointFrom2D(csMouseEventHelper::GetX(&ev), 
              csMouseEventHelper::GetY(&ev), 
              cam, &isect, &untransfCoord);

            if (mesh)
            {
              effectsmanager->CreateEffect(PT::Data::EffectsManager::MoveMarker, isect+csVector3(0,0.01f,0));
              //effectsmanager->CreateDecal(isect+csVector3(0,0.25,0), cam);

              csRef<iCelEntity> ownent = entitymanager->getOwnCelEntity();
              if (!ownent) return false;
              csRef<iPcLinearMovement> pclinmove = CEL_QUERY_PROPCLASS_ENT(ownent, iPcLinearMovement);
              if (!pclinmove) return false;

              MoveToRequestMessage msg;
              msg.setTo(isect.x, isect.y, isect.z);
              network->send(&msg);

              Report(PT::Debug, "OnMouseDown: position: %s", isect.Description().GetData());
            }
            else
            {
              Report(PT::Warning, "OnMouseDown: Failed to find mesh!");
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
            if (pcprop->GetPropertyLong(pcprop->GetPropertyIndex("Entity Type")) == PtEntity::ItemEntity)
            {
              unsigned int slotid = guimanager->GetInventoryWindow()->FindFreeSlot();
              if(slotid < 30)
              {
                PickRequestMessage msg;
                msg.setItemEntityId(pcprop->GetPropertyLong(pcprop->GetPropertyIndex("Entity ID")));
                msg.setSlot(slotid); // TODO: get a free slot for this!
                Report(PT::Notify, "OnMouseDown: Requisting picking up entity: %d for slot %d.", msg.getItemEntityId(), slotid);
                network->send(&msg);
              }
            }
            // If it's a door, request to open.
            else if (pcprop->GetPropertyLong(pcprop->GetPropertyIndex("Entity Type")) == PtEntity::DoorEntity)
            {
              if (pcprop->GetPropertyBool(pcprop->GetPropertyIndex("Door Open")))
              {
                CloseDoorRequestMessage msg1;
                msg1.setDoorId(pcprop->GetPropertyLong(pcprop->GetPropertyIndex("Entity ID")));
                Report(PT::Notify, "OnMouseDown: Requesting closing door: %d.", msg1.getDoorId());
                network->send(&msg1);

                //Lock it again after closing.
                LockDoorRequestMessage msg2;
                msg2.setDoorId(pcprop->GetPropertyLong(pcprop->GetPropertyIndex("Entity ID")));
                Report(PT::Notify, "OnMouseDown: Requesting unlocking door: %d.", msg2.getDoorId());
                network->send(&msg2);
              }
              else
              {
                // first click unlocks, second click opens!
                if (pcprop->GetPropertyBool(pcprop->GetPropertyIndex("Door Locked")))
                {
                  UnlockDoorRequestMessage msg;
                  msg.setDoorId(pcprop->GetPropertyLong(pcprop->GetPropertyIndex("Entity ID")));
                  Report(PT::Notify, "OnMouseDown: Requesting unlocking door: %d.", msg.getDoorId());
                  network->send(&msg);
                }
                else
                {
                  OpenDoorRequestMessage msg;
                  msg.setDoorId(pcprop->GetPropertyLong(pcprop->GetPropertyIndex("Entity ID")));
                  Report(PT::Notify, "OnMouseDown: Requesting opening door: %d.", msg.getDoorId());
                  network->send(&msg);
                }
              }
            }
            // If it's a player, attack it.
            else if (pcprop->GetPropertyLong(pcprop->GetPropertyIndex("Entity Type")) == PtEntity::PlayerEntity)
            {
              //combatmanager->RequestSkillUsageStart (ent, guimanager->GetHUDWindow()->GetActiveSkillId());
              TradeRequestMessage msg;
              msg.setEntityId(pcprop->GetPropertyLong(pcprop->GetPropertyIndex("Entity ID")));
              Report(PT::Notify, "OnMouseDown: Requesting trade with: %d.", msg.getEntityId());
              network->send(&msg);
            }
            // If it's a npc, open a dialog.
            else if (pcprop->GetPropertyLong(pcprop->GetPropertyIndex("Entity Type")) == PtEntity::NPCEntity)
            {
              NpcStartDialogMessage msg;
              msg.setNpcId(pcprop->GetPropertyLong(pcprop->GetPropertyIndex("Entity ID")));
              Report(PT::Notify, "OnMouseDown: Requesting dialog with: %d.", msg.getNpcId());
              network->send(&msg);
            }
            // If it's a mount, mount it.
            else if (pcprop->GetPropertyLong(pcprop->GetPropertyIndex("Entity Type")) == PtEntity::MountEntity)
            {
              PtEntity* ent = entitymanager->findPtEntById(pcprop->GetPropertyLong(pcprop->GetPropertyIndex("Entity ID")));
              if(!ent) return false;

              PtMountEntity* mount = static_cast<PtMountEntity*>(ent);
              if(!mount->isMounted())
              {
                MountRequestMessage msg;
                msg.setMountEntityId(mount->GetId());
                network->send(&msg);
                Report(PT::Notify, "OnMouseDown: Mounting.");
              }
              else
              {
                UnmountRequestMessage msg;
                msg.setMountEntityId(mount->GetId());
                network->send(&msg);
                Report(PT::Notify, "OnMouseDown: UnMounting.");
              }
            }
            else
            {
              Report(PT::Warning, "OnMouseDown: Unknown entity type!");
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
            iCelEntity* entity = entitymanager->getOwnCelEntity();
            if (!entity) return false;
            csRef<iPcDefaultCamera> pccamera = CEL_QUERY_PROPCLASS_ENT(entity, iPcDefaultCamera);
            cameradistance -= 0.5;
            if (pccamera.IsValid()) pccamera->SetDistance(cameradistance);
            return false;
            break;
          }
        case csmbWheelDown:
          {
            iCelEntity* entity = entitymanager->getOwnCelEntity();
            if (!entity) return false;
            csRef<iPcDefaultCamera> pccamera = CEL_QUERY_PROPCLASS_ENT(entity, iPcDefaultCamera);
            cameradistance += 0.5;
            if (pccamera.IsValid()) pccamera->SetDistance(cameradistance);
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
    csRef<iPcDefaultCamera> cam = entitymanager->getOwnCamera();
    if (!cam) return false;
    if (!cam->GetCamera()->GetSector()) return false;
    cursor->MouseMove(pl, cam->GetCamera(), csMouseEventHelper::GetX(&e), csMouseEventHelper::GetY(&e));
    return false;
  }

  bool Client::InitializeCEL()
  {
    iObjectRegistry* object_reg = this->GetObjectRegistry();

    pl = csQueryRegistry<iCelPlLayer> (object_reg);
    if (!pl) return Report(PT::Error, "Failed to load CEL Physical Layer!");

    return true;
  }

  bool Client::loggedIn(PT::Events::Eventp ev)
  {
    using namespace PT::Events;

    StateLoggedInEvent* stateev = GetStateEvent<StateLoggedInEvent*>(ev);
    if (!stateev) return false;

    if (stateev->error)
    {
      Report(PT::Error, "Login Failed due to: %s.", stateev->errorMessage.c_str());
      GUIManager* guimanager = PointerLibrary::getInstance()->getGUIManager();
      guimanager->CreateOkWindow()->SetText(stateev->errorMessage.c_str());
      guimanager->GetLoginWindow()->EnableWindow();
      return true;
    }
    else
      Report(PT::Notify, "Login succeeded!");


    if (state == STATE_RECONNECTED)
    {
      selectCharacter(char_id);
    }
    else if (state == STATE_CONNECTED)
    {

      guimanager->GetLoginWindow ()->HideWindow ();
      guimanager->GetSelectCharWindow ()->ShowWindow ();

      state = STATE_LOGGED_IN;

      if (cmdline)
      {
        const char* character = cmdline->GetOption("char");
        if (character)
        {
          selectCharacter(atoi(character));
        }
      }
    }

    return true;
  }

  void Client::selectCharacter(unsigned int char_id)
  {
    this->char_id = char_id;
    CharSelectRequestMessage answer_msg;
    answer_msg.setCharId(char_id);
    network->send(&answer_msg);
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

    for (unsigned int i = 0; i<NUM_OPTIONS ; i++)
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

  bool Client::LoadRegion(PT::Events::Eventp ev)
  {
    using namespace PT::Events;

    RegionLoadEvent* regionEv = GetRegionEvent<RegionLoadEvent*>(ev);
    if (!regionEv) return false;

    iCelEntity* ent = pl->FindEntity("ptIntroWorld");
    if (ent)
    {
      csRef<iPcRegion> pcreg = CEL_QUERY_PROPCLASS_ENT(ent, iPcRegion);
      pcreg->Unload();
      pl->RemoveEntity(ent);
    }

    playing = true;
    entitymanager->setPlaying(playing);

    sawServer();
    state = STATE_PLAY;

    if (world_loaded) return true;

    guimanager->GetSelectCharWindow ()->HideWindow();
    guimanager->GetOptionsWindow ()->HideWindow();

    guimanager->GetChatWindow ()->ShowWindow();
    guimanager->GetHUDWindow ()->ShowWindow();

    // Hide the background.
    iCEGUI* cegui = guimanager->GetCEGUI();
    if (   cegui->GetWindowManagerPtr ()->isWindowPresent("Root")
      && cegui->GetWindowManagerPtr ()->isWindowPresent("Background") )
    {
      CEGUI::Window* image = cegui->GetWindowManagerPtr ()->getWindow("Background");
      CEGUI::Window* root = cegui->GetWindowManagerPtr ()->getWindow("Root");
      if (image && root) root->removeChildWindow(image);
    }


    const char* regionname = cmdline->GetOption("world");

    if (!regionname) 
    {
      regionname = regionEv->regionName.c_str();
      Report(PT::Notify, "loadRegion: Using default world.");
    }

    csRef<iCelEntity> entity = pl->CreateEntity();
    pl->CreatePropertyClass(entity, "pczonemanager");

    csRef<iPcZoneManager> pczonemgr = CEL_QUERY_PROPCLASS_ENT (entity,
      iPcZoneManager);

    entity->SetName("ptworld");
    pczonemgr->SetLoadingMode(CEL_ZONE_NORMAL);
    world_loaded = pczonemgr->Load("/peragro/art/world/", "regions.xml");
    iCelRegion* region = pczonemgr->FindRegion(regionname);
    if (region) pczonemgr->ActivateRegion(region);

    entitymanager->setWorldloaded(world_loaded);

    // Stop the intro music.
    sndstream->Pause ();

    // Remove the portal.
    bool ref = app_cfg->GetBool("Client.waterreflections");
    if (!ref)
    {
      iMeshWrapper* portalMesh = engine->FindMeshObject("portal");
      if (portalMesh) engine->RemoveObject(portalMesh->QueryObject());
      Report(PT::Notify, "loadRegion: Waterportal removed!");
    }

    return true;
  }

  void Client::sawServer()
  {
    last_seen = csGetTicks();
  }

  iPcActorMove* Client::getPcActorMove()
  {
    iCelEntity* entity = entitymanager->getOwnCelEntity();
    if (entity == 0) 
      return 0;
    csRef<iPcActorMove> pcactormove = CEL_QUERY_PROPCLASS_ENT(entity, iPcActorMove);
    return pcactormove;
  }

} // PT namespace 

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

  return csApplicationRunner<PT::Client>::Run (argc, argv);
}
