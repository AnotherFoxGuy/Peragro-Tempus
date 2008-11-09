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

#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "common/database/database.h"

#include "table-characterreputations.h"

#include "server/entity/characterreputations.h"

CharacterReputationsTable::CharacterReputationsTable(Database* db) : Table(db)
{
  ResultSet* rs = db->query("select count(*) from characterreputation;");
  if (rs == 0)
  {
    createTable();
  }
  delete rs;
}

void CharacterReputationsTable::createTable()
{
  printf("Creating Table characterreputation...\n");
  db->update("create table characterreputation ("
    "id INTEGER, "
    "reputation INTEGER, "
    "level INTEGER, "
    "PRIMARY KEY (id, reputation) );");
}

void CharacterReputationsTable::insert(int characterreputation, int reputation, int level)
{
  db->update("insert or replace into characterreputation (id, reputation, level) values ('%d','%d','%d');",
    characterreputation, reputation, level);
}

void CharacterReputationsTable::set(int characterreputation, Reputation* reputation, int level)
{
  insert(characterreputation, reputation->getId(), level);
}

int CharacterReputationsTable::get(int characterreputation, Reputation* reputation)
{
  ResultSet* rs = db->query("select * from characterreputation where id = '%d' and reputation = '%d';",
    characterreputation, reputation->getId());

  int level = 0;

  if (rs->GetRowCount() == 1)
  {
    level = atoi(rs->GetData(0,0).c_str());
  }
  else if (rs->GetRowCount() == 0)
  {
    level = 0;
  }
  else
  {
    printf("DB inconsistency: multiple equal reputations in the characterreputation!");
  }

  delete rs;

  return level;
}

void CharacterReputationsTable::dropTable()
{
  db->update("drop table characterreputation;");
}

void CharacterReputationsTable::getAllEntries(Array<CharReputation*>& entries, int id)
{
  ResultSet* rs = db->query("select reputation, level from characterreputation where id = '%d';", id);
  if (!rs) return;
  for (size_t i=0; i<rs->GetRowCount(); i++)
  {
    CharReputation* entry = new CharReputation();
    int reputation_id = atoi(rs->GetData(i,0).c_str());
    entry->reputation_id = reputation_id;
    entry->level = atoi(rs->GetData(i,1).c_str());
    entries.add(entry);
  }
  delete rs;
}
