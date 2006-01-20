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

#include <sstream>

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

void UsersTable::insert(const std::string& name, const std::string& pwhash)
{
  std::ostringstream query;
  query << "insert into users (name, pwhash) values ('"
    << db->escape(name) << "','"
    << db->escape(pwhash) << "');";
  db->update(query.str());
}

void UsersTable::dropTable()
{
  db->update("drop table users;");
}

bool UsersTable::existsUser(const std::string& name)
{
  std::ostringstream query;
  query << "select id from users where name = '" << db->escape(name) << "';";
  ResultSet* rs = db->query(query.str());
  bool existence = (rs->GetRowCount() > 0);
  delete rs;
  return existence;
}

User* UsersTable::getUser(const std::string& name)
{
  std::ostringstream query;
  query << "select * from users where name = '" << db->escape(name) << "';";
  ResultSet* rs = db->query(query.str());
  if (!rs || rs->GetRowCount() == 0) 
    return 0;

  User* user = new User();
  user->setId(atoi(rs->GetData(0,0).c_str()));
  user->setName(rs->GetData(0,1));
  user->setPwHash(rs->GetData(0,2));
  delete rs;
  return user;
}
