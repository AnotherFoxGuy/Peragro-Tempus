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

const char* UserAccountManager::login(const std::string& username,
  const std::string& password, User*& user)
{
  Database* db = server->getDatabase();
  UsersTable* ut = db->getUsersTable();
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
    return "Unknown User";
  }

  printf("User: '%s':'%s' <-  '%s':'%s'\n", username.c_str(), password.c_str(),
    user->getName().c_str(), user->getPwHash().c_str());

  if (user->getPwHash() != password)
  {
    user = 0;
    return "Invalid Password";
  }

  return 0;
}

const char* UserAccountManager::signup(const std::string& username,
  const std::string& password)
{
  Database* db = server->getDatabase();
  UsersTable* ut = db->getUsersTable();
  if (ut->existsUser(username))
  {
    return "User exists already";
  }

  ut->insert(username, password);
  return 0;
}
