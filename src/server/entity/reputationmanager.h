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

#ifndef REPUTATIONMANAGER_H
#define REPUTATIONMANAGER_H

#include "common/util/array.h"
#include "reputation.h"
#include "server/server.h"
#include "server/database/table-reputations.h"

class ReputationManager
{
private:
  Array<Reputation*> reputations;

public:
  ReputationManager() {}

  ~ReputationManager() { reputations.delAll(); }

  size_t getReputationCount()
  {
    return reputations.getCount();
  }

  Reputation* getReputation(size_t index)
  {
    return reputations.get(index);
  }

  void addReputation(Reputation* reputation)
  {
    reputations.add(reputation);
  }

  Reputation* addReputation(ptString name)
  {
    ReputationsTable* table = Server::getServer()->getDatabase()->getReputationsTable();
    Reputation* reputation = new Reputation();
    reputation->setName(name);
    reputation->setId(table->insert(name));
    reputations.add(reputation);
    return reputation;
  }

  void delReputation(Reputation* reputation)
  {
    if (!reputation) return;
    for (size_t i = 0; i<reputations.getCount(); i++)
    {
      Reputation* _reputation = reputations.get(i);
      if (_reputation->getId() == reputation->getId())
      {
        reputations.remove(i);
        return;
      }
    }
  }

  bool exists(Reputation* reputation)
  {
    if (!reputation) return false;
    for (size_t i = 0; i<reputations.getCount(); i++)
    {
      Reputation* _reputation = reputations.get(i);

      if (_reputation->getId() == reputation->getId())
        return true;
    }
    return false;
  }

  Reputation* findByName(ptString name)
  {
    if (name == ptString::Null) return 0;
    for (size_t i = 0; i<reputations.getCount(); i++)
    {
      Reputation* reputation = reputations.get(i);
      if (reputation->getName() == name)
      {
        return reputation;
      }
    }
    return 0;
  }

  Reputation* findById(int id)
  {
    for (size_t i = 0; i<reputations.getCount(); i++)
    {
      Reputation* reputation = reputations.get(i);
      if (reputation->getId() == id)
      {
        return reputation;
      }
    }
    return 0;
  }

  void loadFromDB(ReputationsTable* rt)
  {
    //Load all Reputations from Database
    rt->getAllReputations(reputations);
  }

  void clear()
  {
    reputations.removeAll();
  }
};

#endif // REPUTATIONMANAGER_H
