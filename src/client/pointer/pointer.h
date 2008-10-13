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

#include "common/pointer/ipointer.h"

class Cursor;
struct iObjectRegistry;
class Network;

namespace PT
{
  class Client;
  class StateManager;
  class Reporter;

  namespace GUI
  {
    class GUIManager;
  } // GUI namespace

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

  namespace Component
  {
    class ComponentManager;
  } // Component namespace

  namespace Trade
  {
    class TradeManager;
  } // Trade namespace

  namespace Environment
  {
    class EnvironmentManager;
  } // Environment namespace

  namespace User
  {
    class UserManager;
  } // User namespace

} // PT namespace

class PointerLibrary : public iPointerLibrary
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
  PT::GUI::GUIManager* guiManager;
  PT::StateManager* stateManager;
  PT::Entity::EntityManager* entityManager;
  PT::Effect::EffectsManager* effectsManager;
  PT::Combat::CombatManager* combatManager;
  PT::Chat::ChatManager* chatManager;
  PT::Component::ComponentManager* componentManager;
  PT::Trade::TradeManager* tradeManager;
  PT::Environment::EnvironmentManager* environmentManager;
  PT::User::UserManager* userManager;

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
    entityManager = 0;
    effectsManager = 0;
    combatManager = 0;
    chatManager = 0;
    componentManager = 0;
    tradeManager = 0;
    environmentManager = 0;
    userManager = 0;

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

  void setGUIManager(PT::GUI::GUIManager* guiManager) { this->guiManager = guiManager; }
  PT::GUI::GUIManager* getGUIManager() { return this->guiManager; }

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

  void setStateManager(PT::StateManager* stateManager) { this->stateManager = stateManager; }
  PT::StateManager* getStateManager() { return this->stateManager; }

  void setComponentManager(PT::Component::ComponentManager* componentManager) { this->componentManager = componentManager; }
  PT::Component::ComponentManager* getComponentManager() { return this->componentManager; }

  void setTradeManager(PT::Trade::TradeManager* tradeManager) { this->tradeManager = tradeManager; }
  PT::Trade::TradeManager* getTradeManager() { return this->tradeManager; }

  void setEnvironmentManager(PT::Environment::EnvironmentManager* environmentManager) { this->environmentManager = environmentManager; }
  PT::Environment::EnvironmentManager* getEnvironmentManager() { return this->environmentManager; }

  void setUserManager(PT::User::UserManager* userManager) { this->userManager = userManager; }
  PT::User::UserManager* getUserManager() { return this->userManager; }

  void setReporter(PT::Reporter* reporter) { this->reporter = reporter; }
  PT::Reporter* getReporter() { return this->reporter; }

  void setPlayer(PT::Entity::PlayerEntity* playerEntity) { this->playerEntity = playerEntity; }
  PT::Entity::PlayerEntity* getPlayer() { return this->playerEntity; }

};

#endif // POINTERLIBRARY_H_
