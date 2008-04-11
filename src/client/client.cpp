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
#include "include/clipboard.h"
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
#include "client/effect/effectsmanager.h"
#include "client/data/effect/effectdatamanager.h"
#include "client/data/connection/connectiondatamanager.h"
#include "client/serversetup/serversetupmanager.h"
#include "client/combat/combatmanager.h"
#include "client/data/skilldatamanager.h"
#include "client/data/sector.h"
#include "client/data/sectordatamanager.h"
#include "client/entity/entitymanager.h"
#include "client/chat/chatmanager.h"
#include "client/data/effect/reflection.h"
#include "client/entity/statmanager.h"
#include "client/environment/environmentmanager.h"

#include "client/trade/trademanager.h"

//#include "common/util/wincrashdump.h"
#include "common/version.h"

#include "client/world/world.h"

#define NO_CHARACTER_SELECTED_0 0

CS_IMPLEMENT_APPLICATION

namespace PT
{

  Client::Client()
  {
    SetApplicationName ("Client");
    timer = 0;
    limitFPS = 60;
    last_sleep = 0;
    world_loaded = false;
    last_seen = 0;
    char_id = NO_CHARACTER_SELECTED_0;

    reporter = 0;
    network = 0;
    cursor = 0;

    eventManager = 0;
    effectDataManager = 0;
    sectorDataManager = 0;
    skillDataManager = 0;
    connectionDataManager = 0;
    serverSetupManager = 0;
    guiManager = 0;
    inputManager = 0;
    stateManager = 0;
    // Don't set environmentManager = 0;
    reflectionRenderer = 0;

    entityManager = 0;
    statManager = 0;
    effectsManager = 0;
    combatManager = 0;
    chatManager = 0;
    tradeManager = 0;

    // Don't set world = 0;
  }

  Client::~Client()
  {
    delete reporter;
    delete network;
    delete cursor;

    // Don't delete eventManager, that is taken care of by the boost::shared_ptr
    // in struct PT:Events::EventManager::Listener
    delete effectDataManager;
    delete sectorDataManager;
    delete skillDataManager;
    delete connectionDataManager;
    delete serverSetupManager;
    delete guiManager;
    delete inputManager;
    delete stateManager;
    // Don't delete environmentManager;
    delete reflectionRenderer;

    delete entityManager;
    delete statManager;
    delete effectsManager;
    delete combatManager;
    delete chatManager;
    delete tradeManager;

    // Don't delete world;
  }

  void Client::PreProcessFrame()
  {
    csTicks ticks = vc->GetElapsedTicks();
    timer += ticks;

    effectsManager->HandleEffects(ticks);

    if (limitFPS > 0)
    {
      if (ticks < 1000.0f/limitFPS)
        csSleep((int)1000.0f/limitFPS - ticks);
    }

    if (timer > 1000)
    {
      timer = 0;
      if (stateManager->GetState() == STATE_PLAY)
      {
        entityManager->DrUpdateOwnEntity();
      }
    }
  }

  void Client::ProcessFrame()
  {
    g3d->BeginDraw (engine->GetBeginDrawFlags () | CSDRAW_3DGRAPHICS);

    handleStates();

    if (entityManager)
    {
      PT::Entity::PlayerEntity *player = Entity::PlayerEntity::Instance();
      if (player)
      {
        // Draw the player camera manually.
        static csTicks lastTicks = vc->GetCurrentTicks();
        csTicks elapsedTicks = vc->GetCurrentTicks() - lastTicks;
        player->CameraDraw(elapsedTicks);

        // This is done so that the time accumulates when the elapsed time is
        // less than 0 ticks, to keep any calculations correct.
        if (elapsedTicks > 0) lastTicks = vc->GetCurrentTicks();

        if (stateManager->GetState() == STATE_PLAY)
        {
          player->UpdatePlayerStats();
          if (guiManager)
          {
            float currentStamina = player->GetCurrentStamina();
            float maxStamina = player->GetMaxStamina();
            float ratio = currentStamina / maxStamina;
            guiManager->GetHUDWindow()->SetSP(ratio);
            char buffer[40];
            sprintf(buffer, "            %d/%d", (int)currentStamina,
                                                 (int)maxStamina);
            guiManager->GetHUDWindow()->SetText("PlayerHUD/SPValue", buffer);
          }
        }
      }
    }

    // Paint the interface over the engine
    if (guiManager) guiManager->Render ();
  }

