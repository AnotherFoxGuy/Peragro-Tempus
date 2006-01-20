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

#include "chatmessages.h"

ChatMessage::ChatMessage()
 : NetMessage(MESSAGES::CHAT,CHAT_MESSAGE) 
{
}

void ChatMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
  serial.setInt8(msgtype);
  serial.setString(msg);
}

void ChatMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
  msgtype = serial.getInt8();
  msg = serial.getString();
}

unsigned char ChatMessage::getType() const
{
  return msgtype;
}

void ChatMessage::setType(unsigned char t)
{
  msgtype = t;
}

std::string ChatMessage::getMessage() const
{
  return msg;
}

void ChatMessage::setMessage(const std::string& message)
{
  msg = message;
}

