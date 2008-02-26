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
class CombatMGR;

namespace PT
{
  class Client;
  class World;
  class StateManager;

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

  namespace Effect
  {
    class EffectsManager;
  } // Effect namespace

  namespace Entity
  {
    class EntityManager;
    class StatManager;
  } // Entity namespace

  namespace Events
  {
    class EventManager;
  } // Events namespace

  namespace Chat
  {
    class ChatManager;
  } // Chat namespace

} // PT namespace

class PointerLibrary
{
private:
  static PointerLibrary* pointerlib;

  iObjectRegistry* objreg;
  PT::Client* client;
  Cursor* cursor;
  Network* network;
  PT::Data::SectorDataManager* sectorDataManager;
  PT::Data::SkillDataManager* skillDataManager;
  PT::Data::ConnectionDataManager* connectionDataManager;
  PT::Data::EffectDataManager* effectDataManager;
  PT::Data::TeleportDataManager* teleportDataManager;
  PT::Misc::ServerSetupManager* serverSetupManager;
  GUIManager* guimanager;
  PT::Entity::EntityManager* entitymanager;
  PT::Entity::StatManager* statmanager;
  CombatMGR* combatmanager;
  PT::Events::EventManager* eventmanager;
  PT::Chat::ChatManager* chatmanager;
  PT::Effect::EffectsManager* effectsmanager;
  PT::StateManager* statemanager;

  PT::World* world;

public:
  PointerLibrary() {
    pointerlib = this;

    objreg = 0;
    client = 0;
    cursor = 0;
    network = 0;
    sectorDataManager = 0;
    skillDataManager = 0;
    connectionDataManager = 0;
    serverSetupManager = 0;
    effectDataManager = 0;
    guimanager = 0;
    entitymanager = 0;
    statmanager = 0;
    effectsmanager = 0;
    combatmanager = 0;
    eventmanager = 0;
    chatmanager = 0;
    world = 0;
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

  void setGUIManager(GUIManager* guimanager) { this->guimanager = guimanager; }
  GUIManager* getGUIManager() { return this->guimanager; }

  void setEntityManager(PT::Entity::EntityManager* entitymanager) { this->entitymanager = entitymanager; }
  PT::Entity::EntityManager* getEntityManager() { return this->entitymanager; }

  void setStatManager(PT::Entity::StatManager* statmanager) { this->statmanager = statmanager; }
  PT::Entity::StatManager* getStatManager() { return this->statmanager; }

  void setEffectsManager(PT::Effect::EffectsManager* effectsmanager) { this->effectsmanager = effectsmanager; }
  PT::Effect::EffectsManager* getEffectsManager() { return this->effectsmanager; }

  void setCombatManager(CombatMGR* combatmanager) { this->combatmanager = combatmanager; }
  CombatMGR* getCombatManager() { return this->combatmanager; }

  void setEventManager(PT::Events::EventManager* eventmanager) { this->eventmanager = eventmanager; }
  PT::Events::EventManager* getEventManager() { return this->eventmanager; }

  void setChatManager(PT::Chat::ChatManager* chatmanager) { this->chatmanager = chatmanager; }
  PT::Chat::ChatManager* getChatManager() { return this->chatmanager; }

  void setWorld(PT::World* world) { this->world = world; }
  PT::World* getWorld() { return this->world; }

  void setStateManager(PT::StateManager* statemanager) { this->statemanager = statemanager; }
  PT::StateManager* getStateManager() { return this->statemanager; }

};

#endif // POINTERLIBRARY_H_
