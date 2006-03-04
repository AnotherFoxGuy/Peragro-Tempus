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

#ifndef _NETMESSAGE_H_
#define _NETMESSAGE_H_

#include "assert.h"
#include "string.h"

//#include "common/network/socket.h"
#include "deserialiser.h"
#include "serialiser.h"

class Connection;

namespace MESSAGES
{
  enum HANDLERS
  {
    CONNECTION=0,
    USER=1,
    ENTITY=2,
    CHAT=3,
    SKILL=4
  };
}

class NetMessage
{
protected:
  Connection* conn;

  char type;
  char id;

public:
  NetMessage() {};
  NetMessage(char type, char id) : type(type), id(id) {};
  virtual ~NetMessage() {}

  virtual void serialise(ByteStream* bs) = 0;

  unsigned char getMsgType()
  {
    return type;
  }

  unsigned char getMsgId()
  {
    return id;
  }

  inline void setConnection(Connection* conn)
  {
    this->conn = conn;
  }

  inline Connection* getConnection() const
  {
    return conn;
  }
};

class GenericMessage : public NetMessage
{
private:
  ByteStream* bs;

public:
  GenericMessage(ByteStream* bs) : bs(bs)
  {
    Deserialiser serial(bs);
    type = serial.getInt8();
    id = serial.getInt8();
  }

  ~GenericMessage() {}

  void serialise(ByteStream* bs)
  {
    Serialiser serial(bs);
    serial.setInt8(type);
    serial.setInt8(id);
  }

  void deserialise(ByteStream* bs)
  {
    Deserialiser serial(bs);
    type = serial.getInt8();
    id = serial.getInt8();
  }

  ByteStream* getByteStream()
  {
    return bs;
  }
};

#endif // _NETMESSAGE_H_
