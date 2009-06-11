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

#include <wfmath/point.h>
#include "entity/entitymanager.h"

class TableManager;
class Database;
class EntityManager;
class SkillManager;
class Network;
class UserManager;
class TimerEngine;
class Entity;
class Character;
class CharacterEntity;
class Spawner;
class ByteStream;
class CollisionDetection;
class ZoneManager;
class LocationManager;
class EnvironmentManager;
class InteractionManager;

// Character stuff
class MovementManager;

class EquipmentFactory;

class SkillsFactory;
class AbilitiesFactory;
class VulnerabilitiesFactory;
class ReputationsFactory;
class ResourcesFactory;
class AttributesFactory;

class ItemTemplatesManager;

class SpeciesManager;

class Server
{
private:
  static Server* server;

  TableManager* tablemanager;
  Database* db;
  Network* network;
  EntityManager* ent_mgr;
  SkillManager* skill_mgr;
  UserManager* usr_mgr;
  TimerEngine* timer_engine;
  Spawner* spawner;
  CollisionDetection* colldet;
  ZoneManager* zone_mgr;
  LocationManager* locationMgr;
  EnvironmentManager* environment_mgr;
  InteractionManager* interactionMgr;

  // Character stuff
  MovementManager* movementManager;

  EquipmentFactory* equipmentFactory;

  SkillsFactory* skillsFactory;
  AbilitiesFactory* abilitiesFactory;
  VulnerabilitiesFactory* vulnerabilitiesFactory;
  ReputationsFactory* reputationsFactory;
  ResourcesFactory* resourcesFactory;
  AttributesFactory* attributesFactory;

  ItemTemplatesManager* itemTemplatesManager;

  SpeciesManager* speciesManager;

public:
  Server() { server = this; }
  static Server* getServer() { return server; }

  void setInteractionManager(InteractionManager* interactionMgr) {
    this->interactionMgr = interactionMgr;
  }
  InteractionManager* getInteractionManager() { return this->interactionMgr; }

  void setDatabase(Database* db) { this->db = db; }
  Database* getDatabase() { return this->db; }

  void SetTableManager(TableManager* tablemanager) { this->tablemanager = tablemanager; }
  TableManager* GetTableManager() { return this->tablemanager; }

  void setNetwork(Network* network) { this->network = network; }
  Network* getNetwork() { return this->network; }

  void setEntityManager(EntityManager* ent_mgr) { this->ent_mgr = ent_mgr; }
  EntityManager* getEntityManager() { return this->ent_mgr; }

  void setSkillManager(SkillManager* skill_mgr) { this->skill_mgr = skill_mgr; }
  SkillManager* getSkillManager() { return this->skill_mgr; }

  void setUserManager(UserManager* usr_mgr) { this->usr_mgr = usr_mgr; }
  UserManager* getUserManager() { return this->usr_mgr; }

  void setTimerEngine(TimerEngine* timer_engine) { this->timer_engine = timer_engine; }
  TimerEngine* getTimerEngine() { return this->timer_engine; }

  void setSpawner(Spawner* spawner) { this->spawner = spawner; }
  Spawner* getSpawner() { return this->spawner; }

  void setCollisionDetection(CollisionDetection* colldet) { this->colldet = colldet; }
  CollisionDetection* getCollisionDetection() { return this->colldet; }

  void setZoneManager(ZoneManager* zone_mgr) { this->zone_mgr = zone_mgr; }
  ZoneManager* getZoneManager() { return this->zone_mgr; }

  void setLocationManager(LocationManager* locationMgr) { this->locationMgr = locationMgr; }
  LocationManager* getLocationManager() { return this->locationMgr; }

  //void setSkillEngine(SkillEngine* spawner) { this->skillengine = skillengine; }
  //SkillEngine* getSkillEngine() { return this->skillengine; }

  void setEnvironmentManager(EnvironmentManager* environment_mgr) { this->environment_mgr = environment_mgr; }
  EnvironmentManager* getEnvironmentManager() { return this->environment_mgr; }

  // Character stuff
  void SetMovementManager(MovementManager* movementManager) { this->movementManager = movementManager; }
  MovementManager* GetMovementManager() { return this->movementManager; }

  void SetEquipmentFactory(EquipmentFactory* equipmentFactory) { this->equipmentFactory = equipmentFactory; }
  EquipmentFactory* GetEquipmentFactory() { return this->equipmentFactory; }

  void SetSkillsFactory(SkillsFactory* skillsFactory) { this->skillsFactory = skillsFactory; }
  SkillsFactory* GetSkillsFactory() { return this->skillsFactory; }

  void SetAbilitiesFactory(AbilitiesFactory* abilitiesFactory) { this->abilitiesFactory = abilitiesFactory; }
  AbilitiesFactory* GetAbilitiesFactory() { return this->abilitiesFactory; }

  void SetVulnerabilitiesFactory(VulnerabilitiesFactory* vulnerabilitiesFactory) { this->vulnerabilitiesFactory = vulnerabilitiesFactory; }
  VulnerabilitiesFactory* GetVulnerabilitiesFactory() { return this->vulnerabilitiesFactory; }

  void SetReputationsFactory(ReputationsFactory* reputationsFactory) { this->reputationsFactory = reputationsFactory; }
  ReputationsFactory* GetReputationsFactory() { return this->reputationsFactory; }

  void SetAttributesFactory(AttributesFactory* attributesFactory) { this->attributesFactory = attributesFactory; }
  AttributesFactory* GetAttributesFactory() { return this->attributesFactory; }

  void SetResourcesFactory(ResourcesFactory* resourcesFactory) { this->resourcesFactory = resourcesFactory; }
  ResourcesFactory* GetResourcesFactory() { return this->resourcesFactory; }


  void SetItemTemplatesManager(ItemTemplatesManager* itemTemplatesManager) { this->itemTemplatesManager = itemTemplatesManager; }
  ItemTemplatesManager* GetItemTemplatesManager() { return this->itemTemplatesManager; }

  void SetSpeciesManager(SpeciesManager* speciesManager) { this->speciesManager = speciesManager; }
  SpeciesManager* GetSpeciesManager() { return this->speciesManager; }

  void moveEntity(boost::shared_ptr<Character> entity, const WFMath::Point<3>& pos, float speed, bool run);
};

#endif // SERVER_H
