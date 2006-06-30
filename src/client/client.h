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

#include "cssysdef.h"
#include "csutil/ref.h"
#include "csutil/refarr.h"
#include "csutil/parray.h"
#include "csutil/csbaseeventh.h"
#include "cstool/csapplicationframework.h"
#include "iengine/engine.h"
#include "iutil/vfs.h"
#include "iutil/pluginconfig.h"
#include "iutil/virtclk.h"
//#include "iutil/csinput.h"
#include "ivideo/graph3d.h"

#include "behaviourlayer/bl.h"
#include "physicallayer/pl.h"
#include "physicallayer/entity.h"
#include "propclass/mesh.h"

#include "common/network/netmessage.h"
#include "common/util/mutex.h"

struct iPcDefaultCamera;
struct iPcActorMove;
struct iSector;
class Entity;
class Network;
class GUIManager;
class EffectsManager;
class ptEntityManager;
class CombatMGR;
class Cursor;
struct iCommandLineParser;
struct iPath;
struct iView;

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
    STATE_PLAY = 5
  };

  ///Game internal state
  eSTATE state;

private:
  iSector *room;
  float rotX, rotY;

  bool world_loaded;

  int limitFPS;
  csTicks last_sleep;

  csRef<iEngine> engine;
  csRef<iGraphics3D> g3d;
  csRef<iVirtualClock> vc;
  //csRef<iKeyboardDriver> kbd;
  csRef<iVFS> vfs;
  csRef<iCommandLineParser> cmdline;
  csRef<iView> view;

  csRef<iCelPlLayer> pl;
  csRef<iCelBlLayer> bl;
  csRef<iCelEntity> zonemanager;

  void PreProcessFrame();
  void ProcessFrame();
  void PostProcessFrame();
  void FinishFrame();

  bool OnKeyboard(iEvent&);
  bool OnMouseMove(iEvent&);
  iPcActorMove* getPcActorMove();

  Network* network;
  GUIManager* guimanager;
  EffectsManager* effectsmanager;
  ptEntityManager* entitymanager;
  CombatMGR* combatmanager;

  bool InitializeCEL();

  csRefArray<iString> chat_msg;

  Cursor* cursor;

  void handleStates();

private:

  csRef<iString> load_region;

  bool playing;

  char walk, turn;

  Mutex mutex;

  csTicks timer;

public:
  Client();
  ~Client();

  void OnExit();
  bool OnInitialize(int argc, char* argv[]);

  bool Application();
  void OnCommandLineHelp();

  void connected();
  void loggedIn();
  void loadRegion(const char* name);
  void loadRegion();

  void chat(char type, const char* msg);
  void chat();

  GUIManager* GetGuiManager ()  {return guimanager;}

  ptEntityManager* GetEntityManager ()  {return entitymanager;}

  iEngine* getCsEngine()  { return engine; }
  iGraphics3D* getG3D()  { return g3d; }
  iVFS* getVFS()  { return vfs; }
  Network* getNetwork()  { return network; }
  EffectsManager* getEffectsmgr() { return effectsmanager; }
  GUIManager* getGuimgr() { return guimanager;; }
};

#endif // CLIENT_H
