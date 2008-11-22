/*
    Copyright (C) 2008 Development Team of Peragro Tempus

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

#ifndef AUTHMESSAGES_H
#define AUTHMESSAGES_H

#include "netmessage.h"

namespace AUTH
{
  enum MESSAGES
  {
    CLIENTLOGINREQUEST=0,
    CLIENTLOGINCHALLENGE=1,
    CLIENTLOGINRESPONSE=2,
    SERVERLOGINREQUEST=3,
    SERVERLOGINCHALLENGE=4,
    SERVERLOGINRESPONSE=5,
    JOINSERVERREQUEST=6,
    SESSIONID=7,
    CLIENTJOINED=8,
    CLIENTLEFT=9,
    REGISTERGAMEREQUEST=10,
    REGISTERSERVERREQUEST=11,
    REGISTERSERVERRESPONSE=12,
    REGISTERGAMERESPONSE=13,
    SERVERLIST=14,
    SERVERLISTADD=15,
    SERVERLISTREMOVE=16,
    SERVERLISTUPDATE=17
  };
}

class ClientLoginRequestMessage : public NetMessage
{
  unsigned int serverid;

public:
  ClientLoginRequestMessage() : NetMessage(MESSAGES::AUTH,AUTH::CLIENTLOGINREQUEST)
  {
  }

  ~ClientLoginRequestMessage()
  {
  }

  bool serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  unsigned int getServerId() const { return serverid; }
  void setServerId(unsigned int x) { serverid = x; }

};

class ClientLoginChallengeMessage : public NetMessage
{
  unsigned int challenge;

public:
  ClientLoginChallengeMessage() : NetMessage(MESSAGES::AUTH,AUTH::CLIENTLOGINCHALLENGE)
  {
  }

  ~ClientLoginChallengeMessage()
  {
  }

  bool serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  unsigned int getChallenge() const { return challenge; }
  void setChallenge(unsigned int x) { challenge = x; }

};

class ClientLoginResponseMessage : public NetMessage
{
  const char* hash;

public:
  ClientLoginResponseMessage() : NetMessage(MESSAGES::AUTH,AUTH::CLIENTLOGINRESPONSE)
  {
  }

  ~ClientLoginResponseMessage()
  {
  }

  bool serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  const char* getHash() const { return hash; }
  void setHash(const char* x) { hash = x; }

};

class ServerLoginRequestMessage : public NetMessage
{
  unsigned int serverid;

public:
  ServerLoginRequestMessage() : NetMessage(MESSAGES::AUTH,AUTH::SERVERLOGINREQUEST)
  {
  }

  ~ServerLoginRequestMessage()
  {
  }

  bool serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  unsigned int getServerId() const { return serverid; }
  void setServerId(unsigned int x) { serverid = x; }

};

class ServerLoginChallengeMessage : public NetMessage
{
  unsigned int challenge;

public:
  ServerLoginChallengeMessage() : NetMessage(MESSAGES::AUTH,AUTH::SERVERLOGINCHALLENGE)
  {
  }

  ~ServerLoginChallengeMessage()
  {
  }

  bool serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  unsigned int getChallenge() const { return challenge; }
  void setChallenge(unsigned int x) { challenge = x; }

};

class ServerLoginResponseMessage : public NetMessage
{
  const char* hash;

public:
  ServerLoginResponseMessage() : NetMessage(MESSAGES::AUTH,AUTH::SERVERLOGINRESPONSE)
  {
  }

  ~ServerLoginResponseMessage()
  {
  }

  bool serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  const char* getHash() const { return hash; }
  void setHash(const char* x) { hash = x; }

};

class JoinServerRequestMessage : public NetMessage
{
  unsigned int serverid;

public:
  JoinServerRequestMessage() : NetMessage(MESSAGES::AUTH,AUTH::JOINSERVERREQUEST)
  {
  }

  ~JoinServerRequestMessage()
  {
  }

  bool serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  unsigned int getServerId() const { return serverid; }
  void setServerId(unsigned int x) { serverid = x; }

};

class SessionIdMessage : public NetMessage
{
  unsigned int sessionid;

public:
  SessionIdMessage() : NetMessage(MESSAGES::AUTH,AUTH::SESSIONID)
  {
  }

  ~SessionIdMessage()
  {
  }

  bool serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  unsigned int getSessionId() const { return sessionid; }
  void setSessionId(unsigned int x) { sessionid = x; }

};

class ClientJoinedMessage : public NetMessage
{
  unsigned int userid;

public:
  ClientJoinedMessage() : NetMessage(MESSAGES::AUTH,AUTH::CLIENTJOINED)
  {
  }

  ~ClientJoinedMessage()
  {
  }

  bool serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  unsigned int getUserId() const { return userid; }
  void setUserId(unsigned int x) { userid = x; }

};

class ClientLeftMessage : public NetMessage
{
  unsigned int userid;

public:
  ClientLeftMessage() : NetMessage(MESSAGES::AUTH,AUTH::CLIENTLEFT)
  {
  }

  ~ClientLeftMessage()
  {
  }

  bool serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  unsigned int getUserId() const { return userid; }
  void setUserId(unsigned int x) { userid = x; }

};

class RegisterGameRequestMessage : public NetMessage
{
  ptString name;
  unsigned int ipaddress;

public:
  RegisterGameRequestMessage() : NetMessage(MESSAGES::AUTH,AUTH::REGISTERGAMEREQUEST)
  {
  }

  ~RegisterGameRequestMessage()
  {
  }

  bool serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  ptString getName() const { return name; }
  void setName(ptString x) { name = x; }

  unsigned int getIpAddress() const { return ipaddress; }
  void setIpAddress(unsigned int x) { ipaddress = x; }

};

class RegisterServerRequestMessage : public NetMessage
{
  unsigned int userid;
  ptString username;

public:
  RegisterServerRequestMessage() : NetMessage(MESSAGES::AUTH,AUTH::REGISTERSERVERREQUEST)
  {
  }

  ~RegisterServerRequestMessage()
  {
  }

  bool serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  unsigned int getUserId() const { return userid; }
  void setUserId(unsigned int x) { userid = x; }

  ptString getUserName() const { return username; }
  void setUserName(ptString x) { username = x; }

};

class RegisterServerResponseMessage : public NetMessage
{
  const char* passwordhash;
  ptString error;

public:
  RegisterServerResponseMessage() : NetMessage(MESSAGES::AUTH,AUTH::REGISTERSERVERRESPONSE)
  {
  }

  ~RegisterServerResponseMessage()
  {
  }

  bool serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  const char* getPasswordHash() const { return passwordhash; }
  void setPasswordHash(const char* x) { passwordhash = x; }

  ptString getError() const { return error; }
  void setError(ptString x) { error = x; }

};

class RegisterGameResponseMessage : public NetMessage
{
  ptString error;

public:
  RegisterGameResponseMessage() : NetMessage(MESSAGES::AUTH,AUTH::REGISTERGAMERESPONSE)
  {
  }

  ~RegisterGameResponseMessage()
  {
  }

  bool serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  ptString getError() const { return error; }
  void setError(ptString x) { error = x; }

};

class ServerListMessage : public NetMessage
{
  class Listunnamed1
  {
  public:
    unsigned int serverid;
    ptString name;
    unsigned int ipaddress;
    bool isjoinable;
  };

  unsigned int unnamed1count;
  Listunnamed1* unnamed1;


public:
  ServerListMessage() : NetMessage(MESSAGES::AUTH,AUTH::SERVERLIST)
  {
    unnamed1 = 0;
  }

  ~ServerListMessage()
  {
    delete [] unnamed1;
  }

  bool serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  unsigned char getunnamed1Count() const { return unnamed1count; }
  void setunnamed1Count(unsigned char ic)
  {
    unnamed1count = ic;
    delete [] unnamed1;
    unnamed1 = new Listunnamed1[ic];
  }

  // --- begin Listunnamed1 Getter and Setter ---

  unsigned int getServerId(size_t i) { return unnamed1[i].serverid; }
  void setServerId(size_t i, unsigned int x) { unnamed1[i].serverid = x; }

  ptString getName(size_t i) { return unnamed1[i].name; }
  void setName(size_t i, ptString x) { unnamed1[i].name = x; }

  unsigned int getIpAddress(size_t i) { return unnamed1[i].ipaddress; }
  void setIpAddress(size_t i, unsigned int x) { unnamed1[i].ipaddress = x; }

  bool getIsJoinable(size_t i) { return unnamed1[i].isjoinable; }
  void setIsJoinable(size_t i, bool x) { unnamed1[i].isjoinable = x; }

  // --- end Listunnamed1 Getter and Setter ---

};

class ServerListAddMessage : public NetMessage
{
  unsigned int serverid;
  ptString name;
  unsigned int ipaddress;
  bool isjoinable;

public:
  ServerListAddMessage() : NetMessage(MESSAGES::AUTH,AUTH::SERVERLISTADD)
  {
  }

  ~ServerListAddMessage()
  {
  }

  bool serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  unsigned int getServerId() const { return serverid; }
  void setServerId(unsigned int x) { serverid = x; }

  ptString getName() const { return name; }
  void setName(ptString x) { name = x; }

  unsigned int getIpAddress() const { return ipaddress; }
  void setIpAddress(unsigned int x) { ipaddress = x; }

  bool getIsJoinable() const { return isjoinable; }
  void setIsJoinable(bool x) { isjoinable = x; }

};

class ServerListRemoveMessage : public NetMessage
{
  unsigned int serverid;

public:
  ServerListRemoveMessage() : NetMessage(MESSAGES::AUTH,AUTH::SERVERLISTREMOVE)
  {
  }

  ~ServerListRemoveMessage()
  {
  }

  bool serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  unsigned int getServerId() const { return serverid; }
  void setServerId(unsigned int x) { serverid = x; }

};

class ServerListUpdateMessage : public NetMessage
{
  unsigned int serverid;
  bool isjoinable;

public:
  ServerListUpdateMessage() : NetMessage(MESSAGES::AUTH,AUTH::SERVERLISTUPDATE)
  {
  }

  ~ServerListUpdateMessage()
  {
  }

  bool serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  unsigned int getServerId() const { return serverid; }
  void setServerId(unsigned int x) { serverid = x; }

  bool getIsJoinable() const { return isjoinable; }
  void setIsJoinable(bool x) { isjoinable = x; }

};

#endif // AUTHMESSAGES_H
