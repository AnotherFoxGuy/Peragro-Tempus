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

#ifndef NETMESSAGE_H
#define NETMESSAGE_H

#include "assert.h"
#include "string.h"

#include "common/geom/ptvector3.h"

//#include "common/network/socket.h"
#include "deserialiser.h"
#include "serialiser.h"

#include "nwtypes.h"

class Connection;

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

  virtual bool serialise(ByteStream* bs) = 0;

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
  unsigned short size;

public:
  GenericMessage(ByteStream* bs) : bs(bs)
  {
    Deserialiser serial(bs);
    size = serial.getSize();
    type = serial.getInt8();
    id = serial.getInt8();
  }

  ~GenericMessage() {}

  bool serialise(ByteStream* bs)
  {
    Serialiser serial(bs);
    serial.setInt8(type);
    serial.setInt8(id);
    return serial.isValid();
  }

  void deserialise(ByteStream* bs)
  {
    Deserialiser serial(bs);
    type = serial.getInt8();
    id = serial.getInt8();
  }

  unsigned short getSize()
  {
    return size;
  }

  ByteStream* getByteStream()
  {
    return bs;
  }
};

#endif // NETMESSAGE_H
