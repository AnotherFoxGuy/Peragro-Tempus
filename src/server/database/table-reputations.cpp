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

#include "database.h"

#include "table-reputations.h"
#include "server/reputation/reputation.h"

ReputationsTable::ReputationsTable(Database* db) : Table(db)
{
  ResultSet* rs = db->query("select count(*) from reputations;");
  if (rs == 0)
  {
    createTable();
  }
  delete rs;
}

void ReputationsTable::createTable()
{
  printf("Creating Table reputations...\n");
  db->update("create table reputations ("
    "id INTEGER, "
    "name TEXT, "
    "PRIMARY KEY (id) );");
}

int ReputationsTable::insert(ptString name)
{
  ResultSet* rs = db->query("insert into reputations (name) values ('%q');select id from reputations where name = '%q';", *name, *name);
  if (!rs) {
    return 0;
  }else{
    return atoi(rs->GetData(0,0).c_str());
  }
}

void ReputationsTable::dropTable()
{
  db->update("drop table reputations;");
}

bool ReputationsTable::existsReputation(ptString name)
{
  if (strlen(*name)> 512) assert("String too long");
  ResultSet* rs = db->query("select id from reputations where name = '%q';", *name);
  if (!rs) {
    return false;
  }
  bool existence = (rs->GetRowCount() > 0);
  delete rs;
  return existence;
}

Reputation* ReputationsTable::getReputation(ptString name)
{
  ResultSet* rs = db->query("select * from reputations where name = '%q';", *name);

  Reputation* reputation = 0;

  if (rs && rs->GetRowCount() == 1)
  {
    reputation = new Reputation();
    reputation->setId(atoi(rs->GetData(0,0).c_str()));
    reputation->setName(ptString(rs->GetData(0,1).c_str(), rs->GetData(0,1).length()));
  }
  delete rs;
  return reputation;
}

void ReputationsTable::getAllReputations(Array<Reputation*>& reputations)
{
  ResultSet* rs = db->query("select * from reputations;");
  if (!rs) return;
  for (size_t i=0; i<rs->GetRowCount(); i++)
  {
    Reputation* reputation = new Reputation();
    reputation->setId(atoi(rs->GetData(i,0).c_str()));
    reputation->setName(ptString(rs->GetData(i,1).c_str(), rs->GetData(i,1).length()));
    reputations.add(reputation);
  }
  delete rs;
}
