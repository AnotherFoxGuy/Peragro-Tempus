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

  namespace Data
  {
    class ItemManager;
    class SectorManager;
    class EffectsManager;
  } // Data namespace 

  namespace Entity
  {
    class EntityManager;
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
  PT::Data::ItemManager* itemmanager;
  PT::Data::SectorManager* sectormanager;
  PT::Data::EffectsManager* effectsmanager;
  GUIManager* guimanager;
  PT::Entity::EntityManager* entitymanager;
  CombatMGR* combatmanager;
  PT::Events::EventManager* eventmanager;
  PT::Chat::ChatManager* chatmanager;

public:
  PointerLibrary() { 
    pointerlib = this; 

    objreg = 0;
    client = 0;
    cursor = 0;
    network = 0;
    itemmanager = 0;
    sectormanager = 0;
    guimanager = 0;
    entitymanager = 0;
    effectsmanager = 0;
    combatmanager = 0;
    eventmanager = 0;
    chatmanager = 0;
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

  void setItemManager(PT::Data::ItemManager* itemmanager) { this->itemmanager = itemmanager; }
  PT::Data::ItemManager* getItemManager() { return this->itemmanager; }

  void setSectorManager(PT::Data::SectorManager* sectormanager) { this->sectormanager = sectormanager; }
  PT::Data::SectorManager* getSectorManager() { return this->sectormanager; }

  void setGUIManager(GUIManager* guimanager) { this->guimanager = guimanager; }
  GUIManager* getGUIManager() { return this->guimanager; }

  void setEntityManager(PT::Entity::EntityManager* entitymanager) { this->entitymanager = entitymanager; }
  PT::Entity::EntityManager* getEntityManager() { return this->entitymanager; }

  void setEffectsManager(PT::Data::EffectsManager* effectsmanager) { this->effectsmanager = effectsmanager; }
  PT::Data::EffectsManager* getEffectsManager() { return this->effectsmanager; }

  void setCombatManager(CombatMGR* combatmanager) { this->combatmanager = combatmanager; }
  CombatMGR* getCombatManager() { return this->combatmanager; }

  void setEventManager(PT::Events::EventManager* eventmanager) { this->eventmanager = eventmanager; }
  PT::Events::EventManager* getEventManager() { return this->eventmanager; }

  void setChatManager(PT::Chat::ChatManager* chatmanager) { this->chatmanager = chatmanager; }
  PT::Chat::ChatManager* getChatManager() { return this->chatmanager; }

};

#endif // POINTERLIBRARY_H_
