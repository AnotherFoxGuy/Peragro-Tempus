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

#include "database.h"

#include "table-users.h"

#include "server/user.h"

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
  db->update("create table users ("
    "id INTEGER, "
    "name TEXT, "
    "pwhash TEXT, "
    "PRIMARY KEY (id) );");
}

void UsersTable::insert(ptString name, const char* pwhash)
{
  if (strlen(*name) + strlen(pwhash) > 512) assert("Strings too long");
  db->update("insert into users (name, pwhash) values ('%s','%s');", *name, pwhash);
}

void UsersTable::dropTable()
{
  db->update("drop table users;");
}

bool UsersTable::existsUser(ptString name)
{
  if (strlen(*name)> 512) assert("String too long");
  char query[1024];
  sprintf(query, "select id from users where name = '%s';", *name);
  ResultSet* rs = db->query(query);
  bool existence = (rs->GetRowCount() > 0);
  delete rs;
  return existence;
}

User* UsersTable::getUser(ptString name)
{
  if (strlen(*name)> 512) assert("String too long");
  char query[1024];
  sprintf(query, "select * from users where name = '%s';", *name);
  ResultSet* rs = db->query(query);
  if (!rs || rs->GetRowCount() == 0) 
    return 0;

  User* user = new User();
  user->setId(atoi(rs->GetData(0,0).c_str()));
  user->setName(ptString(rs->GetData(0,1).c_str(), rs->GetData(0,1).length()));
  user->setPwHash(rs->GetData(0,2).c_str(), rs->GetData(0,2).length());
  delete rs;
  return user;
}
