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

#ifndef _CHATMESSAGES_H_
#define _CHATMESSAGES_H_

#include "netmessage.h"

namespace CHAT
{
  enum CHAT_MESSAGES
  {
    CHAT=0
  };
};

class ChatMessage : public NetMessage
{
  char msgtype;
  const char* msg;

public:
  ChatMessage() : NetMessage(MESSAGES::CHAT,CHAT::CHAT) 
  {
  }

  void serialise(ByteStream* bs)
  {
    Serialiser serial(bs);
    serial.setInt8(type);
    serial.setInt8(id);
    serial.setInt8(msgtype);
    serial.setString(msg);
  }

  void deserialise(ByteStream* bs)
  {
    Deserialiser serial(bs);
    type = serial.getInt8();
    id = serial.getInt8();
    msgtype = serial.getInt8();
    serial.getString(msg);
  }

  unsigned char getType()
  {
    return msgtype;
  }
  void setType(unsigned char t)
  {
    msgtype = t;
  }

  const char* getMessage()
  {
    return msg;
  }

  void setMessage(const char* message)
  {
    msg = message;
  }
};

#endif // _CHATMESSAGES_H_
