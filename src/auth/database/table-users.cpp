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

#include "common/database/database.h"

#include "table-users.h"

UsersTable::UsersTable(Database* db) : Table(db)
{
  ResultSet* rs = db->query("select count(*) from users;");
  if (rs == 0)
  {
    createTable();
  }
  delete rs;
}

void UsersTable::createTable()
{
  printf("Creating Table users...\n");
  db->update("create table users ("
    "id INTEGER, "
    "name TEXT, "
    "pwhash TEXT, "
    "PRIMARY KEY (id) );");

  db->update("insert into users (id, name, pwhash) values (0, '_npc_',null);");
}

void UsersTable::insert(ptString name, const char* pwhash)
{
  db->update("insert into users (name, pwhash) values ('%q','%q');", *name, pwhash);
}

void UsersTable::dropTable()
{
  db->update("drop table users;");
}

bool UsersTable::existsUser(ptString name)
{
  ResultSet* rs = db->query("select id from users where name = '%q' and id > 0;", *name);
  bool existence = (rs->GetRowCount() > 0);
  delete rs;
  return existence;
}

UsersTableVO* UsersTable::getUser(ptString name)
{
  ResultSet* rs = db->query("select * from users where name = '%q' and id > 0;", *name);
  if (!rs || rs->GetRowCount() == 0)
  {
    delete rs;
    return 0;
  }

  UsersTableVO* user = new UsersTableVO();
  user->id = atoi(rs->GetData(0,0).c_str());
  user->name = ptString(rs->GetData(0,1).c_str(), rs->GetData(0,1).length());
  user->passwd = rs->GetData(0,2).c_str();

  delete rs;
  return user;
}
