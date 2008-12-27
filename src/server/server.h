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

#ifndef SERVER_H
#define SERVER_H

#include "common/geom/ptvector3.h"

class CharacterManager;
class Tables;
class Database;
class EntityManager;
class RaceManager;
class ItemManager;
class StatManager;
class DoorManager;
class SkillManager;
class Network;
class UserManager;
class UserAccountManager;
class TimerEngine;
class Entity;
class PcEntity;
class NpcEntity;
class CharacterEntity;
class Spawner;
class SkillEngine;
class ByteStream;
class SectorManager;
class MountEntity;
class CollisionDetection;
class ZoneManager;
class EnvironmentManager;
class ReputationManager;
class CombatManager;
class MeshManager;

class Server
{
private:
  static Server* server;

  CharacterManager* char_mgr;
  Tables* tables;
  Database* db;
  Network* network;
  EntityManager* ent_mgr;
  RaceManager* race_mgr;
  ItemManager* item_mgr;
  StatManager* stat_mgr;
  DoorManager* door_mgr;
  SkillManager* skill_mgr;
  UserManager* usr_mgr;
  UserAccountManager* usr_acc_mgr;
  TimerEngine* timer_engine;
  Spawner* spawner;
  SkillEngine* skillengine;
  SectorManager* sectormanager;
  CollisionDetection* colldet;
  ZoneManager* zone_mgr;
  EnvironmentManager* environment_mgr;
  ReputationManager* reputation_mgr;
  CombatManager* combatMgr;
  MeshManager* meshMgr;

public:
  Server() { server = this; }
  static Server* getServer() { return server; }

  void setCombatManager(CombatManager* combatMgr) { this->combatMgr = combatMgr; }
  CombatManager* getCombatManager() { return this->combatMgr; }

  void setMeshManager(MeshManager* meshMgr) { this->meshMgr = meshMgr; }
  MeshManager* getMeshManager() { return this->meshMgr; }

  void setCharacterManager(CharacterManager* char_mgr) { this->char_mgr = char_mgr; }
  CharacterManager* getCharacterManager() { return this->char_mgr; }

  void setDatabase(Database* db) { this->db = db; }
  Database* getDatabase() { return this->db; }

  void setTables(Tables* tables) { this->tables = tables; }
  Tables* getTables() { return this->tables; }

  void setNetwork(Network* network) { this->network = network; }
  Network* getNetwork() { return this->network; }

  void setEntityManager(EntityManager* ent_mgr) { this->ent_mgr = ent_mgr; }
  EntityManager* getEntityManager() { return this->ent_mgr; }

  void setRaceManager(RaceManager* race_mgr) { this->race_mgr = race_mgr; }
  RaceManager* getRaceManager() { return this->race_mgr; }

  void setItemManager(ItemManager* item_mgr) { this->item_mgr = item_mgr; }
  ItemManager* getItemManager() { return this->item_mgr; }

  void setStatManager(StatManager* stat_mgr) { this->stat_mgr = stat_mgr; }
  StatManager* getStatManager() { return this->stat_mgr; }

  void setDoorManager(DoorManager* door_mgr) { this->door_mgr = door_mgr; }
  DoorManager* getDoorManager() { return this->door_mgr; }

  void setSkillManager(SkillManager* skill_mgr) { this->skill_mgr = skill_mgr; }
  SkillManager* getSkillManager() { return this->skill_mgr; }

  void setUserManager(UserManager* usr_mgr) { this->usr_mgr = usr_mgr; }
  UserManager* getUserManager() { return this->usr_mgr; }

  void setUserAccountManager(UserAccountManager* usr_acc_mgr) { this->usr_acc_mgr = usr_acc_mgr; }
  UserAccountManager* getUserAccountManager() { return this->usr_acc_mgr; }

  void setTimerEngine(TimerEngine* timer_engine) { this->timer_engine = timer_engine; }
  TimerEngine* getTimerEngine() { return this->timer_engine; }

  void setSpawner(Spawner* spawner) { this->spawner = spawner; }
  Spawner* getSpawner() { return this->spawner; }

  void setSectorManager(SectorManager* sectormanager) { this->sectormanager = sectormanager; }
  SectorManager* getSectorManager() { return this->sectormanager; }

  void setCollisionDetection(CollisionDetection* colldet) { this->colldet = colldet; }
  CollisionDetection* getCollisionDetection() { return this->colldet; }

  void setZoneManager(ZoneManager* zone_mgr) { this->zone_mgr = zone_mgr; }
  ZoneManager* getZoneManager() { return this->zone_mgr; }

  //void setSkillEngine(SkillEngine* spawner) { this->skillengine = skillengine; }
  //SkillEngine* getSkillEngine() { return this->skillengine; }

  void setEnvironmentManager(EnvironmentManager* environment_mgr) { this->environment_mgr = environment_mgr; }
  EnvironmentManager* getEnvironmentManager() { return this->environment_mgr; }

  void setReputationManager(ReputationManager* reputation_mgr) { this->reputation_mgr = reputation_mgr; }
  ReputationManager* getReputationManager() { return this->reputation_mgr; }

  void addEntity(const Entity* entity, bool presistent);
  void delEntity(const Entity* entity);
  void moveEntity(PcEntity* entity, const PtVector3& pos, float speed, bool run);
  void moveEntity(MountEntity* entity, const PtVector3& pos, float speed, bool run);
  void moveEntity(const NpcEntity* entity, const PtVector3& pos, float speed, bool run);
  void broadCast(const ByteStream& bs);
};

#endif // SERVER_H
