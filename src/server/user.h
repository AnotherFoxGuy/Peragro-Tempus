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

#ifndef _USER_H_
#define _USER_H_

#include <string.h>
#include <time.h>
#include <math.h>

#include "common/entity/entitymanager.h"

class Connection;
class Network;

class User
{
private:
  size_t id;
  char* name;
  char* pwhash;
  Connection* connection;
  Entity* own_entity;
  EntityList ent_list;

public:
  User() : name(0), pwhash(0), connection(0), own_entity(0)
  {
  }

  ~User()
  {
    delete [] name;
    delete [] pwhash;
  }

  size_t getId()
  {
    return id;
  }

  void setId(size_t id)
  {
    this->id = id;
  }

  const char* getName()
  {
    return name;
  }

  void setName(const char* name, size_t namelen)
  {
    delete [] this->name;
    this->name = new char[namelen+1];
    strncpy(this->name, name, namelen+1);
  }

  const char* getPwHash()
  {
    return pwhash;
  }

  void setPwHash(const char* pwhash, size_t pwhashlen)
  {
    delete [] this->pwhash;
    this->pwhash = new char[pwhashlen+1];
    strncpy(this->pwhash, pwhash, pwhashlen+1);
  }

  void sendAddEntity(Entity* entity);
  void sendRemoveEntity(Entity* entity);

  Entity* getEntity()
  {
    return own_entity;
  }

  void setEntity(Entity* entity)
  {
    own_entity = entity;
  }

  Connection* getConnection()
  {
    return connection;
  }

  void setConnection(Connection* connection)
  {
    this->connection = connection;
  }

  void clearEntityList()
  {
    ent_list.clear();
  }

  void remove();
};

#endif // _USER_H_
