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
#include "iengine/engine.h"

#include "physicallayer/pl.h"

#include "client/entity/ptCelEntity.h"

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
  csRef<iLoader> loader;
  csRef<iCelPlLayer> pl;

  struct Movement
  {
    int entity_id;
    float walk;
    float turn;
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
  csPDelArray<DrUpdate> drupdate_entity_name;

  Mutex mutex;

  int own_char_id;
  csRef<iCamera> owncam;
  csRef<iCelEntity> ownent;
  const char* ownname;

  bool playing;
  bool world_loaded;

  void createCelEntity(Entity* ent);

  void addEntity();
  void delEntity();
  void moveEntity();
  void DrUpdateEntity();

public:

  ptEntityManager (iObjectRegistry* obj_reg);
  ~ptEntityManager ();

  bool Initialize (iObjectRegistry* obj_reg);
  void Handle();

  void addEntity(Entity* name);
  void delEntity(Entity* name);
  void moveEntity(int entity_id, float walk, float turn);
  void moveEntity(int entity_id, float speed, float* ori, float* dst);
  void DrUpdateEntity(ptEntityManager::DrUpdate* drupdate);
  UpdateDREntityRequestMessage DrUpdateOwnEntity();

  iCelEntity* findCelEntById(int id);

  void setCharacter(int own_char) { own_char_id = own_char; }
  iCamera* getOwnCamera() { return owncam; } 
  iCelEntity* getOwnEntity() { return ownent; }
  const char* GetOwnName() { return ownname; }

  void setPlaying(bool value) { playing = value; }
  void setWorldloaded(bool value) { world_loaded = value; }

};

#endif // PTENTITYMANAGER_H
