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

#ifndef _SERVER_H_
#define _SERVER_H_

class CharacterManager;
class Database;
class EntityManager;
class RaceManager;
class ItemManager;
class StatManager;
class Network;
class UserManager;
class UserAccountManager;
class TimerEngine;
class Entity;
class CharacterEntity;
class Spawner;

class Server
{
private:
  static Server* server;

  CharacterManager* char_mgr;
  Database* db;
  Network* network;
  EntityManager* ent_mgr;
  RaceManager* race_mgr;
  ItemManager* item_mgr;
  StatManager* stat_mgr;
  UserManager* usr_mgr;
  UserAccountManager* usr_acc_mgr;
  TimerEngine* timer_engine;
  Spawner* spawner;

public:
  Server() { server = this; }
  static Server* getServer() { return server; }

  void setCharacterManager(CharacterManager* char_mgr) { this->char_mgr = char_mgr; }
  CharacterManager* getCharacterManager() { return this->char_mgr; }

  void setDatabase(Database* db) { this->db = db; }
  Database* getDatabase() { return this->db; }

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

  void setUserManager(UserManager* usr_mgr) { this->usr_mgr = usr_mgr; }
  UserManager* getUserManager() { return this->usr_mgr; }

  void setUserAccountManager(UserAccountManager* usr_acc_mgr) { this->usr_acc_mgr = usr_acc_mgr; }
  UserAccountManager* getUserAccountManager() { return this->usr_acc_mgr; }

  void setTimerEngine(TimerEngine* timer_engine) { this->timer_engine = timer_engine; }
  TimerEngine* getTimerEngine() { return this->timer_engine; }

  void setSpawner(Spawner* spawner) { this->spawner = spawner; }
  Spawner* getSpawner() { return this->spawner; }

  void addEntity(Entity* entity, bool presistent);
  void delEntity(Entity* entity);
  void moveEntity(CharacterEntity* entity, float* pos, float speed);
};

#endif // _SERVER_H_
