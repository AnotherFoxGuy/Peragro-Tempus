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

struct iObjectRegistry;
class Client;
class Network;
class ItemMGR;
class GUIManager;
class ptEntityManager;
class EffectsManager;
class CombatMGR;

class PointerLibrary
{
private:
  static PointerLibrary* pointerlib;

  iObjectRegistry* objreg;
  Client* client;
  Network* network;
  ItemMGR* itemmanager;
  GUIManager* guimanager;
  ptEntityManager* entitymanager;
  EffectsManager* effectsmanager;
  CombatMGR* combatmanager;

public:
  PointerLibrary() { pointerlib = this; }
  static PointerLibrary* getInstance() { return pointerlib; }

  void setObjectRegistry(iObjectRegistry* objreg) { this->objreg = objreg; }
  iObjectRegistry* getObjectRegistry() { return this->objreg; }
  
  void setClient(Client* client) { this->client = client; }
  Client* getClient() { return this->client; }

  void setNetwork(Network* network) { this->network = network; }
  Network* getNetwork() { return this->network; }

  void setItemManager(ItemMGR* itemmanager) { this->itemmanager = itemmanager; }
  ItemMGR* getItemManager() { return this->itemmanager; }

  void setGUIManager(GUIManager* guimanager) { this->guimanager = guimanager; }
  GUIManager* getGUIManager() { return this->guimanager; }

  void setEntityManager(ptEntityManager* entitymanager) { this->entitymanager = entitymanager; }
  ptEntityManager* getEntityManager() { return this->entitymanager; }

  void setEffectsManager(EffectsManager* effectsmanager) { this->effectsmanager = effectsmanager; }
  EffectsManager* getEffectsManager() { return this->effectsmanager; }

  void setCombatManager(CombatMGR* combatmanager) { this->combatmanager = combatmanager; }
  CombatMGR* getCombatManager() { return this->combatmanager; }

};

#endif // POINTERLIBRARY_H_
