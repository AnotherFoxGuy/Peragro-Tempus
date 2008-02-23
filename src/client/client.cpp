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
#include "client/data/effect/effectsmanager.h"
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

#include "client/trade/trademanager.h"
#include "client/trade/playerinventory.h"

//#include "common/util/wincrashdump.h"
#include "common/version.h"

#include "client/world/world.h"

#define NO_CHARACTER_SELECTED_0 0

CS_IMPLEMENT_APPLICATION

namespace PT
{

  Client::Client() : playing(false)
  {
    SetApplicationName ("Client");
    state = STATE_INITIAL;
    timer = 0;
    limitFPS = 60;
    last_sleep = 0;
    world_loaded = false;
    last_seen = 0;
    char_id = NO_CHARACTER_SELECTED_0;

    reporter = 0;
    network = 0;
    guimanager = 0;
    effectsmanager = 0;
    entitymanager = 0;
    combatmanager = 0;
    effectDataManager = 0;
    skillDataManager = 0;
    connectionDataManager = 0;
    serverSetupManager = 0;
    cursor = 0;
    inputMgr = 0;
    statmanager = 0;

    eventmanager = 0;
    chatmanager = 0;
    trademanager = 0;
    sectorDataManager = 0;

    playerinventory = 0;
  }

  Client::~Client()
  {
    delete combatmanager;
    delete entitymanager;
    delete effectsmanager;
    delete guimanager;
    delete network;
    delete cursor;
    delete effectDataManager;
    delete skillDataManager;
    delete connectionDataManager;
    delete serverSetupManager;
    delete inputMgr;
    delete reporter;
    delete trademanager;
    delete sectorDataManager;
    delete statmanager;
    delete playerinventory;
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
    // To get the hopping effect, change the limitFPS attribute in constructor to something sane,
    // like 50 FPS or so.
    //TODO: Implement some way of getting the current FPS, and pass that instead of limit.
    if (Entity::PlayerEntity::Instance()) Entity::PlayerEntity::Instance()->CameraDraw(limitFPS);

    if (state == STATE_PLAY)
    {
      if (entitymanager)
      {
        PT::Entity::PlayerEntity *player = Entity::PlayerEntity::Instance();
        if (player)
        {
          player->UpdatePlayerStats();
          float currentStamina = player->GetCurrentStamina();
          float maxStamina = player->GetMaxStamina();
          float ratio = currentStamina / maxStamina;
          if (guimanager)
          {
            guimanager->GetHUDWindow()->SetSP(ratio);
            char buffer[40];
            sprintf(buffer, "            %d/%d", (int)currentStamina,
                                                 (int)maxStamina);
            guimanager->GetHUDWindow()->SetText("PlayerHUD/SPValue", buffer);
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
    reporter= new Reporter ();
    if (!reporter) return ReportError("Error loading Reporter!");
    reporter->Initialize();

#ifdef CS_STATIC_LINKED
    reporter->SetLoggingLevel(PT::Errors);
#else
    reporter->SetLoggingLevel(PT::Insane);
#endif

    vfs = csQueryRegistry<iVFS> (GetObjectRegistry());
    if (!vfs) return Report(PT::Error, "Failed to locate VFS!");

    g3d = csQueryRegistry<iGraphics3D> (GetObjectRegistry());
    if (!g3d) return Report(PT::Error, "Failed to locate 3D renderer!");

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
    eventmanager = new PT::Events::EventManager();
    if (!eventmanager->Initialize())
      return Report(PT::Error, "Failed to initialize EventManager!");
    pointerlib.setEventManager(eventmanager);

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

    // Create and Initialize the GUImanager.
    guimanager = new GUIManager ();
    if (!guimanager->Initialize ())
      return Report(PT::Error, "Failed to initialize GUIManager!");
    pointerlib.setGUIManager(guimanager);

    // Create and Initialize the InputManager.
    inputMgr = new InputManager();
    if(!inputMgr->Initialize())
      return Report(PT::Error, "Failed to create InputManager object!");

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

    // It's used in PreProcessFrame(), so store this boolean.
    enable_reflections = app_cfg->GetBool("Client.waterreflections");

    iNativeWindow* nw = g3d->GetDriver2D()->GetNativeWindow ();
    if (nw) nw->SetTitle ("Peragro Tempus");

    InitializeCEL();

    // Create the cursor.
    cursor = new Cursor(this);
    if (!cursor) return Report(PT::Error, "Can't create cursor!");
    pointerlib.setCursor(cursor);

    // Create and Initialize the EntityManager.
    entitymanager = new PT::Entity::EntityManager ();
    if (!entitymanager->Initialize())
      return Report(PT::Error, "Failed to initialize EntityManager!");
    pointerlib.setEntityManager(entitymanager);

    // Create and Initialize the Effectsmanager.
    effectsmanager = new PT::Effect::EffectsManager ();
    if (!effectsmanager->Initialize())
      return Report(PT::Error, "Failed to initialize EffectsManager!");
    pointerlib.setEffectsManager(effectsmanager);

    // Create and Initialize the Combatmanager.
    combatmanager = new CombatMGR ();
    if (!combatmanager->Initialize())
      return Report(PT::Error, "Failed to initialize CombatManager!");
    pointerlib.setCombatManager(combatmanager);

    // Create and Initialize the ChatManager.
    chatmanager = new PT::Chat::ChatManager ();
    if (!chatmanager->Initialize())
      return Report(PT::Error, "Failed to initialize ChatManager!");
    pointerlib.setChatManager(chatmanager);

    // Create and Initialize the TradeManager.
    trademanager = new PT::Trade::TradeManager ();
    if (!trademanager->Initialize())
      return Report(PT::Error, "Failed to initialize TradeManager!");
    //pointerlib.setTradeManager(trademanager);

    // Create and Initialize the PlayerInventory.
    playerinventory = new PT::Trade::PlayerInventory ();
    if (!playerinventory->Initialize())
      return Report(PT::Error, "Failed to initialize PlayerInventory!");

    // Create and Initialize the StatManager.
    statmanager = new PT::Entity::StatManager ();
    if (!statmanager->Initialize())
      return Report(PT::Error, "Failed to initialize StatManager!");
    pointerlib.setStatManager(statmanager);

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

    // Register listener for RegionLoadEvent.
    EventHandler<Client>* cbLoad = new EventHandler<Client>(&Client::LoadRegion, this);
    PointerLibrary::getInstance()->getEventManager()->AddListener("world.loaded", cbLoad);

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
        guimanager->GetLoginWindow ()->ShowWindow ();
        guimanager->GetServerWindow ()->ShowWindow ();

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
        checkConnection();
        entitymanager->Handle();
        break;
      }
    }
  }

  void Client::checkConnection()
  {
    //Report(PT::Notify, "Saw server %d ms ago.", csGetTicks() - last_seen);
    size_t ticks = csGetTicks();
    if ( last_seen > 0 && ticks - last_seen > 10000 && ! network->isRunning() && state >= STATE_CONNECTED )
    {
      last_seen = csGetTicks();
      Report(PT::Warning, "Disconnect!");

      //TODO: Make it actually delete all entities like it claims to do...
      entitymanager->delAllEntities();

      entitymanager->Handle();

      state = STATE_RECONNECTED;

      network->init();

      if (!network->isRunning())
      {
        guimanager->CreateOkWindow()->ShowWindow();
        guimanager->CreateOkWindow()->BringToFront();
        guimanager->CreateOkWindow()->SetText("Disconnect!\n Trying to reconnect, please wait!");
        return;
      }

      guimanager->CreateOkWindow()->HideWindow();

      ConnectRequestMessage msg(CLIENTVERSION);
      network->send(&msg);
    }
  }

  bool Client::Connected (PT::Events::Eventp ev)
  {
    using namespace PT::Events;

    StateConnectedEvent* stateEv = GetStateEvent<StateConnectedEvent*>(ev);
    if (!stateEv) return false;

    if (state == STATE_RECONNECTED)
    {
      login(user, pass);
    }
    else
    {
      if (cmdline && state == STATE_INTRO)
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

      state = STATE_CONNECTED;
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

  bool Client::ActionActivateSkill(PT::Events::Eventp ev)
  {
    using namespace PT::Events;

    if (playing)
    {
      InputEvent* inputEv = GetInputEvent<InputEvent*>(ev);
      if (!inputEv) return false;

      if (!inputEv->released)
      {
        // Activate the skill
        csRef<iCelEntity> ent = cursor->GetSelectedEntity();
        csRef<iPcProperties> pcprop;
        if (ent) pcprop = CEL_QUERY_PROPCLASS_ENT(ent, iPcProperties);
        if (!pcprop) return false;
        if (pcprop->GetPropertyLong(pcprop->GetPropertyIndex("Entity Type")) == PT::Entity::PCEntityType)
        {
          combatmanager->levelup(pcprop->GetPropertyLong(pcprop->GetPropertyIndex("Entity ID")));
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

  bool Client::ActionQuit(PT::Events::Eventp ev)
  {
    using namespace PT::Events;

    if (playing)
    {
      ConfirmDialogWindow* dialog = guimanager->CreateConfirmWindow();
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

  bool Client::OnMouseMove(iEvent& e)
  {
    if (!Entity::PlayerEntity::Instance()) return false;
    csRef<iPcDefaultCamera> cam = Entity::PlayerEntity::Instance()->GetCamera();
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
      guimanager->GetLoginWindow()->EnableWindow();
      guimanager->GetServerWindow()->EnableWindow();
      //network->stop();
      //state = STATE_INTRO;
      guimanager->CreateOkWindow(true)->SetText(stateev->errorMessage.c_str());
      return true;
    }
    else
      Report(PT::Notify, "Login succeeded!");


    if (state == STATE_RECONNECTED && char_id != NO_CHARACTER_SELECTED_0)
    {
      selectCharacter(char_id);
    }
    else if (state == STATE_CONNECTED)
    {
      guimanager->GetLoginWindow ()->HideWindow ();
      guimanager->GetServerWindow ()->HideWindow ();
      guimanager->GetSelectCharWindow ()->ShowWindow ();
      if(stateev->isAdmin){guimanager->GetSelectCharWindow ()->ShowAdminButton();}

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

  bool Client::PlayingEvent(PT::Events::Eventp ev)
  {
    using namespace PT::Events;

    StatePlayEvent* playEv = GetStateEvent<StatePlayEvent*>(ev);
    if (!playEv) return false;

    state = STATE_PLAY;

    return true;
  }

  bool Client::LoadRegion(PT::Events::Eventp ev)
  {
    using namespace PT::Events;

    WorldLoadedEvent* worldEv = GetWorldEvent<WorldLoadedEvent*>(ev);
    if (!worldEv) return false;

    iCelEntity* ent = pl->FindEntity("ptIntroWorld");
    if (ent)
    {
      csRef<iPcRegion> pcreg = CEL_QUERY_PROPCLASS_ENT(ent, iPcRegion);
      pcreg->Unload();
      pl->RemoveEntity(ent);
    }

    //TODO: We need a StateManager singleton class for this kind of thing.
    //It really sucks as it is now.
    playing = true;
    entitymanager->setPlaying(playing);
    combatmanager->SetPlaying(playing);

    sawServer();
    state = STATE_PLAY;

    if (!world_loaded) 
    {
      guimanager->GetSelectCharWindow ()->HideWindow();
      guimanager->GetOptionsWindow ()->HideWindow();

      guimanager->GetChatWindow ()->ShowWindow();
      guimanager->GetHUDWindow ()->ShowWindow();


      // Hide the background.
      iCEGUI* cegui = guimanager->GetCEGUI();
      if (cegui->GetWindowManagerPtr ()->isWindowPresent("Root")
        && cegui->GetWindowManagerPtr ()->isWindowPresent("Background") )
      {
        CEGUI::Window* image = cegui->GetWindowManagerPtr ()->getWindow("Background");
        CEGUI::Window* root = cegui->GetWindowManagerPtr ()->getWindow("Root");
        if (image && root) root->removeChildWindow(image);
      }

      // Little hack to restore focus.
      guimanager->GetCEGUI()->GetWindowManagerPtr ()->getWindow("Chatlog/Frame")->activate();

      // Stop the intro music.
      sndstream->Pause ();
    }

    // Enable reflection.
    if (enable_reflections)
    {
      Reflection::ReflectionUtils::ApplyReflection(view, GetObjectRegistry());
      Reflection::ReflectionUtils::SetFrameSkip(app_cfg->GetInt("Client.reflectionskip"));
      Report(PT::Notify, "loadRegion: Enabled reflections!");
    }

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