  void Client::FinishFrame()
  {
    g3d->FinishDraw();
    g3d->Print(0);
  }

  bool Client::OnInitialize(int argc, char* argv[])
  {
    setenv("APPDIR", csInstallationPathsHelper::GetAppDir(argv[0]).GetData(), true);

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
    reporter = new Reporter ();
    if (!reporter) return ReportError("Error loading Reporter!");
    reporter->Initialize();
    pointerlib.setReporter(reporter);

#ifdef CS_STATIC_LINKED
    reporter->SetLoggingLevel(PT::Errors);
#else
    reporter->SetLoggingLevel(PT::Insane);
#endif

    vfs = csQueryRegistry<iVFS> (GetObjectRegistry());
    if (!vfs) return Report(PT::Error, "Failed to locate VFS!");

    g3d = csQueryRegistry<iGraphics3D> (GetObjectRegistry());
    if (!g3d) return Report(PT::Error, "Failed to locate 3D renderer!");

    iNativeWindow* nw = g3d->GetDriver2D()->GetNativeWindow ();
    if (nw) nw->SetTitle ("Peragro Tempus");

    csRef<iPluginManager> plugin_mgr (csQueryRegistry<iPluginManager> (object_reg));
    csRef<iClipboard> csTheClipboard = csLoadPlugin<iClipboard> (plugin_mgr, "crystalspace.gui.clipboard");
    if(csTheClipboard.IsValid())
        object_reg->Register (csTheClipboard, "iClipboard");
    else
        Report(PT::Error, "Failed to load the iClipboard!");

    pointerlib.setObjectRegistry(GetObjectRegistry());
    pointerlib.setClient(this);

    // Create and Initialize the Network.
    network = new Network (this);
    if (!network)
      return Report(PT::Error, "Failed to create Network object!");
    //network->init();
    pointerlib.setNetwork(network);

    // Create and Initialize the EventManager.
    eventManager = new PT::Events::EventManager();
    if (!eventManager->Initialize(&pointerlib))
      return Report(PT::Error, "Failed to initialize EventManager!");
    pointerlib.setEventManager(eventManager);

    // Create and Initialize the EffectDataManager.
    effectDataManager = new PT::Data::EffectDataManager ();
    if (!effectDataManager->LoadEffectData())
      return Report(PT::Error, "Failed to initialize EffectDataManager!");
    pointerlib.setEffectDataManager(effectDataManager);

    // Create and Initialize the SectorDataManager.
    sectorDataManager = new PT::Data::SectorDataManager ();
    if (!sectorDataManager->parse())
      return Report(PT::Error, "Failed to initialize SectorManager!");
    pointerlib.setSectorDataManager(sectorDataManager);

    // Create and Initialize the SkillDataManager.
    skillDataManager = new PT::Data::SkillDataManager ();
    if (!skillDataManager->parse())
      return Report(PT::Error, "Failed to initialize SkillDataManager!");
    pointerlib.setSkillDataManager(skillDataManager);

    // Create and Initialize the ConnectionDataManager.
    connectionDataManager = new PT::Data::ConnectionDataManager ();
    if (!connectionDataManager->LoadServerData())
      return Report(PT::Error, "Failed to initialize ConnectionDataManager!");
    pointerlib.setConnectionDataManager(connectionDataManager);

    // Create the ServerSetupManager.
    serverSetupManager = new PT::Misc::ServerSetupManager ();
    pointerlib.setServerSetupManager(serverSetupManager);

    // Create and Initialize the GUIManager.
    guiManager = new GUIManager ();
    if (!guiManager->Initialize ())
      return Report(PT::Error, "Failed to initialize GUIManager!");
    pointerlib.setGUIManager(guiManager);

    // Create and Initialize the InputManager.
    inputManager = new InputManager();
    if(!inputManager->Initialize())
      return Report(PT::Error, "Failed to create InputManager object!");

    // Create and Initialize the StateManager.
    stateManager = new StateManager();
    if(!stateManager->Initialize())
      return Report(PT::Error, "Failed to create StateManager object!");
    pointerlib.setStateManager(stateManager);

    // Create and Initialize the EnvironmentManager.
    environmentManager = new EnvironmentManager();
    if(!environmentManager->Initialize())
      return Report(PT::Error, "Failed to create EnvironmentManager object!");
    pointerlib.setEnvironmentManager(environmentManager);

    if (!RegisterQueue(GetObjectRegistry(), csevAllEvents(GetObjectRegistry())))
      return Report(PT::Error, "Failed to set up event handler!");

    engine = csQueryRegistry<iEngine> (GetObjectRegistry());
    if (!engine) return Report(PT::Error, "Failed to locate 3D engine!");

    vc = csQueryRegistry<iVirtualClock> (GetObjectRegistry());
    if (!vc) return Report(PT::Error, "Failed to locate Virtual Clock!");

    sndrenderer = csQueryRegistry<iSndSysRenderer> (GetObjectRegistry());
    if (!sndrenderer) return Report(PT::Error, "Failed to locate sound renderer!");

    sndloader = csQueryRegistry<iSndSysLoader> (GetObjectRegistry());
    if (!sndloader) return Report(PT::Error, "Failed to locate sound loader!");

    app_cfg = csQueryRegistry<iConfigManager> (GetObjectRegistry());
    if (!app_cfg) return Report(PT::Error, "Can't find the config manager!");

    // Enable reflection.
    bool enable_reflections = app_cfg->GetBool("Client.waterreflections");
    if (enable_reflections)
    {
      reflectionRenderer = new PT::Reflection::ReflectionRenderer();
      if (!reflectionRenderer->Initialize())
        return Report(PT::Error, "Failed to initialize reflection!");

      reflectionRenderer->SetFrameSkip(app_cfg->GetInt("Client.reflectionskip"));
      Report(PT::Notify, "Enabled reflections!");
    }

    limitFPS = static_cast<int>( app_cfg->GetFloat("Client.maxFPS", limitFPS) );

    InitializeCEL();

    // Create the cursor.
    cursor = new Cursor();
    if (!cursor) return Report(PT::Error, "Can't create cursor!");
    pointerlib.setCursor(cursor);

    // Create and Initialize the EntityManager.
    entityManager = new PT::Entity::EntityManager ();
    if (!entityManager->Initialize())
      return Report(PT::Error, "Failed to initialize EntityManager!");
    pointerlib.setEntityManager(entityManager);

    // Create and Initialize the StatManager.
    statManager = new PT::Entity::StatManager ();
    if (!statManager->Initialize())
      return Report(PT::Error, "Failed to initialize StatManager!");
    pointerlib.setStatManager(statManager);

    // Create and Initialize the EffectsManager.
    effectsManager = new PT::Effect::EffectsManager ();
    if (!effectsManager->Initialize())
      return Report(PT::Error, "Failed to initialize EffectsManager!");
    pointerlib.setEffectsManager(effectsManager);

    // Create and Initialize the CombatManager.
    combatManager = new PT::Combat::CombatManager ();
    if (!combatManager->Initialize())
      return Report(PT::Error, "Failed to initialize CombatManager!");
    pointerlib.setCombatManager(combatManager);

    // Create and Initialize the ChatManager.
    chatManager = new PT::Chat::ChatManager ();
    if (!chatManager->Initialize())
      return Report(PT::Error, "Failed to initialize ChatManager!");
    pointerlib.setChatManager(chatManager);

    // Create and Initialize the TradeManager.
    tradeManager = new PT::Trade::TradeManager ();
    if (!tradeManager->Initialize())
      return Report(PT::Error, "Failed to initialize TradeManager!");
    //pointerlib.setTradeManager(tradeManager);

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
    sndsource3d = scfQueryInterface<iSndSysSource3D> (sndsource);

    sndsource3d->SetPosition (csVector3(0,0,0));
    sndsource->SetVolume (1.0f);

    sndstream->SetLoopState (CS_SNDSYS_STREAM_LOOP);
    sndstream->Unpause ();

    // end intro sound
    csRef<iLoader >loader = csQueryRegistry<iLoader> (GetObjectRegistry());
    if (!loader) return Report(PT::Error, "Failed to locate Loader!");
    loader->LoadLibraryFile("/peragro/xml/quests/doorquests.xml");

    //Listen for events.
    using namespace PT::Events;

    // Register listener for StateConnectedEvent.
    EventHandler<Client>* cbConnected = new EventHandler<Client>(&Client::Connected, this);
    PointerLibrary::getInstance()->getEventManager()->AddListener("state.connected", cbConnected);

    // Register listener for StateLoggedInEvent.
    EventHandler<Client>* cbLoggedIn = new EventHandler<Client>(&Client::loggedIn, this);
    PointerLibrary::getInstance()->getEventManager()->AddListener("state.loggedin", cbLoggedIn);

    // Register listener for state.play.
    EventHandler<Client>* cbPlay = new EventHandler<Client>(&Client::PlayingEvent, this);
    PointerLibrary::getInstance()->getEventManager()->AddListener("state.play", cbPlay);

    // Register listener for RegionLoadingEvent.
    EventHandler<Client>* cbLoading = new EventHandler<Client>(&Client::LoadingRegion, this);
    PointerLibrary::getInstance()->getEventManager()->AddListener("world.loading", cbLoading);

    // Register listener for RegionLoadedEvent.
    EventHandler<Client>* cbLoaded = new EventHandler<Client>(&Client::LoadRegion, this);
    PointerLibrary::getInstance()->getEventManager()->AddListener("world.loaded", cbLoaded);

    //Actions

    // Register listener for ActionActivateSkill.
    EventHandler<Client>* cbActionActivateSkill = new EventHandler<Client>(&Client::ActionActivateSkill, this);
    PointerLibrary::getInstance()->getEventManager()->AddListener("input.ACTION_ACTIVATESKILL", cbActionActivateSkill);

    // Register listener for ActionQuit.
    EventHandler<Client>* cbActionQuit = new EventHandler<Client>(&Client::ActionQuit, this);
    PointerLibrary::getInstance()->getEventManager()->AddListener("input.ACTION_QUIT", cbActionQuit);

    // Disable the lighting cache.
    engine->SetLightingCacheMode (CS_ENGINE_CACHE_NOUPDATE);

    // Create the world.
    world = new World("MyWorld", GetObjectRegistry());
    pointerlib.setWorld(world);

    // Let the engine prepare all lightmaps for use and also free all images
    // that were loaded for the texture manager.
    engine->Prepare ();

    Run();

    return true;
  }

