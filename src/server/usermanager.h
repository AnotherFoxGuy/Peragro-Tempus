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

#ifndef _USERMANAGER_H_
#define _USERMANAGER_H_

#include <vector>

#include "user.h"

class UserManager
{
private:
  std::vector<User*> users;

public:
  UserManager() {}

  size_t getUserCount() const
  {
    return users.size();
  }

  User* getUser(size_t index) const
  {
    return users[index];
  }

  void addUser(User* user)
  {
    users.push_back(user);
  }

  void delUser(size_t index)
  {
    users.erase(users.begin()+index);
  }

  void delUser(User* user)
  {
    if (!user) 
    {
      printf("Removing user: Not a user!\n");
      return;
    }

    
    for (size_t i = 0; i < users.size(); i++)
    {
      User* tmp_user = users[i];
      if (tmp_user->getName() == user->getName())
      {
        users.erase(users.begin()+i);
        printf("Removing user: User removed!\n");
        return;
      }
    }
    printf("Removing user: User not found!\n");
  }

  User* findByName(const std::string& name) const
  {
    if (name.length() == 0) return 0;
    for (size_t i = 0; i < users.size(); i++)
    {
      User* user = users[i];
      if (user->getName() == name)
      {
        return user;
      }
    }
    return 0;
  }
};

#endif // _USERMANAGER_H_

