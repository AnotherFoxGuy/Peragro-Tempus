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

#include "common/util/array.h"
#include "user.h"

class UserManager
{
private:
  Array<User*> users;

public:
  UserManager() {}

  size_t getUserCount()
  {
    return users.getCount();
  }

  User* getUser(size_t index)
  {
    return users.get(index);
  }

  void addUser(User* user)
  {
    users.add(user);
  }

  void delUser(size_t index)
  {
    users.del(index);
  }

  void delUser(User* user)
  {
    if (!user) 
    {
      printf("Removing user: Not a user!\n");
      return;
    }

    for (size_t i = 0; i<users.getCount(); i++)
    {
      User* tmp_user = users.get(i);
      if (strlen(tmp_user->getName()) == strlen(user->getName()) && !strcmp(tmp_user->getName(), user->getName()))
      {
        users.del(i);
        printf("Removing user: User removed!\n");
        return;
      }
    }
    printf("Removing user: User not found!\n");
  }

  User* findByName(const char* name)
  {
    if (!name) return 0;
    for (size_t i = 0; i<users.getCount(); i++)
    {
      User* user = users.get(i);
      if (strlen(user->getName()) == strlen(name) && !strcmp(user->getName(), name))
      {
        return user;
      }
    }
    return 0;
  }
};

#endif // _USERMANAGER_H_
