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
class Cursor;
struct iCommandLineParser;

class Client : public csApplicationFramework, public csBaseEventHandler
{
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

  csRef<iCelPlLayer> pl;
  csRef<iCelBlLayer> bl;
  csRef<iCelEntity> zonemanager;

  void ProcessFrame();
  void FinishFrame();

  bool OnKeyboard(iEvent&);
  bool OnMouseMove(iEvent&);
  void CreateRoom(); 

  Network* network;
  GUIManager* guimanager;
  EffectsManager* effectsmanager;

  bool InitializeCEL();

  csString name;
  int own_char_id;
  const char* own_char_name;

  csPDelArray<Entity> new_entity_name;
  csPDelArray<Entity> del_entity_name;

  csRefArray<iString> chat_msg;

  struct Movement
  {
    int entity_id;
    float walk;
    float turn;
  };
  csPDelArray<Movement> move_entity_name;

  Cursor* cursor;

public:
  struct DrUpdate
  {
    int entity_id;
    csString sector;
    csVector3 pos, vel, wvel;
    float rot, speed, avel;
    bool on_ground;
  };
private:
  csPDelArray<DrUpdate> drupdate_entity_name;

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

  void connected();
  void loggedIn();
  void loadRegion(const char* name);
  void loadRegion();

  void addEntity(Entity* name);
  void addEntity();

  void delEntity(Entity* name);
  void delEntity();

  void moveEntity(int entity_id, float walk, float turn);
  void moveEntity();

  void DrUpdateEntity(DrUpdate* drupdate);
  void DrUpdateEntity();

  void chat(char type, const char* msg);
  void chat();

  void setCharacter(int own_char) { own_char_id = own_char; }

  const char* GetOwnCharName() { return own_char_name; }

  GUIManager* GetGuiManager ()
  {
    return guimanager;
  }

  iEngine* getCsEngine()  { return engine; }
  iGraphics3D* getG3D()  { return g3d; }
  iCamera* getCamera();

  iPcDefaultCamera* getPcCamera();
  iPcActorMove* getPcActorMove();
  iCelEntity* getPlayerEntity();
  
  iVFS* getVFS()  { return vfs; }
  Network* getNetwork()  { return network; }
};

#endif // CLIENT_H
