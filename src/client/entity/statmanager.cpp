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

      return true;
    }

    bool StatManager::AddStat(iEvent& ev)
    {
      using namespace PT::Events;

      Stat stat;

      ev.Retrieve("name", stat.name);
      ev.Retrieve("level", stat.level);

      stats.push_back(stat);
      return true;
    }

    int StatManager::GetStat(const char* name)
    {
      unsigned int i=0;
      while(i<stats.size())
      {
        if(!strcmp(stats[i].name,name)){return stats[i].level;}
        i++;
      }
      return 0;
    }

  } // Entity namespace
} // PT namespace
