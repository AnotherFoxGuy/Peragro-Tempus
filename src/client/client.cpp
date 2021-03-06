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
#include <csutil/setenv.h>
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

#include <celtool/initapp.h>
#include <physicallayer/pl.h>
#include <physicallayer/propfact.h>
#include <physicallayer/propclas.h>
#include <propclass/region.h>
#include <propclass/zone.h>
#include <propclass/prop.h>
#include <propclass/mesh.h>

#include "common/reporter/reporter.h"

#include "cursor/cursor.h"
#include "client/network/network.h"
#include "client/gui/gui.h"
#include "client/gui/guimanager.h"
#include "client/input/inputmanager.h"
#include "client/data/connection/connectiondatamanager.h"
#include "client/serversetup/serversetupmanager.h"
#include "client/combat/combatmanager.h"
#include "client/data/sector/sector.h"
#include "client/data/sector/sectordatamanager.h"
#include "client/entity/entitymanager.h"
#include "client/chat/chatmanager.h"
#include "client/environment/environmentmanager.h"
#include "client/component/componentmanager.h"
#include "client/trade/trademanager.h"
#include "client/state/statemanager.h"
#include "client/pointerplug/pointerplug.h"

#include "client/user/usermanager.h"
#include "client/quest/questmanager.h"

#include "common/eventcs/eventmanager.h"
#include "common/eventcs/regionevent.h"
#include "common/eventcs/inputevent.h"

//#include "common/util/wincrashdump.h"
#include "common/version.h"

#include "include/world.h"
#include "include/soundmanager.h"
#include "include/effectsmanager.h"
#include "include/cachemanager.h"

#include "imystarbox.h"

const char* const appConfigFile = "/peragro/config/client.cfg";
const char* const userConfigFile = "/UserData/client.cfg";

