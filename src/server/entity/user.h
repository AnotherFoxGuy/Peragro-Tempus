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

#include "common/util/monitorable.h"
#include "common/util/ptstring.h"

#include "server/network/connection.h"

#include "entitylist.h"
#include "permission.h"

class Network;
class Connection;
class Entity;
class PcEntity;

class User : public ptMonitorable<User>
{
private:
  size_t id;
  ptString name;
  char* pwhash;

  ptMonitor<Connection> connection;
  ptMonitor<PcEntity> own_entity;

  EntityList ent_list;

  PermissionList permissions;

public:
  User(size_t id) : id(id), pwhash(0), permissions(id) { }
  ~User() { delete [] pwhash; }

  size_t getId() const { return id; }

  const ptString& getName() const { return name; }
  void setName(ptString name) { this->name = name; }

  const char* getPwHash() { return pwhash; }
  void setPwHash(const char* pwhash, size_t pwhashlen)
  {
    delete [] this->pwhash;
    this->pwhash = new char[pwhashlen+1];
    strncpy(this->pwhash, pwhash, pwhashlen+1);
  }

  const PcEntity* getEntity() const { return own_entity.get(); }
  void setEntity(PcEntity* entity);

  const Connection* getConnection() const { return connection.get(); }
  void setConnection(Connection* connection)
  {
    if (connection)
      this->connection = connection->getRef();
    else
      this->connection.clear();
  }

  void sendAddEntity(const Entity* entity);
  void sendRemoveEntity(const Entity* entity);

  void clearEntityList() { ent_list.clear(); }
  void remove();

  PermissionList& getPermissionList() { return permissions; }
};

#endif // _USER_H_
