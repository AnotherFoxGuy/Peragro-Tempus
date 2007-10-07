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
class PtConsole;
class Cursor;

namespace PT
{
  namespace Data
  {
    class DoorManager;
    class ItemManager;
    class EffectsManager;
    class SectorManager;
  } // Data namespace 

  namespace Entity
  {
    class EntityManager;
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
  public:
    /// Game states
    enum eSTATE
    {
      STATE_INITIAL = 0,
      STATE_INTRO = 1,
      STATE_CONNECTED = 2,
      STATE_LOGGED_IN = 3,
      STATE_SELECTING_CHAR = 4,
      STATE_PLAY = 5,
      STATE_RECONNECTED = 6
    };

    ///Game internal state
    eSTATE state;

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
    csRef<iCelEntity> zonemanager;

    // The sound renderer.
    csRef<iSndSysRenderer> sndrenderer;

    // The sound loader.
    csRef<iSndSysLoader> sndloader;

    // The sound stream.
    csRef<iSndSysStream> sndstream;

    // The sound source.
    csRef<iSndSysSource> sndsource;
    csRef<iSndSysSourceSoftware3D> sndsource3d;

    // The config manager.
    csRef<iConfigManager> app_cfg;

    Reporter* reporter;
    Network* network;
    GUIManager* guimanager;
    
    PT::Entity::EntityManager* entitymanager;
    CombatMGR* combatmanager;

    PT::Data::DoorManager* doormanager;
    PT::Data::ItemManager* itemmanager;
    PT::Data::SectorManager* sectormanager;
    PT::Data::EffectsManager* effectsmanager;

    Cursor* cursor;
    PtConsole* ptconsole;
    PT::InputManager *inputMgr;

    PT::Events::EventManager* eventmanager;
    PT::Chat::ChatManager* chatmanager;
    PT::Trade::TradeManager* trademanager;

    bool ActionForward(PT::Events::Eventp);
    bool ActionBackward(PT::Events::Eventp);
    bool ActionLeft(PT::Events::Eventp);
    bool ActionRight(PT::Events::Eventp);
    bool ActionToggleWalk(PT::Events::Eventp);
    bool ActionToggleRun(PT::Events::Eventp);
    bool ActionPanUp(PT::Events::Eventp);
    bool ActionPanDown(PT::Events::Eventp);
    bool ActionToggleCamera(PT::Events::Eventp);
    bool ActionToggleDistClipping(PT::Events::Eventp);
    bool ActionHit(PT::Events::Eventp);
    bool ActionActivateSkill(PT::Events::Eventp);
    bool ActionActivateWeapon(PT::Events::Eventp);
    bool ActionQuit(PT::Events::Eventp);
    bool DoAction();
    bool ActionMoveTo(PT::Events::Eventp);
    bool ActionOnInteract(PT::Events::Eventp ev);
    bool ActionZoomIn(PT::Events::Eventp ev);
    bool ActionZoomOut(PT::Events::Eventp ev);
    bool Quit(const CEGUI::EventArgs &args);
    bool NoQuit(const CEGUI::EventArgs &args);

  private:
    bool playing;
    char walk, turn;
    bool run;
    Mutex mutex;
    csTicks timer;
    iSector *room;
    float rotX, rotY;
    float cameradistance;
    bool world_loaded;
    int limitFPS;
    csTicks last_sleep;
    csTicks last_seen;

    // needed for relogin on disconnect
    csString user;
    csString pass;
    unsigned int char_id;

  public:
    bool loggedIn(PT::Events::Eventp ev);
    bool LoadRegion(PT::Events::Eventp ev);
    bool Connected(PT::Events::Eventp ev);

  public:
    Client();
    ~Client();

    void OnExit();
    bool OnInitialize(int argc, char* argv[]);

    bool Application();
    void OnCommandLineHelp();

    void login(csString user, csString pass);

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