  void Client::handleStates()
  {
    switch(stateManager->GetState())
    {
    case STATE_INITIAL: // Initial state. Load intro sector and go to STATE_INTRO.
      {
        // Load introduction sector, draw it once for this frame, and switch to STATE_INTRO
        const char* path = cmdline->GetOption("intro");

        if (!path)
        {
          path = 0;
          iCEGUI* cegui = guiManager->GetCEGUI();

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
        guiManager->GetLoginWindow ()->ShowWindow ();
        guiManager->GetServerWindow ()->ShowWindow ();

        if (cmdline)
        {
          const char* host = cmdline->GetOption("host");
          const char* port = cmdline->GetOption("port");
          if (host)
          {
            if(!port){port = "12345";}
            ConnectRequestMessage msg(CLIENTVERSION);
            network->setServerAddress(host, atoi(port));
            network->init();
            network->send(&msg);
          }
        }

        stateManager->SetState(STATE_INTRO);
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
        entityManager->Handle();
        break;
      }
    case STATE_RECONNECTED:
      {
        checkConnection();
        entityManager->Handle();
        break;
      }
    }
  }

  void Client::checkConnection()
  {
    //Report(PT::Notify, "Saw server %d ms ago.", csGetTicks() - last_seen);
    size_t ticks = csGetTicks();
    if ( last_seen > 0 && ticks - last_seen > 10000 && ! network->isRunning() && stateManager->GetState() >= STATE_CONNECTED )
    {
      last_seen = csGetTicks();
      Report(PT::Warning, "Disconnect!");

      //TODO: Make it actually delete all entities like it claims to do...
      entityManager->delAllEntities();

      entityManager->Handle();

      stateManager->SetState(STATE_RECONNECTED);

      network->init();

      if (!network->isRunning())
      {
        guiManager->CreateOkWindow()->ShowWindow();
        guiManager->CreateOkWindow()->BringToFront();
        guiManager->CreateOkWindow()->SetText("Disconnect!\n Trying to reconnect, please wait!");
        return;
      }

      guiManager->CreateOkWindow()->HideWindow();

      ConnectRequestMessage msg(CLIENTVERSION);
      network->send(&msg);
    }
  }

  bool Client::Connected (iEvent& ev)
  {
    using namespace PT::Events;

    if (stateManager->GetState() == STATE_RECONNECTED)
    {
      login(user, pass);
    }
    else
    {
      if (cmdline && stateManager->GetState() == STATE_INTRO)
      {
        const char* user = cmdline->GetOption("user", 0);
        const char* pass = cmdline->GetOption("pass", 0);

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

      stateManager->SetState(STATE_CONNECTED);
    }

    Report(PT::Notify, "Connected!");
    return true;
  }

  void Client::login(const std::string& user, const std::string& pass)
  {
    this->user = user;
    this->pass = pass;

    LoginRequestMessage answer_msg;
    answer_msg.setUsername(ptString(user.c_str(), strlen(user.c_str())));
    answer_msg.setPassword(pass.c_str());
    network->send(&answer_msg);
  }

  bool Client::ActionActivateSkill(iEvent& ev)
  {
    using namespace PT::Events;

    if (stateManager->GetState() == STATE_PLAY)
    {
      if (InputHelper::GetButtonDown(&ev))
      {
        // Activate the skill
        csRef<iCelEntity> ent = cursor->GetSelectedEntity();
        csRef<iPcProperties> pcprop;
        if (ent) pcprop = CEL_QUERY_PROPCLASS_ENT(ent, iPcProperties);
        if (!pcprop) return false;
        if (pcprop->GetPropertyLong(pcprop->GetPropertyIndex("Entity Type")) == PT::Entity::PCEntityType)
        {
          combatManager->LevelUp(pcprop->GetPropertyLong(pcprop->GetPropertyIndex("Entity ID")));
        }
        // Animate the player.
        iCelEntity* entity = Entity::PlayerEntity::Instance()->GetCelEntity();
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

  bool Client::ActionQuit(iEvent& ev)
  {
    using namespace PT::Events;

    if (stateManager->GetState() == STATE_PLAY)
    {
      ConfirmDialogWindow* dialog = guiManager->CreateConfirmWindow();
      dialog->SetText("Are you sure you want to quit?");
      dialog->SetYesEvent(CEGUI::Event::Subscriber(&Client::Quit, this));
      dialog->SetNoEvent(CEGUI::Event::Subscriber(&Client::NoQuit, this));
      return true;
    } else {
      csRef<iEventQueue> q = csQueryRegistry<iEventQueue> (GetObjectRegistry());
      if (q.IsValid())
      {
        q->GetEventOutlet()->Broadcast(csevQuit(GetObjectRegistry()));
        return true;
      }
    }
    return true;
  }

  bool Client::Quit(const CEGUI::EventArgs &args)
  {
    csRef<iEventQueue> q = csQueryRegistry<iEventQueue> (GetObjectRegistry());
    if (q.IsValid()) q->GetEventOutlet()->Broadcast(csevQuit(GetObjectRegistry()));

    return true;
  }

  bool Client::NoQuit(const CEGUI::EventArgs &args)
  {
    return true;
  }

  bool Client::InitializeCEL()
  {
    iObjectRegistry* object_reg = this->GetObjectRegistry();

    pl = csQueryRegistry<iCelPlLayer> (object_reg);
    if (!pl) return Report(PT::Error, "Failed to load CEL Physical Layer!");

    return true;
  }

  bool Client::loggedIn(iEvent& ev)
  {
    using namespace PT::Events;

    if (StateHelper::GetError(&ev))
    {
      Report(PT::Error, "Login Failed due to: %s.", StateHelper::GetErrorMessage(&ev).c_str());
      GUIManager* guiManager = PointerLibrary::getInstance()->getGUIManager();
      guiManager->GetLoginWindow()->EnableWindow();
      guiManager->GetServerWindow()->EnableWindow();
      //network->stop();
      //stateManager->SetState(STATE_INTRO);
      guiManager->CreateOkWindow(true)->SetText(StateHelper::GetErrorMessage(&ev).c_str());
      return true;
    }
    else
      Report(PT::Notify, "Login succeeded!");


    if (stateManager->GetState() == STATE_RECONNECTED && char_id != NO_CHARACTER_SELECTED_0)
    {
      selectCharacter(char_id);
    }
    else if (stateManager->GetState() == STATE_CONNECTED)
    {
      guiManager->GetLoginWindow ()->HideWindow ();
      guiManager->GetServerWindow ()->HideWindow ();
      guiManager->GetSelectCharWindow ()->ShowWindow ();

      bool isAdmin = false;
      ev.Retrieve("isAdmin", isAdmin);
      if(isAdmin){guiManager->GetSelectCharWindow ()->ShowAdminButton();}

      stateManager->SetState(STATE_LOGGED_IN);

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
    const size_t NUM_OPTIONS = 6;

    const csOptionDescription pt_config_options [NUM_OPTIONS] =
    {
      { 0, "host", "Hostname the client will connect to (peragro.org)", CSVAR_STRING },
      { 1, "port", "Port number the client will use to connect to the server", CSVAR_STRING },
      { 2, "useCD", "Enable/Disable Collision Detection on the client", CSVAR_BOOL },
      { 3, "user", "Automatic Login with the given Username", CSVAR_STRING },
      { 4, "pass", "Automatic Login with the given Password", CSVAR_STRING },
      { 5, "char", "Automatic selection of the character with the given ID", CSVAR_LONG },
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

  bool Client::PlayingEvent(iEvent& ev)
  {
    using namespace PT::Events;

    stateManager->SetState(STATE_PLAY);

    return true;
  }

  bool Client::LoadingRegion(iEvent& ev)
  {
    using namespace PT::Events;

    if(!guiManager->GetLoadScreenWindow()->IsVisible())
    {
      guiManager->GetLoadScreenWindow()->ShowWindow();
      iCEGUI* cegui = guiManager->GetCEGUI();
      if(!cegui->GetImagesetManagerPtr()->isImagesetPresent("LoadScreen")){ // TODO: Different loading screens for different tiles(?)
        vfs->ChDir ("/peragro/skin/");
        cegui->GetImagesetManagerPtr()->createImagesetFromImageFile("LoadScreen", "loadscreen.jpg");
        CEGUI::Window* image = cegui->GetWindowManagerPtr()->getWindow("LoadScreen");
        image->setProperty("Image", "set:LoadScreen image:full_image");
        image->setProperty("BackgroundEnabled", "True");
      }
    }
    guiManager->GetLoadScreenWindow()->SetProgress(WorldHelper::GetProgress(&ev));
    return true;
  }

  bool Client::LoadRegion(iEvent& ev)
  {
    using namespace PT::Events;

    iCelEntity* ent = pl->FindEntity("ptIntroWorld");
    if (ent)
    {
      csRef<iPcRegion> pcreg = CEL_QUERY_PROPCLASS_ENT(ent, iPcRegion);
      pcreg->Unload();
      pl->RemoveEntity(ent);
    }

    stateManager->SetState(STATE_PLAY);

    sawServer();

    if (!world_loaded)
    {
      guiManager->GetSelectCharWindow ()->HideWindow();
      guiManager->GetOptionsWindow ()->HideWindow();

      guiManager->GetChatWindow ()->ShowWindow();
      guiManager->GetHUDWindow ()->ShowWindow();


      // Hide the background.
      iCEGUI* cegui = guiManager->GetCEGUI();
      if (cegui->GetWindowManagerPtr ()->isWindowPresent("Root")
        && cegui->GetWindowManagerPtr ()->isWindowPresent("Background") )
      {
        CEGUI::Window* image = cegui->GetWindowManagerPtr ()->getWindow("Background");
        CEGUI::Window* root = cegui->GetWindowManagerPtr ()->getWindow("Root");
        if (image && root) root->removeChildWindow(image);
      }

      // Little hack to restore focus.
      guiManager->GetCEGUI()->GetWindowManagerPtr ()->getWindow("Chatlog/Frame")->activate();

      // Stop the intro music.
      sndstream->Pause ();
    }

    guiManager->GetLoadScreenWindow()->HideWindow();

    world_loaded = true;
    PointerLibrary::getInstance()->getEntityManager()->setWorldloaded(true);

    return true;
  }

  void Client::sawServer()
  {
    last_seen = csGetTicks();
  }

  iPcActorMove* Client::getPcActorMove()
  {
    iCelEntity* entity = Entity::PlayerEntity::Instance()->GetCelEntity();
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
  printf("Peragro Tempus - Client\n$Revision$\tBuild-Date: %s %s\n", __DATE__, __TIME__);
#endif
  //setWinCrashDump(argv[0]);

#ifdef CS_STATIC_LINKED
  //Overwrite existing env-vars for static linked client
  setenv("CRYSTAL", ".", true);
  setenv("CEL", ".", true);
#endif

  return csApplicationRunner<PT::Client>::Run (argc, argv);
}
