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

//  !! This file was automatically generated by a network code generator   !!
//  !! Do not change this file since all changes will be overwritten later !!
//  !! Instead please change the source files here: peragro/data/generate  !!

#ifndef CHATMESSAGES_H
#define CHATMESSAGES_H

#include "netmessage.h"

namespace CHAT
{
  enum MESSAGES
  {
    CHAT=0,
    WHISPERTO=2,
    GROUP=4
  };
}

class ChatMessage : public NetMessage
{
  ptString speakername;
  unsigned char volume;
  const char* message;

public:
  ChatMessage() : NetMessage(MESSAGES::CHAT,CHAT::CHAT)
  {
  }

  ~ChatMessage()
  {
  }

  void serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  ptString getSpeakerName() const { return speakername; }
  void setSpeakerName(ptString x) { speakername = x; }

  unsigned char getVolume() const { return volume; }
  void setVolume(unsigned char x) { volume = x; }

  const char* getMessage() const { return message; }
  void setMessage(const char* x) { message = x; }

};

class WhisperToMessage : public NetMessage
{
  ptString listenername;
  const char* message;

public:
  WhisperToMessage() : NetMessage(MESSAGES::CHAT,CHAT::WHISPERTO)
  {
  }

  ~WhisperToMessage()
  {
  }

  void serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  ptString getListenerName() const { return listenername; }
  void setListenerName(ptString x) { listenername = x; }

  const char* getMessage() const { return message; }
  void setMessage(const char* x) { message = x; }

};

class GroupMessage : public NetMessage
{
  ptString speakername;
  ptString channel;
  const char* message;

public:
  GroupMessage() : NetMessage(MESSAGES::CHAT,CHAT::GROUP)
  {
  }

  ~GroupMessage()
  {
  }

  void serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  ptString getSpeakerName() const { return speakername; }
  void setSpeakerName(ptString x) { speakername = x; }

  ptString getChannel() const { return channel; }
  void setChannel(ptString x) { channel = x; }

  const char* getMessage() const { return message; }
  void setMessage(const char* x) { message = x; }

};

#endif // CHATMESSAGES_H
