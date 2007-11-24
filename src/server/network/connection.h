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

#ifndef _CONNECTION_H_
#define _CONNECTION_H_

#include "common/network/bytestream.h"
#include "common/util/monitorable.h"

class User;

namespace CONNECTION
{
  enum TYPE
  {
    UDP = 0,
    TCP = 1
  };
}

class Connection : public ptMonitorable<Connection>
{
protected:
  User* user;
  CONNECTION::TYPE type;

public:
  Connection(CONNECTION::TYPE type) : type(type)
  {
  }

  virtual ~Connection()
  {
  }

  User* getUser() const
  {
    return user;
  }

  void setUser(User* user)
  {
    // TODO: What if there is already a connection?
    this->user = user;
  }

  CONNECTION::TYPE getType() const
  {
    return type;
  }

  virtual void send(const ByteStream& bs) const = 0;
};

#endif // _CONNECTION_H_
