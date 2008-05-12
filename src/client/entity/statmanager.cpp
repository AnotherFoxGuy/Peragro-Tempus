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

#include "client/entity/statmanager.h"

#include "client/event/entityevent.h"

#include "client/pointer/pointer.h"
#include "client/entity/entitymanager.h"
#include "client/event/eventmanager.h"
#include "client/combat/combatmanager.h"

#include "client/gui/gui.h"
#include "client/gui/guimanager.h"

#include "client/entity/player/playerentity.h"

namespace PT
{
  namespace Entity
  {

    StatManager::StatManager ()
    {

    }

    StatManager::~StatManager ()
    {
    }

    bool StatManager::Initialize ()
    {
      using namespace PT::Events;

      EventHandler<StatManager>* cb = new EventHandler<StatManager>(&StatManager::AddStat, this);
      // Register listener for EntityStatEvent.
      PointerLibrary::getInstance()->getEventManager()->AddListener("entity.stat", cb);

      EventHandler<StatManager>* update = new EventHandler<StatManager>(
                                              &StatManager::UpdateStat, this);
      // Register listener for EntityStatEvent.
      PointerLibrary::getInstance()->getEventManager()->
                      AddListener("entity.stat.change", update);

      return true;
    }

    /**
     *
     * Will take care of stat updates and take appropriate actions.
     *
     * @param ev The iEvent containing the stat update.
     *
     */
    bool StatManager::UpdateStat(iEvent& ev)
    {
      using namespace PT::Events;
      Stat stat;
      const char* tmpname;
      int maxLife;

      ev.Retrieve("name", tmpname);
      stat.name = tmpname;
      ev.Retrieve("level", stat.level);
      ev.Retrieve("id", stat.id);
      ev.Retrieve("entityid", stat.entityId);

      // Locate the correct stat.
      for (unsigned int i = 0; i < stats.size(); i++) {
        if(stats[i].id == stat.id) {
          stats[i].level = stat.level;
        }
      }

      // TODO really shouldn't calculate it here, but get it from the character.
      // Life 1 * endurance
      maxLife = GetStat("Endurance");

      if (strncmp("Health", stat.name.c_str(), strlen("Health")) == 0) {
        // If the health update belong to the player, update the gui.
        if (stat.entityId == PlayerEntity::Instance()->GetId()) {
          GUIManager* guimanager = PointerLibrary::getInstance()->getGUIManager();
          HUDWindow* hudWindow = guimanager->GetHUDWindow();
          hudWindow->SetHP(stat.level, maxLife);
        }
        // If HP = 0, then play the die animation for the dying entity.
        if (stat.level == 0) {
          PointerLibrary::getInstance()->getCombatManager()->Die(stat.entityId);
        }
      }
      return true;
    }

    bool StatManager::AddStat(iEvent& ev)
    {
      using namespace PT::Events;

      Stat stat;
      const char* tmpname;

      ev.Retrieve("name", tmpname);
      stat.name = tmpname;
      ev.Retrieve("level", stat.level);
      ev.Retrieve("id", stat.id);

      stats.push_back(stat);
      return true;
    }

    int StatManager::GetStat(const char* name)
    {
      unsigned int i=0;
      while(i<stats.size())
      {
        if(stats[i].name == name){return stats[i].level;}
        i++;
      }
      return 0;
    }
  } // Entity namespace
} // PT namespace
