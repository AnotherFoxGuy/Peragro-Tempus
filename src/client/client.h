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
#include "client/event/environmentevent.h"
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
class CombatMGR;
class Cursor;

namespace PT
{
  class World;

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

  namespace Effect
  {
    class EffectsManager;
  } // Effect namespace

  namespace Entity
  {
    class EntityManager;
    class StatManager;
  } // Entity namespace

  namespace Events
  {
    class EventManager;
  } // Events namespace

  namespace Chat
  {
    class ChatManager;
  } // Chat namespace

  namespace Trade
  {
    class TradeManager;
  } // Trade namespace

  class Reporter;

  class Client : public csApplicationFramework, public csBaseEventHandler
  {
  private:
    void PreProcessFrame();
    void ProcessFrame();
    void FinishFrame();

    bool OnMouseMove(iEvent&);
    iPcActorMove* getPcActorMove();
    bool InitializeCEL();
    void checkConnection();
    void handleStates();

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

    // The sound renderer.
    csRef<iSndSysRenderer> sndrenderer;

    // The sound loader.
    csRef<iSndSysLoader> sndloader;

    // The sound stream.
    csRef<iSndSysStream> sndstream;

    // The sound source.
    csRef<iSndSysSource> sndsource;
    csRef<iSndSysSource3D> sndsource3d;

    // The config manager.
    csRef<iConfigManager> app_cfg;

    Reporter* reporter;
    Network* network;
    GUIManager* guimanager;

    PT::Entity::EntityManager* entitymanager;
    PT::Entity::StatManager* statmanager;
    CombatMGR* combatmanager;

    PT::Data::EffectDataManager* effectDataManager;
    PT::Data::SectorDataManager* sectorDataManager;
    PT::Data::SkillDataManager* skillDataManager;
    PT::Data::ConnectionDataManager* connectionDataManager;
    PT::Misc::ServerSetupManager* serverSetupManager;
    PT::Effect::EffectsManager* effectsmanager;
    PT::StateManager* statemanager;

    Cursor* cursor;
    PT::InputManager *inputMgr;

    PT::Events::EventManager* eventmanager;
    PT::Chat::ChatManager* chatmanager;
    PT::Trade::TradeManager* trademanager;

    World* world;

    bool ActionActivateSkill(PT::Events::Eventp);
    bool ActionQuit(PT::Events::Eventp);
    bool Quit(const CEGUI::EventArgs &args);
    bool NoQuit(const CEGUI::EventArgs &args);

  private:
    Mutex mutex;
    csTicks timer;
    iSector *room;
    bool world_loaded;
    bool enable_reflections;
    int limitFPS;
    csTicks last_sleep;
    csTicks last_seen;

    // needed for relogin on disconnect
    std::string user;
    std::string pass;
    unsigned int char_id;

  public:
    bool loggedIn(PT::Events::Eventp ev);
    bool PlayingEvent(PT::Events::Eventp ev);
    bool LoadingRegion(PT::Events::Eventp ev);
    bool LoadRegion(PT::Events::Eventp ev);
    bool Connected(PT::Events::Eventp ev);
    bool SetDayTime(PT::Events::Eventp ev);

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
  };

} // PT namespace

#endif // CLIENT_H
