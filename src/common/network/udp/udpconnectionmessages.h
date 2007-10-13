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

#ifndef _CONNECTIONMESSAGES_H_
#define _CONNECTIONMESSAGES_H_

#include "common/network/netmessage.h"

namespace CONNECTION
{
  enum MESSAGES
  {
    REQUEST=0,
    RESPONSE=1,
    PING=2,
    PONG=3
  };
}

class ConnectRequestMessage : public NetMessage
{
private:
  unsigned int version;
public:

  ConnectRequestMessage(unsigned int v) : 
    NetMessage(MESSAGES::CONNECTION,CONNECTION::REQUEST) { version = v; }

  void serialise(ByteStream* bs)
  {
    Serialiser serial(bs);
    serial.setInt8(type);
    serial.setInt8(id);
    serial.setInt32(version);
  }

  void deserialise(ByteStream* bs)
  {
    Deserialiser serial(bs);
    type = serial.getInt8();
    id = serial.getInt8();
    version = serial.getInt32();
  }

  unsigned char getMsgType()
  {
    return type;
  }

  unsigned char getMsgId()
  {
    return id;
  }
  
  unsigned int getVersion()
  {
    return version;
  }
};

class ConnectResponseMessage : public NetMessage
{
private: 
  unsigned char sessionId;
  bool succeeded;

public:
  ConnectResponseMessage(bool c) : NetMessage(MESSAGES::CONNECTION,CONNECTION::RESPONSE)
  {
    succeeded = c;
  }

  void serialise(ByteStream* bs)
  {
    Serialiser serial(bs);
    serial.setInt8(type);
    serial.setInt8(id);
    serial.setInt8(sessionId);
    serial.setInt8(succeeded?1:0);
  }

  void deserialise(ByteStream* bs)
  {
    Deserialiser serial(bs);
    type = serial.getInt8();
    id = serial.getInt8();
    sessionId = serial.getInt8();
    succeeded = serial.getInt8()>0;
  }

  unsigned char getSessionId()
  {
    return sessionId;
  }
  void setSessionId(int ssid)
  {
    sessionId = (unsigned char) (ssid & 0xFF);
  }

  bool connectionSucceeded()
  {
    return succeeded;
  }

};


class PingMessage : public NetMessage
{

public:
  PingMessage() : NetMessage(MESSAGES::CONNECTION,CONNECTION::PING) 
  {
  }

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
};

class PongMessage : public NetMessage
{

public:
  PongMessage() : NetMessage(MESSAGES::CONNECTION,CONNECTION::PONG) 
  {
  }

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
};

#endif // _CONNECTIONMESSAGES_H_
