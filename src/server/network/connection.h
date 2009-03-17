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

#ifndef CONNECTION_H
#define CONNECTION_H

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

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
  boost::weak_ptr<User> user;
  CONNECTION::TYPE type;

public:
  Connection(CONNECTION::TYPE type) : type(type)
  {
  }

  virtual ~Connection()
  {
  }

  boost::shared_ptr<User> GetUser() const
  {
    return user.lock();
  }

  void SetUser(boost::shared_ptr<User> user)
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

#endif // CONNECTION_H
