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

#ifndef CHARACTERREPUTATIONS_H_
#define CHARACTERREPUTATIONS_H_

#include "reputation.h"
#include "server/database/table-characterreputations.h"

class CharReputation
{
public:
  int reputation_id;
  int level;
};

class Connection;

class CharacterReputations
{
private:
  Array<CharReputation*> entries;

  CharReputation* findEntry(int reputation_id)
  {
    for(unsigned int i=0; i<entries.getCount(); i++)
    {
      if (entries.get(i)->reputation_id == reputation_id)
      {
        return entries.get(i);
      }
    }
    return 0;
  }

  CharacterReputationsTable* crtab;

  int cr_id;

public:
  CharacterReputations() : crtab(0) {}
  ~CharacterReputations() { entries.delAll(); }

  void addReputation(Reputation* reputation, int level)
  {
    CharReputation* entry = findEntry(reputation->getId());
    if (!entry)
    {
      entry = new CharReputation();
      entry->reputation_id = reputation->getId();
      entry->level = level;
      entries.add(entry);
    }
    else
    {
      entry->level += level;
    }
    if (crtab) crtab->set(cr_id, reputation, entry->level);
  }

  bool takeReputation(Reputation* reputation, int level)
  {
    CharReputation* entry = findEntry(reputation->getId());
    if (!entry)
    {
      entry = new CharReputation();
      entry->reputation_id = reputation->getId();
      entry->level = -level;
      entries.add(entry);
    }
    else
    {
      entry->level -= level;
    }
    if (crtab) crtab->set(cr_id, reputation, entry->level);

    return true;
  }

  bool setReputation(Reputation* reputation, int level)
  {
    CharReputation* entry = findEntry(reputation->getId());
    if (!entry)
    {
      entry = new CharReputation();
      entry->reputation_id = reputation->getId();
      entry->level = level;
      entries.add(entry);
    }
    else
    {
      entry->level = level;
    }
    if (crtab) crtab->set(cr_id, reputation, entry->level);

    return true;
  }

  unsigned int getAmount(Reputation* reputation)
  {
    CharReputation* entry = findEntry(reputation->getId());
    return (entry?entry->level:0);
  }

  void loadFromDatabase(CharacterReputationsTable* crt, int id)
  {
    cr_id = id;
    crtab = crt;

    //Load all Reputations from Database
    crt->getAllEntries(entries, id);
  }

  //void sendAllReputations(Connection* conn); Do we need anything like this?
};

#endif // CHARACTERREPUTATIONS_H_
