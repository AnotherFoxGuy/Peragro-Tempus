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

#include "table-servers.h"

ServersTable::ServersTable(Database* db) : Table(db)
{
  ResultSet* rs = db->Query("select count(*) from servers;");
  if (rs == 0)
  {
    createTable();
  }
  delete rs;
}

void ServersTable::createTable()
{
  printf("Creating Table servers...\n");
  db->Update("create table servers ("
    "id INTEGER, "
    "name TEXT, "
    "pwhash TEXT, "
    "PRIMARY KEY (id) );");

  db->Update("insert into servers (id, name, pwhash) values (0, '_npc_',null);");
}

void ServersTable::insert(ptString name, const char* pwhash)
{
  db->Update("insert into servers (name, pwhash) values ('%q','%q');", *name, pwhash);
}

void ServersTable::dropTable()
{
  db->Update("drop table servers;");
}

bool ServersTable::existsServer(ptString name)
{
  ResultSet* rs = db->Query("select id from servers where name = '%q' and id > 0;", *name);
  bool existence = (rs->GetRowCount() > 0);
  delete rs;
  return existence;
}

ServersTableVO* ServersTable::getServer(ptString name)
{
  ResultSet* rs = db->Query("select * from servers where name = '%q' and id > 0;", *name);
  if (!rs || rs->GetRowCount() == 0)
  {
    delete rs;
    return 0;
  }

  ServersTableVO* server = new ServersTableVO();
  server->id = atoi(rs->GetData(0,0).c_str());
  server->name = ptString(rs->GetData(0,1).c_str(), rs->GetData(0,1).length());
  server->passwd = rs->GetData(0,2).c_str();

  delete rs;
  return server;
}
