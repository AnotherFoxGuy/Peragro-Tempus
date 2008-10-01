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

#include "reputationmanager.h"


#include "reputation.h"

#include "server/server.h"
#include "server/database/tables.h"
#include "server/database/table-reputations.h"


size_t ReputationManager::getReputationCount()
{
  return reputations.getCount();
}

Reputation* ReputationManager::getReputation(size_t index)
{
  return reputations.get(index);
}

void ReputationManager::addReputation(Reputation* reputation)
{
  reputations.add(reputation);
}

Reputation* ReputationManager::addReputation(ptString name)
{
  ReputationsTable* table = Server::getServer()->getTables()->getReputationsTable();
  Reputation* reputation = new Reputation();
  reputation->setName(name);
  reputation->setId(table->insert(name));
  reputations.add(reputation);
  return reputation;
}

void ReputationManager::delReputation(Reputation* reputation)
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

bool ReputationManager::exists(Reputation* reputation)
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

Reputation* ReputationManager::findByName(ptString name)
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

Reputation* ReputationManager::findById(int id)
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

void ReputationManager::loadFromDB(ReputationsTable* rt)
{
  //Load all Reputations from Database
  rt->getAllReputations(reputations);
}

void ReputationManager::clear()
{
  reputations.removeAll();
}
