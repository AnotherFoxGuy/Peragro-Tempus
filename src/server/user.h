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

#include <string>

#include <time.h>
#include <math.h>

#include "common/entity/entitymanager.h"

class Connection;
class Network;

class User
{
private:
  size_t id;
  std::string name;
  std::string pwhash;
  Connection* connection;
  Entity* own_entity;
  EntityList ent_list;

public:
  User() : connection(0), own_entity(0)
  {
  }

  ~User()
  {
  }

  size_t getId() const
  {
    return id;
  }

  void setId(size_t id)
  {
    this->id = id;
  }

  std::string getName() const
  {
    return name;
  }

  void setName(const std::string& name)
  {
    this->name = name;
  }

  std::string getPwHash() const
  {
    return pwhash;
  }

  void setPwHash(const std::string& pwhash)
  {
    this->pwhash = pwhash;
  }

  void sendAddEntity(Entity* entity);
  void sendRemoveEntity(Entity* entity);

  Entity* getEntity() const
  {
    return own_entity;
  }

  void setEntity(Entity* entity)
  {
    own_entity = entity;
  }

  Connection* getConnection() const
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
