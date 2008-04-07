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

#ifndef CLIENT_H
#define CLIENT_H

#include <cssysdef.h>
#include <csutil/ref.h>
#include <csutil/refarr.h>
#include <csutil/parray.h>
#include <csutil/csbaseeventh.h>
#include <cstool/csapplicationframework.h>
#include <iengine/engine.h>
#include <iutil/vfs.h>
#include <iutil/pluginconfig.h>
#include <iutil/virtclk.h>
#include <ivideo/graph3d.h>

#include <isndsys/ss_renderer.h>
#include <isndsys/ss_loader.h>
#include <isndsys/ss_source.h>
#include <isndsys/ss_stream.h>
#include <isndsys/ss_data.h>
#include <isndsys/ss_listener.h>

#include <behaviourlayer/bl.h>
#include <physicallayer/pl.h>
#include <physicallayer/entity.h>

#include "common/network/netmessage.h"
#include "common/util/mutex.h"

#include "client/event/eventmanager.h"
#include "client/event/regionevent.h"
#include "client/event/inputevent.h"

#include "client/pointer/pointer.h"

#include "client/input/inputmanager.h"
#include "client/state/statemanager.h"

#include "CEGUI.h"
#include "CEGUIWindowManager.h"
#include "CEGUILogger.h"

struct iPcDefaultCamera;
struct iPcActorMove;
struct iSector;

struct iCommandLineParser;
struct iPath;
struct iView;

class Network;
class GUIManager;
class CombatManager;
class Cursor;

namespace PT
{
  class Reporter;
  class World;

  namespace Events
  {
    class EventManager;
  } // Events namespace

  namespace Data
  {
    class EffectDataManager;
    class SectorDataManager;
    class SkillDataManager;
    class ConnectionDataManager;
  } // Data namespace

  namespace Misc
  {
    class ServerSetupManager;
  } // Misc namespace

  namespace Reflection
  {
    class ReflectionRenderer;
  } // Reflection namespace

  namespace Entity
  {
    class EntityManager;
    class StatManager;
  } // Entity namespace

  namespace Effect
  {
    class EffectsManager;
  } // Effect namespace

  namespace Combat
  {
    class CombatManager;
  } // Combat namespace

  namespace Chat
  {
    class ChatManager;
  } // Chat namespace

  namespace Trade
  {
    class TradeManager;
  } // Trade namespace

  class Client : public csApplicationFramework, public csBaseEventHandler
  {
  public:
    Client();
    ~Client();

    void OnExit();
    bool OnInitialize(int argc, char* argv[]);

    bool Application();
    void OnCommandLineHelp();

    void login(const std::string& user, const std::string& pass);

    void selectCharacter(unsigned int char_id);

    void sawServer();

    csTicks GetTicks() { return csGetTicks(); }
    void DrawFrame ()
    {
      PreProcessFrame();
      ProcessFrame();
      PostProcessFrame();
      FinishFrame();
    }

    bool loggedIn(iEvent& ev);
    bool PlayingEvent(iEvent& ev);
    bool LoadingRegion(iEvent& ev);
    bool LoadRegion(iEvent& ev);
    bool Connected(iEvent& ev);

  private:
    void PreProcessFrame();
    void ProcessFrame();
    void FinishFrame();

    iPcActorMove* getPcActorMove();
    bool InitializeCEL();
    void checkConnection();
    void handleStates();

    bool ActionActivateSkill(iEvent& ev);
    bool ActionQuit(iEvent& ev);
    bool Quit(const CEGUI::EventArgs &args);
    bool NoQuit(const CEGUI::EventArgs &args);

  private:
    PointerLibrary pointerlib;

    csRef<iEngine> engine;
    csRef<iGraphics3D> g3d;
    csRef<iVirtualClock> vc;
    csRef<iVFS> vfs;
    csRef<iCommandLineParser> cmdline;
    csRef<iView> view;
    csRef<iCelPlLayer> pl;
    csRef<iCelBlLayer> bl;

    ///The sound renderer.
    csRef<iSndSysRenderer> sndrenderer;

    ///The sound loader.
    csRef<iSndSysLoader> sndloader;

    ///The sound stream.
    csRef<iSndSysStream> sndstream;

    ///The sound source.
    csRef<iSndSysSource> sndsource;
    csRef<iSndSysSource3D> sndsource3d;

    ///The config manager.
    csRef<iConfigManager> app_cfg;

    Reporter* reporter;
    Network* network;
    Cursor* cursor;

    PT::Events::EventManager* eventManager;
    PT::Data::EffectDataManager* effectDataManager;
    PT::Data::SectorDataManager* sectorDataManager;
    PT::Data::SkillDataManager* skillDataManager;
    PT::Data::ConnectionDataManager* connectionDataManager;
    PT::Misc::ServerSetupManager* serverSetupManager;
    GUIManager* guiManager;
    PT::InputManager* inputManager;
    PT::StateManager* stateManager;
    PT::EnvironmentManager* environmentManager;
    PT::Reflection::ReflectionRenderer* reflectionRenderer;

    PT::Entity::EntityManager* entityManager;
    PT::Entity::StatManager* statManager;
    PT::Effect::EffectsManager* effectsManager;
    PT::Combat::CombatManager* combatManager;
    PT::Chat::ChatManager* chatManager;
    PT::Trade::TradeManager* tradeManager;

    World* world;

    Mutex mutex;
    csTicks timer;
    iSector* room;
    bool world_loaded;
    int limitFPS;
    csTicks last_sleep;
    csTicks last_seen;

    // needed for relogin on disconnect
    std::string user;
    std::string pass;
    unsigned int char_id;
  };

} // PT namespace

#endif // CLIENT_H
