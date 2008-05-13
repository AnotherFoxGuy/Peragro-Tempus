/*
    Copyright (C) 2008 Development Team of Peragro Tempus

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

#include <cssysdef.h>
#include "plugins/component/entity/stat/stats.h"

#include <iutil/objreg.h>
#include <iutil/cfgmgr.h>
#include <iengine/sector.h>
#include <iutil/object.h>

#include "client/event/eventmanager.h"
#include "client/event/entityevent.h"

#include "client/entity/entity.h"
//#include "client/entity/player/playerentity.h"
//#include "client/entity/pc/pcentity.h"

#include "client/reporter/reporter.h"
#include "client/pointer/pointer.h"

CS_IMPLEMENT_PLUGIN
IMPLEMENT_COMPONENTFACTORY (Stats, "peragro.entity.stats")

ComponentStats::ComponentStats(iObjectRegistry* object_reg) :
	scfImplementationType (this, object_reg)
{
}

ComponentStats::~ComponentStats()
{
}

bool ComponentStats::Initialize (PointerLibrary* pl, PT::Entity::Entity* ent)
{
  pointerlib = pl;
  entity = ent;

  using namespace PT::Events;
  using namespace PT::Entity;

  EventManager* evmgr = pointerlib->getEventManager();

  REGISTER_LISTENER(ComponentStats, AddStat, "entity.stat.add", true)
  REGISTER_LISTENER(ComponentStats, UpdateStat, "entity.stat.add", true)

  return true;
} // end Initialize()

bool ComponentStats::UpdateStat(iEvent& ev)
{
  using namespace PT::Events;
  using namespace PT::Entity;

  int maxLife;

  unsigned int statId = -1;
  ev.Retrieve("id", statId);

  // Locate the correct stat.
  Stat* stat = GetStat(statId);
  if (!stat)
  {
    printf("E: Failed to find stat %d for entity %d", statId, entity->GetId());
    return true;
  }

  unsigned int statlevel = -1;
  ev.Retrieve("level", statlevel);
  stat->level = statlevel;

  // TODO really shouldn't calculate it here, but get it from the character.
  // Life 1 * endurance
  maxLife = GetStatLevel("Endurance");

/*
  if (strncmp("Health", stat->name.c_str(), strlen("Health")) == 0) 
  {
    // If the health update belong to the player, update the gui.
    if (entity->GetId() == PlayerEntity::Instance()->GetId()) 
    {
      GUIManager* guimanager = PointerLibrary::getInstance()->getGUIManager();
      HUDWindow* hudWindow = guimanager->GetHUDWindow();
      hudWindow->SetHP(stat.level, maxLife);
    }
    // If HP = 0, then play the die animation for the dying entity.
    if (stat->level == 0) 
    {
      PointerLibrary::getInstance()->getCombatManager()->Die(stat.entityId);
    }
  }
*/

  return true;
} // end UpdateStat()

bool ComponentStats::AddStat(iEvent& ev)
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
} // end AddStat()

Stat* ComponentStats::GetStat(unsigned int id)
{
  unsigned int i = 0;
  while(i<stats.size())
  {
    if (stats[i].id == id) {return &stats[i];}
    i++;
  }
  return 0;
} // end GetStat()

Stat* ComponentStats::GetStat(const char* name)
{
  unsigned int i = 0;
  while(i<stats.size())
  {
    if (stats[i].name == name) {return &stats[i];}
    i++;
  }
  return 0;
} // end GetStat()

int ComponentStats::GetStatLevel(const char* name)
{
  Stat* stat = GetStat(name);
  if (stat)
  {
    return stat->level;
  }
  return 0;
} // end GetStatLevel()