#include <imesh/animesh.h>

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

    reporter = 0;
    network = 0;
    cursor = 0;

    eventManager = 0;
    sectorDataManager = 0;
    skillDataManager = 0;
    connectionDataManager = 0;
    serverSetupManager = 0;
    guiManager = 0;
    inputManager = 0;
    stateManager = 0;
    environmentManager = 0;

    entityManager = 0;
    combatManager = 0;
    chatManager = 0;
    tradeManager = 0;
    componentManager = 0;

    userManager = 0;
    questManager = 0;

    // Don't set world = 0;
  }

  Client::~Client()
  {
    // Shut down the event handlers we spawned earlier.
    printer.Invalidate();

    delete network;
    delete cursor;

    delete sectorDataManager;
    delete connectionDataManager;
    delete serverSetupManager;
    delete guiManager;
    delete inputManager;
    delete stateManager;
    delete environmentManager;

    delete entityManager;
    delete combatManager;
    delete chatManager;
    delete tradeManager;
    delete componentManager;

    delete userManager;
    delete questManager;

    delete eventManager;

    delete reporter;

    // Don't delete world;
  }

  void Client::Frame()
  {
    csTicks ticks = vc->GetElapsedTicks();
    timer += ticks;

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

    g3d->BeginDraw (engine->GetBeginDrawFlags () | CSDRAW_3DGRAPHICS);

    handleStates();

    // Paint the interface over the engine
    if (guiManager) guiManager->Render();

    //csRef<iCacheManager> m = csQueryRegistry<iCacheManager> (object_reg);
    //if (m) printf("Cache %uKB/%uKB\n", m->GetUsedCacheSize()/1024, m->GetCacheSize()/1024);
  }

  bool Client::OnInitialize(int argc, char* argv[])
  {
    CS::Utility::setenv("APPDIR", csInstallationPathsHelper::GetAppDir(argv[0]).GetData(), true);

    if (!csInitializer::SetupConfigManager(GetObjectRegistry(),
      appConfigFile, GetApplicationName()))
      return ReportError("Failed to initialize configuration manager!");

    app_cfg = csQueryRegistry<iConfigManager> (GetObjectRegistry());
    if (!app_cfg) return ReportError("Can't find the config manager!");

    vfs = csQueryRegistry<iVFS> (GetObjectRegistry());
    if (!vfs) return ReportError("Failed to locate VFS!");

    // Load the user configuration files as early as possible.
    if (!MountUserData()) return ReportError("Failed to mount user data!");

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

  bool Client::MountUserData()
  {
    csString userDataPath(csGetPlatformConfigPath("peragro"));

    // Move out of the ~/.crystalspace dir, where it goes with unixes.
    userDataPath.FindReplace("/.crystalspace/", "/.");

    ReportInfo("User data path: %s", userDataPath.GetData());

    // We want a directory, not a file.
    userDataPath.Append(CS_PATH_SEPARATOR);

    if (!vfs->Mount("/UserData/", userDataPath.GetData()))
    {
      return false;
    }

    // VFS apparently can't create a file in a dir that doesnt exists but the
    // immediate parent does. So let's create a dir, to create all dirs in the
    // path.
    csRef<iFile> path = vfs->Open("/UserData/.keep/", VFS_FILE_WRITE);

    // Load the user client configuration.
    csRef<iConfigFile> userCfg = app_cfg->AddDomain(userConfigFile, vfs,
      iConfigManager::ConfigPriorityUserApp);
    app_cfg->SetDynamicDomain(userCfg);

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
    reporter = new Reporter();
    if (!reporter) return ReportError("Error loading Reporter!");
    reporter->Initialize("peragro.log");
    pointerlib.setReporter(reporter);

#ifdef CS_STATIC_LINKED
    reporter->SetLoggingLevel(PT::Errors);
#else
    reporter->SetLoggingLevel(PT::Insane);
#endif

    g3d = csQueryRegistry<iGraphics3D> (GetObjectRegistry());
    if (!g3d) return Report(PT::Error, "Failed to locate 3D renderer!");

    iNativeWindow* nw = g3d->GetDriver2D()->GetNativeWindow ();
    if (nw) nw->SetTitle ("Peragro Tempus");

    csRef<iPluginManager> plugin_mgr (csQueryRegistry<iPluginManager> (object_reg));
    csRef<iClipboard> csTheClipboard = csLoadPlugin<iClipboard> (plugin_mgr, "crystalspace.gui.clipboard");
    if (csTheClipboard.IsValid())
        object_reg->Register (csTheClipboard, "iClipboard");
    else
        Report(PT::Error, "Failed to load the iClipboard!");

    pointerlib.setObjectRegistry(GetObjectRegistry());
    pointerlib.setClient(this);

    // Create and Initialize the Network.
    network = new Network ();
    if (!network)
      return Report(PT::Error, "Failed to create Network object!");
    //network->init();
    pointerlib.setNetwork(network);

    // Create and Initialize the EventManager.
    eventManager = new PT::Events::EventManager();
    if (!eventManager->Initialize(&pointerlib))
      return Report(PT::Error, "Failed to initialize EventManager!");
    pointerlib.setEventManager(eventManager);

    // Create and Initialize the ComponentManager.
    componentManager = new PT::Component::ComponentManager (&pointerlib);
    if (!componentManager->Initialize())
      return Report(PT::Error, "Failed to initialize ComponentManager!");
    pointerlib.setComponentManager(componentManager);

    // Create and Initialize the UserManager.
    userManager = new PT::User::UserManager ();
    if (!userManager->Initialize())
      return Report(PT::Error, "Failed to initialize UserManager!");
    pointerlib.setUserManager(userManager);

    // Create and Initialize the SectorDataManager.
    sectorDataManager = new PT::Data::SectorDataManager (&pointerlib);
    if (!sectorDataManager->parse())
      return Report(PT::Error, "Failed to initialize SectorManager!");
    pointerlib.SetSectorDataManager(sectorDataManager);

    // Create and Initialize the ConnectionDataManager.
    connectionDataManager = new PT::Data::ConnectionDataManager (&pointerlib);
    if (!connectionDataManager->parse())
      return Report(PT::Error, "Failed to initialize ConnectionDataManager!");
    pointerlib.setConnectionDataManager(connectionDataManager);

    // Create the ServerSetupManager.
    serverSetupManager = new PT::Misc::ServerSetupManager ();
    pointerlib.setServerSetupManager(serverSetupManager);

    // Create and Initialize the GUIManager.
    guiManager = new PT::GUI::GUIManager ();
    if (!guiManager->Initialize ())
      return Report(PT::Error, "Failed to initialize GUIManager!");
    pointerlib.setGUIManager(guiManager);

    // Create and Initialize the InputManager.
    inputManager = new PT::Input::InputManager();
    if (!inputManager->Initialize())
      return Report(PT::Error, "Failed to create InputManager object!");

    // Create and Initialize the StateManager.
    stateManager = new PT::StateManager();
    if (!stateManager->Initialize())
      return Report(PT::Error, "Failed to create StateManager object!");
    pointerlib.setStateManager(stateManager);

    if (!RegisterQueue(GetObjectRegistry(), csevAllEvents(GetObjectRegistry())))
      return Report(PT::Error, "Failed to set up event handler!");

    engine = csQueryRegistry<iEngine> (GetObjectRegistry());
    if (!engine) return Report(PT::Error, "Failed to locate 3D engine!");

    vc = csQueryRegistry<iVirtualClock> (GetObjectRegistry());
    if (!vc) return Report(PT::Error, "Failed to locate Virtual Clock!");

    limitFPS = app_cfg->GetInt("Peragro.Video.MaxFPS", limitFPS);

    InitializeCEL();

    // Create the cursor.
    cursor = new Cursor(&pointerlib);
    if (!cursor) return Report(PT::Error, "Can't create cursor!");
    pointerlib.setCursor(cursor);

    // Create and Initialize the EntityManager.
    entityManager = new PT::Entity::EntityManager ();
    if (!entityManager->Initialize())
      return Report(PT::Error, "Failed to initialize EntityManager!");
    pointerlib.setEntityManager(entityManager);

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
    pointerlib.setTradeManager(tradeManager);

    // Create and Initialize the QuestManager.
    questManager = new PT::Quest::QuestManager ();
    if (!questManager->Initialize())
      return Report(PT::Error, "Failed to initialize QuestManager!");
    //pointerlib.setQuestManager(questManager);

    view.AttachNew(new csView(engine, g3d));
    iGraphics2D* g2d = g3d->GetDriver2D();
    view->GetCamera()->SetViewportSize(g2d->GetWidth(), g2d->GetHeight());

    // We use some other "helper" event handlers to handle
    // pushing our work into the 3D engine and rendering it
    // to the screen.
    //drawer.AttachNew(new FrameBegin3DDraw (GetObjectRegistry (), view));
    printer.AttachNew(new FramePrinter (GetObjectRegistry ()));

    csRef<iSoundManager> soundMananger = csLoadPlugin<iSoundManager> (plugin_mgr, "peragro.sound");
    if (soundMananger.IsValid())
    {
      object_reg->Register (soundMananger, "iSoundManager");
      soundMananger->LoadSoundEvents("/peragro/xml/sounds/sounds.xml");
      soundMananger->PlayAmbient("/peragro/art/audio/music/intro/peragrotempus.ogg");
    }
    else
        Report(PT::Error, "Failed to load the iSoundManager!");

    csRef<iEffectsManager> effectsMananger = csLoadPlugin<iEffectsManager> (plugin_mgr, "peragro.effectsmanager");
    if (effectsMananger.IsValid())
    {
      object_reg->Register (effectsMananger, "iEffectsManager");
      effectsMananger->LoadEffectTemplates("/peragro/xml/effects/effects.xml");
    }
    else
        Report(PT::Error, "Failed to load the iEffectsManager!");

    //Listen for events.
    PT_SETUP_HANDLER

    // Register listener for connection.
    PT_REGISTER_LISTENER(Client, SawServer, "connection.sawServer")

    // Register listener for state.play.
    PT_REGISTER_LISTENER(Client, PlayingEvent, "state.play")

    // Register listener for WorldLoadingEvent.
    PT_REGISTER_LISTENER(Client, LoadingRegion, "world.loading")

    // Register listener for WorldLoadedEvent.
    PT_REGISTER_LISTENER(Client, LoadRegion, "world.loaded")

    //Actions

    // Register listener for ActionQuit.
    PT_REGISTER_LISTENER(Client, ActionQuit, "input.Quit")

    // Register listener for Quit
    PT_REGISTER_LISTENER(Client, UserQuit, "user.quit")

    PT_REGISTER_LISTENER(Client, ActionFat, "input.Fat")

    // Create the world.
    csRef<iWorld> world = csQueryRegistry<iWorld>(object_reg);
    if (world.IsValid())
    {
      world->Initialize("MyWorld");
      object_reg->Register (world, "iWorld");
    }
    else
      Report(PT::Error, "Failed to load the iWorld!");

    // Create and Initialize the EnvironmentManager.
    environmentManager = new PT::Environment::EnvironmentManager();
    if (!environmentManager->Initialize())
      return Report(PT::Error, "Failed to create EnvironmentManager object!");
    pointerlib.setEnvironmentManager(environmentManager);

    // Register the PointerPlug pseudo-plugin
    csRef<PointerPlug> ptrplug = new PointerPlug(NULL);
    if (ptrplug)
      object_reg->Register(ptrplug, PT_POINTERLIBRARY_PLUGNAME);

    // Let the engine prepare all lightmaps for use and also free all images
    // that were loaded for the texture manager.
    engine->Prepare ();

    //TODO: this must be done nicer somehow.
    view->GetCamera()->SetSector (engine->FindSector("CSIconRenderer"));

    Run();

    return true;
  }

  void Client::handleStates()
  {
    switch(stateManager->GetState())
    {
    case STATE_INITIAL: // Initial state. Load intro sector and go to STATE_INTRO.
      {
        using namespace PT::GUI::Windows;

        // Load introduction sector, draw it once for this frame, and switch to STATE_INTRO
        const char* path = cmdline->GetOption("intro");

        if (!path)
        {
          path = 0;
          BackgroundWindow* bg = guiManager->GetWindow<BackgroundWindow>(BACKGROUNDWINDOW);
          bg->ShowWindow();
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
        LoginWindow* loginWindow = guiManager->GetWindow<LoginWindow>(LOGINWINDOW);
        loginWindow->ShowWindow();
        ServerWindow* serverWindow = guiManager->GetWindow<ServerWindow>(SERVERWINDOW);
        serverWindow->ShowWindow ();

        if (cmdline)
        {
          const char* host = cmdline->GetOption("host");
          const char* port = cmdline->GetOption("port");
          if (host)
          {
            if (!port){port = "12345";}
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
      {
        world_loaded = false;
        entityManager->SetWorldloaded(false);
      }
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
        boost::shared_ptr<PT::Entity::PlayerEntity> player = Entity::PlayerEntity::Instance();
        if (player)
        {
          // Draw the player camera manually.
          player->CameraDraw();
        }
        break;
      }
    case STATE_RECONNECTED:
      {
        checkConnection();
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
      entityManager->RemoveAll();

      stateManager->SetState(STATE_RECONNECTED);

      network->init();

      PT::GUI::Windows::OkDialogWindow* dialog = new PT::GUI::Windows::OkDialogWindow(guiManager);

      if (!network->isRunning())
      {
        dialog->ShowWindow();
        dialog->BringToFront();
        dialog->SetText("Disconnect!\n Trying to reconnect, please wait!");
        return;
      }

      dialog->HideWindow();

      ConnectRequestMessage msg(CLIENTVERSION);
      network->send(&msg);
    }
  }

  bool Client::ActionQuit(iEvent& ev)
  {
    using namespace PT::Events;

    if (stateManager->GetState() == STATE_PLAY)
    {
      PT::GUI::Windows::ConfirmDialogWindow* dialog = new PT::GUI::Windows::ConfirmDialogWindow(guiManager);
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

  bool Client::UserQuit(iEvent& ev)
  {
    Quit();
    return true;
  }

  bool Client::Quit(const CEGUI::EventArgs &args)
  {
    Quit();
    return true;
  }

  void Client::Quit()
  {
    csRef<iEventQueue> q = csQueryRegistry<iEventQueue> (GetObjectRegistry());
    if (q.IsValid()) q->GetEventOutlet()->Broadcast(csevQuit(GetObjectRegistry()));
  }

  bool Client::NoQuit(const CEGUI::EventArgs &args)
  {
    return true;
  }

  bool Client::ActionFat(iEvent& ev)
  {
    /*
    static float weight = 0.0f;

    weight += 0.1f;
    if (weight > 1.0f) weight = 0.0f;

    boost::shared_ptr<PT::Entity::PlayerEntity> player = Entity::PlayerEntity::Instance();

    csRef<iPcMesh> pcmesh = CEL_QUERY_PROPCLASS_ENT(player->GetCelEntity(), iPcMesh);

    csRef<iAnimatedMesh> animesh = scfQueryInterface<iAnimatedMesh> (pcmesh->GetMesh()->GetMeshObject());

    csRef<iAnimatedMeshFactory> animeshf = scfQueryInterface<iAnimatedMeshFactory> (pcmesh->GetMesh()->GetFactory()->GetMeshObjectFactory());

    uint target = animeshf->FindMorphTarget("Fat");

    animesh->SetMorphTargetWeight(target, weight);
    */

    boost::shared_ptr<PT::Entity::PlayerEntity> player = Entity::PlayerEntity::Instance();
    csRef<iPcMesh> pcmesh = CEL_QUERY_PROPCLASS_ENT(player->GetCelEntity(), iPcMesh);
    csRef<CS::Mesh::iAnimatedMesh> animesh = scfQueryInterface<CS::Mesh::iAnimatedMesh> (pcmesh->GetMesh()->GetMeshObject());
    csRef<CS::Mesh::iAnimatedMeshFactory> animeshf = scfQueryInterface<CS::Mesh::iAnimatedMeshFactory> (pcmesh->GetMesh()->GetFactory()->GetMeshObjectFactory());

    csRef<CS::Animation::iSkeletonFactory> skeletonf = animeshf->GetSkeletonFactory();
    csRef<CS::Animation::iSkeleton> skeleton = animesh->GetSkeleton();

    csQuaternion rot; 
    csVector3 offset;

    CS::Animation::BoneID spine = skeletonf->FindBone("Leg_Lower.L");
    skeleton->GetTransformBoneSpace(spine, rot, offset);
    offset = offset + csVector3(0,0.5,0);
    rot.SetEulerAngles(csVector3(1.45,0,0));
    skeleton->SetTransformBoneSpace(spine, rot, offset);

    printf("BLAH: FAT\n");

    return true;
  }

  bool Client::InitializeCEL()
  {
    iObjectRegistry* object_reg = this->GetObjectRegistry();

    pl = csQueryRegistry<iCelPlLayer> (object_reg);
    if (!pl) return Report(PT::Error, "Failed to load CEL Physical Layer!");

    return true;
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
    using namespace PT::GUI::Windows;

    LoadScreenWindow* loadScreenWindow = guiManager->GetWindow<LoadScreenWindow>(LOADSCREENWINDOW);
    if (!loadScreenWindow->IsVisible())
      loadScreenWindow->ShowWindow();
    loadScreenWindow->SetProgress(WorldHelper::GetProgress(&ev));
    return true;
  }

  bool Client::LoadRegion(iEvent& ev)
  {
    using namespace PT::Events;
    using namespace PT::GUI::Windows;

    iCelEntity* ent = pl->FindEntity("ptIntroWorld");
    if (ent)
    {
      csRef<iPcRegion> pcreg = CEL_QUERY_PROPCLASS_ENT(ent, iPcRegion);
      pcreg->Unload();
      pl->RemoveEntity(ent);
    }

    stateManager->SetState(STATE_PLAY);

    SawServer(ev);

    if (!world_loaded)
    {
      SelectCharWindow* selectCharWindow = guiManager->GetWindow<SelectCharWindow>(SELECTCHARWINDOW);
      selectCharWindow->HideWindow();
      OptionsWindow* optionsWindow = guiManager->GetWindow<OptionsWindow>(OPTIONSWINDOW);
      optionsWindow->HideWindow();

      ChatWindow* chatWindow = guiManager->GetWindow<ChatWindow>(CHATWINDOW);
      chatWindow->ShowWindow();
      StatsHUDWindow* statshudWindow = guiManager->GetWindow<StatsHUDWindow>(STATSHUDWINDOW);
      statshudWindow->ShowWindow();
      SkillsHUDWindow* skillshudWindow = guiManager->GetWindow<SkillsHUDWindow>(SKILLSHUDWINDOW);
      skillshudWindow->ShowWindow();

      // Hide the background.
      BackgroundWindow* bg = guiManager->GetWindow<BackgroundWindow>(BACKGROUNDWINDOW);
      bg->HideWindow();

      // Little hack to restore focus.
      guiManager->GetCEGUI()->GetWindowManagerPtr ()->getWindow("Chatlog/Frame")->activate();

      // Stop the intro music.
      csRef<iSoundManager> soundMananger = csQueryRegistry<iSoundManager> (object_reg);
      if (soundMananger.IsValid())
        soundMananger->StopAmbient();
    }

    LoadScreenWindow* loadScreenWindow = guiManager->GetWindow<LoadScreenWindow>(LOADSCREENWINDOW);
    loadScreenWindow->HideWindow();

    world_loaded = true;
    PointerLibrary::getInstance()->getEntityManager()->SetWorldloaded(true);

    return true;
  }

  bool Client::SawServer(iEvent& ev)
  {
    last_seen = csGetTicks();
    return true;
  }

} // PT namespace

/*
void pvFunc(void)
{
  printf("AAARRRRRGGGG");
}
*/

/*---------------*
* Main function
*---------------*/
int main (int argc, char* argv[])
{
  //_set_purecall_handler(pvFunc);

#ifdef __TIMESTAMP__
  printf("Peragro Tempus - Client\n$Revision$\tBuild-Date: %s\n", __TIMESTAMP__);
#else
  printf("Peragro Tempus - Client\n$Revision$\tBuild-Date: %s %s\n", __DATE__, __TIME__);
#endif
  //setWinCrashDump(argv[0]);

#ifdef CS_STATIC_LINKED
  //Overwrite existing env-vars for static linked client
  CS::Utility::setenv("CRYSTAL", ".", true);
  CS::Utility::setenv("CEL", ".", true);
#endif

  return csApplicationRunner<PT::Client>::Run (argc, argv);
}
