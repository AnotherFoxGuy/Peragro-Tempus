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
#include "server/usermanager.h"

#include "server/server.h"

#include "server/user.h"

const ptString UserAccountManager::login(ptString username, const char* password, User*& user)
{
  UserManager* um = server->getUserManager();

  // See if already logged in
  user = um->findByName(username);

  // Fetch user from DB
  if (!user)
  {
    user = server->getDatabase()->getUsersTable()->getUser(username);
  }

  // Unknown User
  if (!user)
  {
    return ptString("Unknown User", strlen("Unknown User"));
  }

  printf("User: '%s':'%s' <-  '%s':'%s'\n", *username, password, *user->getName(), user->getPwHash());

  if (strlen(user->getPwHash()) != strlen(password) || strcmp(user->getPwHash(), password))
  {
    user = 0;
    return ptString("Invalid Password", strlen("Invalid Password"));
  }

  return ptString(0,0);
}

const ptString UserAccountManager::signup(ptString username, const char* password)
{
  Database* db = server->getDatabase();
  UsersTable* ut = db->getUsersTable();
  if (ut->existsUser(username))
  {
    return ptString("User exists already", strlen("User exists already"));
  }

  ut->insert(username, password);
  return ptString(0,0);
}
