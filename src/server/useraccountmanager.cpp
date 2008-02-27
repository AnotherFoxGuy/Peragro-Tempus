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

#include "useraccountmanager.h"

#include "server/database/database.h"
#include "server/database/table-users.h"
#include "server/entity/usermanager.h"

#include "server/server.h"

#include "server/entity/user.h"

const ptString UserAccountManager::login(ptString username, const char* password, User*& user)
{
  UserManager* um = server->getUserManager();

  // See if already logged in
  user = um->findByName(username);

  // Fetch user from DB
  if (!user)
  {
    UsersTableVO* vo = server->getDatabase()->getUsersTable()->getUser(username);
    if (vo)
    {
      user = new User(vo->id);
      user->setName(vo->name);
      user->setPwHash(vo->passwd.c_str(), vo->passwd.length());
    }
  }

  // Unknown User
  if (!user || strcmp(user->getPwHash(), password) != 0 )
  {
    user = 0;
    return ptString("Unknown user or invalid password", strlen("Unknown User"));
  }

  return ptString::Null;
}

const ptString UserAccountManager::signup(ptString username, const char* password)
{
  Database* db = server->getDatabase();
  UsersTable* ut = db->getUsersTable();

  std::string username_str = *username;
  bool nonspace=false;
  for (size_t i=0; i < username_str.length(); i++)
  {
    if (username_str[i] != ' '){nonspace=true;}
  }
  if (!nonspace){return ptString("Username may not contain only space", 35);}

  if (ut->existsUser(username))
  {
    return ptString("User exists already", strlen("User exists already"));
  }

  ut->insert(username, password);

  UsersTableVO* user = ut->getUser(username);

  if (user->id == 1) // first User, all rights!
  {
    User(user->id).getPermissionList().setLevel(Permission::Admin, 1);
  }
  else
  {
    User(user->id).getPermissionList().setLevel(Permission::Admin, 0);
  }

  // Temporary override for development, admin for all!
  User(user->id).getPermissionList().setLevel(Permission::Admin, 1);

  return ptString::Null;
}
