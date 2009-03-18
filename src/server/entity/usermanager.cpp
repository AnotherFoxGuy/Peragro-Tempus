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
#include "server/server.h"
#include "usermanager.h"
#include "server/database/tablemanager.h"
#include "server/database/table-users.h"

const ptString UserManager::Login(const std::string& username, const std::string& password, boost::shared_ptr<User>& user)
{
  if (username.empty() || password.empty())
    return ptString("You must enter username and password");

  // See if already logged in
  user = FindByName(username);

  // Fetch user from DB
  if (!user)
  {
    UsersTableVOp vo = Server::getServer()->GetTableManager()->Get<UsersTable>()->GetUser(username);
    if (vo)
    {
      user = boost::shared_ptr<User>(new User(vo->login));
      user->setPwHash(vo->passwordHash);
    }
  }

  // Unknown User
  if (!user || user->getPwHash() != password)
  {
    return ptString("Unknown user or invalid password");
  }

  AddUser(user);

  return ptString::Null;
}

const ptString Validate(const std::string& username, const std::string& password)
{
  bool nonspace=false;
  for (size_t i=0; i < username.length(); i++)
    if (username[i] != ' ')
      nonspace=true;

  if (!nonspace)
    return ptString("Username may not contain only space");

  if (password.size() < 6)
    return ptString("Password may not be shorter than 6 characters");

  return ptString::Null;
}

const ptString UserManager::Signup(const std::string& username, const std::string& password)
{
  if (username.empty() || password.empty())
    return ptString("You must enter username and password");

  ptString p = Validate(username, password);
  if (p != ptString::Null) return p;

  TableManager* tablemgr = Server::getServer()->GetTableManager();
  UsersTable* ut = tablemgr->Get<UsersTable>();

  if (ut->GetUser(username))
    return ptString("User exists already");

  ut->Insert(username, password);

  UsersTableVOp user = ut->GetUser(username);

  if (ut->GetAll().size() == 1) // first User, all rights!
  {
    User(user->login).getPermissionList().setLevel(Permission::Admin, 2);
  }
  else
  {
    // Temporary override for development, admin for all!
    User(user->login).getPermissionList().setLevel(Permission::Admin, 1);
  }

  return ptString::Null;
}

void UserManager::AddUser(boost::shared_ptr<User> user)
{
  RemoveUser(user);
  users[user->GetName()] = user;
}

void UserManager::RemoveUser(boost::shared_ptr<User> user)
{
  RemoveUser(user.get());
}

void UserManager::RemoveUser(User* user)
{
  std::map<std::string, boost::shared_ptr<User> >::iterator it;
  for (it=users.begin(); it != users.end(); it++)
  {
    if (it->second.get() == user)
    {
      users.erase(it);
      return;
    }
  }
}

void UserManager::RemoveUser(const std::string& name)
{
  std::map<std::string, boost::shared_ptr<User> >::iterator it;
  it = users.find(name);
  if (it != users.end())
    users.erase(it);
}

boost::shared_ptr<User> UserManager::FindByName(const std::string& name)
{
  std::map<std::string, boost::shared_ptr<User> >::const_iterator it;
  it = users.find(name);
  if (it != users.end())
    return it->second;
  else
    return boost::shared_ptr<User>();
}

