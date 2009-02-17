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
#include "components/entity/stat/stats.h"

#include <iutil/objreg.h>
#include <iutil/cfgmgr.h>
#include <iengine/sector.h>
#include <iutil/object.h>

#include "common/event/eventmanager.h"
#include "common/event/entityevent.h"

#include "client/entity/entity.h"
//#include "client/entity/player/playerentity.h"
//#include "client/entity/pc/pcentity.h"

/*
#include "client/gui/gui.h"
#include "client/gui/guimanager.h"
*/

#include "common/reporter/reporter.h"
#include "client/pointer/pointer.h"

CS_IMPLEMENT_PLUGIN
PT_IMPLEMENT_COMPONENTFACTORY (Stats, "peragro.entity.stats")

ComponentStats::ComponentStats(iObjectRegistry* object_reg) :
  scfImplementationType (this, object_reg)
{
}

ComponentStats::~ComponentStats()
{
}

bool ComponentStats::Initialize (PointerLibrary* pl, Client::Entity::Entity* ent)
{
  pointerlib = pl;
  entity = ent;

  using namespace PT::Events;
  using namespace PT::Entity;

  EventManager* evmgr = pointerlib->getEventManager();

  // TODO: Listen to one of the two, not both.
  // But entityType isn't known at this point.
  PT_REGISTER_LISTENER_ENTITY(ComponentStats, AddStat, "entity.stat.add", true)
  PT_REGISTER_LISTENER_ENTITY(ComponentStats, List, "entity.stat.list.player", false)

  PT_REGISTER_LISTENER_ENTITY(ComponentStats, UpdateStat, "entity.stat.change", true)

  return true;
} // end Initialize()

bool ComponentStats::UpdateStat(iEvent& ev)
{
  using namespace PT::Events;
  using namespace PT::Entity;

  unsigned int statId = -1;
  ev.Retrieve("id", statId);

  // Locate the correct stat.
  Stat* stat = GetStat(statId);
  if (!stat)
  {
    pointerlib->getReporter()->Report(PT::Error, "Failed to find stat %d for entity %d", statId, entity->GetId());
    return true;
  }

  unsigned int statlevel = -1;
  ev.Retrieve("level", statlevel);
  ev.Add("delta", stat->level - statlevel);
  stat->level = statlevel;

  return true;
} // end UpdateStat()

bool ComponentStats::List(iEvent& ev)
{
  using namespace PT::Events;

  csRef<iEvent> list;
  if (ev.Retrieve("statsList", list) == csEventErrNone)
  {
    csRef<iEventAttributeIterator> stats = list->GetAttributeIterator();
    while (stats->HasNext())
    {
      csRef<iEvent> stat; list->Retrieve(stats->Next(), stat);
      AddStat(*stat);
    } // end while
  }
  else
    pointerlib->getReporter()->Report(PT::Error, "ComponentStats failed to get statsList!");

  return true;
} // end List()

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

  /*@TODO: Should this really be here?
  I mean you'd have to link against client-gui etc...
  Do find another place!
  if (entity->GetType() == PT::Common::Entity::PlayerEntityType)
  {
    using namespace PT::GUI;
    using namespace PT::GUI::Windows;
    GUIManager* guimanager = pointerlib->getGUIManager();
    if (!guimanager) return true;
    StatusWindow* statusWindow = guimanager->GetWindow<StatusWindow>(STATUSWINDOW);
    if (!statusWindow) return true;
    statusWindow->AddSkill(stat.name.c_str(), stat.level);
  }
  */

  return true;
} // end AddStat()

Stat* ComponentStats::GetStat(unsigned int id)
{
  unsigned int i = 0;
  while (i<stats.size())
  {
    if (stats[i].id == id) {return &stats[i];}
    i++;
  }
  return 0;
} // end GetStat()

Stat* ComponentStats::GetStat(const char* name)
{
  unsigned int i = 0;
  while (i<stats.size())
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
