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

#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <map>
#include <boost/shared_ptr.hpp>
#include "common/util/ptstring.h"
#include "user.h"

class UserManager
{
public:
  friend class User;
  typedef std::map<std::string, boost::shared_ptr<User> > UserMap;
  typedef UserMap::iterator UserMapIter;
  typedef UserMap::const_iterator const_UserMapIter;

private:
  UserMap users;

private:
  void AddUser(boost::shared_ptr<User> user);
  void RemoveUser(boost::shared_ptr<User> user);
  void RemoveUser(User* user);
  void RemoveUser(const std::string& name);
  boost::shared_ptr<User> FindByName(const std::string& name);


public:
  UserManager() {}
  ~UserManager() {}

  size_t GetUserCount() { return users.size(); }

  const UserMap& GetUsers() { return users; }

  const ptString Login(const std::string& username, const std::string& password, boost::shared_ptr<User>& user);
  const ptString Signup(const std::string& username, const std::string& password);
};

#endif // USERMANAGER_H
