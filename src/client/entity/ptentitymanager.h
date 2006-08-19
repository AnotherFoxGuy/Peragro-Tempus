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

#ifndef PTENTITYMANAGER_H
#define PTENTITYMANAGER_H

#include "cssysdef.h"
#include "csutil/ref.h"
#include "iutil/vfs.h"
#include "iutil/virtclk.h"
#include "iengine/engine.h"
#include "csgeom/path.h"

#include "physicallayer/pl.h"
#include "physicallayer/datatype.h"

#include "propclass/quest.h"

#include "client/entity/ptcelentity.h"

#include "common/util/mutex.h"

#include "client/client.h"

#include "client/network/network.h"

class Effect;

struct iObjectRegistry;
struct iLoader;

class ptEntityManager
{
private:

  csArray<ptCelEntity> entities;

  csRef<iEngine> engine;
  csRef<iVFS> vfs;
  csRef<iStringSet> stringset;
  csRef<iVirtualClock> vc;
  csRef<iLoader> loader;
  csRef<iCelPlLayer> pl;
    
  Client* client;

  struct Movement
  {
    int entity_id;
    float walk;
    float turn;
  };

  struct MoveTo
  {
    int entity_id;
    bool walking;
    float dest_angle;
    float walk_speed;
    float turn_speed;
    float elapsed_time;
    float walk_duration;
  };

  struct UpdatePcProp
  {
    int entity_id;
    csString pcprop;
    celData value;
  };

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
  csPDelArray<Entity> new_entity_name;
  csPDelArray<Entity> del_entity_name;
  csPDelArray<Movement> move_entity_name;
  csPDelArray<MoveTo> move_to_entity_name;
  csPDelArray<DrUpdate> drupdate_entity_name;
  csPDelArray<UpdatePcProp> update_pcprop_entity_name;

  Mutex mutex;

  unsigned int own_char_id;
  csRef<iCamera> owncam;
  csRef<iCelEntity> ownent;
  const char* ownname;

  bool playing;
  bool world_loaded;

  void addEntity();
  void delEntity();
  void moveEntity();
  void moveToEntity();
  void DrUpdateEntity();
  void updatePcProp();

  float GetAngle (const csVector3& v1, const csVector3& v2);

public:

  ptEntityManager (iObjectRegistry* obj_reg, Client* client);
  ~ptEntityManager ();

  bool Initialize (iObjectRegistry* obj_reg);
  void Handle();

  void addEntity(Entity* name);
  void delEntity(Entity* name);
  void moveEntity(int entity_id, float walk, float turn);
  void moveEntity(int entity_id, float walk_speed, float* ori, float* dst);
  void teleport(int entity_id, float* pos, const char* sector);
  void DrUpdateEntity(ptEntityManager::DrUpdate* drupdate);
  UpdateDREntityRequestMessage DrUpdateOwnEntity();
  void Attach(uint entityid, const char* socketName, const char* meshFactName );
  void updatePcProp(int entity_id, const char *pcprop,celData &value);

  void createCelEntity(Entity* ent);
  void SetMaskColor(iMeshWrapper* mesh, const char* maskname, csVector4 colorvalue);
  void SetMaskColor(iMeshWrapper* mesh, const char* maskname, unsigned int colorvalue);

  iCelEntity* findCelEntById(int id);
  ptCelEntity* findPtEntById(int id);

  void setCharacter(unsigned int own_char) { own_char_id = own_char; }
  iCamera* getOwnCamera() { return owncam; }
  iCelEntity* getOwnEntity() { return ownent; }
  unsigned int GetOwnId() { return own_char_id; }
  const char* GetOwnName() { return ownname; }

  void setPlaying(bool value) { playing = value; }
  void setWorldloaded(bool value) { world_loaded = value; }

};

#endif // PTENTITYMANAGER_H
