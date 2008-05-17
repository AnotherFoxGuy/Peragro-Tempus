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

#ifndef POINTERLIBRARY_H_
#define POINTERLIBRARY_H_

class Cursor;
struct iObjectRegistry;
class Network;
class GUIManager;

namespace PT
{
  class Client;
  class World;
  class StateManager;
  class EnvironmentManager;
  class Reporter;

  namespace Events
  {
    class EventManager;
  } // Events namespace

  namespace Data
  {
    class SectorDataManager;
    class SkillDataManager;
    class ConnectionDataManager;
    class EffectDataManager;
    class TeleportDataManager;
  } // Data namespace

  namespace Misc
  {
    class ServerSetupManager;
  } // Misc namespace

  namespace Entity
  {
    class EntityManager;
    class PlayerEntity;
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

  namespace Environment
  {
    class EnvironmentManager;
  } // Environment namespace

  namespace Component
  {
    class ComponentManager;
  } // Component namespace

} // PT namespace

class PointerLibrary
{
private:
  static PointerLibrary* pointerlib;

  iObjectRegistry* objreg;
  PT::Client* client;
  Network* network;
  Cursor* cursor;

  PT::Events::EventManager* eventManager;
  PT::Data::EffectDataManager* effectDataManager;
  PT::Data::SectorDataManager* sectorDataManager;
  PT::Data::SkillDataManager* skillDataManager;
  PT::Data::ConnectionDataManager* connectionDataManager;
  PT::Data::TeleportDataManager* teleportDataManager;
  PT::Misc::ServerSetupManager* serverSetupManager;
  GUIManager* guiManager;
  PT::StateManager* stateManager;
  PT::Entity::EntityManager* entityManager;
  PT::Effect::EffectsManager* effectsManager;
  PT::Combat::CombatManager* combatManager;
  PT::Chat::ChatManager* chatManager;
  PT::Environment::EnvironmentManager* environmentManager;
  PT::Component::ComponentManager* componentManager;

  PT::World* world;
  PT::Reporter* reporter;

  PT::Entity::PlayerEntity* playerEntity;

public:
  PointerLibrary() {
    pointerlib = this;

    objreg = 0;
    client = 0;
    network = 0;
    cursor = 0;

    eventManager = 0;
    effectDataManager = 0;
    sectorDataManager = 0;
    skillDataManager = 0;
    connectionDataManager = 0;
    serverSetupManager = 0;
    guiManager = 0;
    stateManager = 0;
    environmentManager = 0;
    entityManager = 0;
    effectsManager = 0;
    combatManager = 0;
    chatManager = 0;
    componentManager = 0;

    world = 0;
    reporter = 0;

    playerEntity = 0;
  }

  static PointerLibrary* getInstance() { return pointerlib; }

  void setObjectRegistry(iObjectRegistry* objreg) { this->objreg = objreg; }
  iObjectRegistry* getObjectRegistry() { return this->objreg; }

  void setClient(PT::Client* client) { this->client = client; }
  PT::Client* getClient() { return this->client; }

  void setCursor(Cursor* cursor) { this->cursor = cursor; }
  Cursor* getCursor() { return this->cursor; }

  void setNetwork(Network* network) { this->network = network; }
  Network* getNetwork() { return this->network; }

  void setSectorDataManager(PT::Data::SectorDataManager* sectorDataManager) { this->sectorDataManager = sectorDataManager; }
  PT::Data::SectorDataManager* getSectorDataManager() { return this->sectorDataManager; }

  void setSkillDataManager(PT::Data::SkillDataManager* skillDataManager) { this->skillDataManager = skillDataManager; }
  PT::Data::SkillDataManager* getSkillDataManager() { return this->skillDataManager; }

  void setConnectionDataManager(PT::Data::ConnectionDataManager* connectionDataManager) { this->connectionDataManager = connectionDataManager; }
  PT::Data::ConnectionDataManager* getConnectionDataManager() { return this->connectionDataManager; }

  void setEffectDataManager(PT::Data::EffectDataManager* effectDataManager) { this->effectDataManager = effectDataManager; }
  PT::Data::EffectDataManager* getEffectDataManager() { return this->effectDataManager; }

  void setTeleportDataManager(PT::Data::TeleportDataManager* teleportDataManager) { this->teleportDataManager = teleportDataManager; }
  PT::Data::TeleportDataManager* getTeleportDataManager() { return this->teleportDataManager; }

  void setServerSetupManager(PT::Misc::ServerSetupManager* serverSetupManager) { this->serverSetupManager = serverSetupManager; }
  PT::Misc::ServerSetupManager* getServerSetupManager() { return this->serverSetupManager; }

  void setGUIManager(GUIManager* guiManager) { this->guiManager = guiManager; }
  GUIManager* getGUIManager() { return this->guiManager; }

  void setEntityManager(PT::Entity::EntityManager* entityManager) { this->entityManager = entityManager; }
  PT::Entity::EntityManager* getEntityManager() { return this->entityManager; }

  void setEffectsManager(PT::Effect::EffectsManager* effectsManager) { this->effectsManager = effectsManager; }
  PT::Effect::EffectsManager* getEffectsManager() { return this->effectsManager; }

  void setCombatManager(PT::Combat::CombatManager* combatManager) { this->combatManager = combatManager; }
  PT::Combat::CombatManager* getCombatManager() { return this->combatManager; }

  void setEventManager(PT::Events::EventManager* eventManager) { this->eventManager = eventManager; }
  PT::Events::EventManager* getEventManager() { return this->eventManager; }

  void setChatManager(PT::Chat::ChatManager* chatManager) { this->chatManager = chatManager; }
  PT::Chat::ChatManager* getChatManager() { return this->chatManager; }

  void setWorld(PT::World* world) { this->world = world; }
  PT::World* getWorld() { return this->world; }

  void setStateManager(PT::StateManager* stateManager) { this->stateManager = stateManager; }
  PT::StateManager* getStateManager() { return this->stateManager; }

  void setEnvironmentManager(PT::Environment::EnvironmentManager* environmentManager) { this->environmentManager = environmentManager; }
  PT::Environment::EnvironmentManager* getEnvironmentManager() { return this->environmentManager; }

  void setComponentManager(PT::Component::ComponentManager* componentManager) { this->componentManager = componentManager; }
  PT::Component::ComponentManager* getComponentManager() { return this->componentManager; }

  void setReporter(PT::Reporter* reporter) { this->reporter = reporter; }
  PT::Reporter* getReporter() { return this->reporter; }

  void setPlayer(PT::Entity::PlayerEntity* playerEntity) { this->playerEntity = playerEntity; }
  PT::Entity::PlayerEntity* getPlayer() { return this->playerEntity; }

};

#endif // POINTERLIBRARY_H_
