/*
    Copyright (C) 2007 Development Team of Peragro Tempus

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

#include <string.h>

#include "nwfactory.h"
#include "network.h"


nwFactory::nwFactory()
{
}

nwFactory::~nwFactory()
{
}

void nwFactory::createNetwork()
{
  nw = new nwNetwork();
}

void nwFactory::createType(unsigned int id, const char* name)
{
  nwType* type = new nwType();
  type->id = id;
  type->name = name;
  nw->types.push_back(type);

  hasher.process_bytes(&id, sizeof(id));
  hasher.process_bytes(name, strlen(name));
}

void nwFactory::createMessage(unsigned int id, const char* name, const char* typeName)
{
  nwMessage* msg = new nwMessage();
  msg->id = id;
  msg->name = name;
  nwType* type = nw->getType(typeName);
  msg->type = type;
  type->msgs.push_back(msg);
  nw->msgs.push_back(msg);

  hasher.process_bytes(&id, sizeof(id));
  hasher.process_bytes(name, strlen(name));
  hasher.process_bytes(typeName, strlen(typeName));
}

void nwFactory::createPeer(const char* name)
{
  nwPeer* peer = new nwPeer();
  peer->name = name;
  nw->peers.push_back(peer);

  hasher.process_bytes(name, strlen(name));
}

void nwFactory::createPeerSendMsg(const char* peerName, const char* msgName)
{
  nwPeer* peer = nw->getPeer(peerName);
  nwMessage* msg = nw->getMessage(msgName);
  peer->sendMsg.push_back(msg);

  hasher.process_bytes(peerName, strlen(peerName));
  hasher.process_bytes(msgName, strlen(msgName));
}

void nwFactory::createPeerReceiveMsg(const char* peerName, const char* msgName)
{
  nwPeer* peer = nw->getPeer(peerName);
  nwMessage* msg = nw->getMessage(msgName);
  peer->recvMsg.push_back(msg);

  hasher.process_bytes(peerName, strlen(peerName));
  hasher.process_bytes(msgName, strlen(msgName));
}

void nwFactory::createMessageParameter(const char* msgName, const char* paramName, const char* type, const char* eventName, bool entitySpecific)
{
  nwMessage* msg = nw->getMessage(msgName);
  nwParams* param = new nwParams();
  param->name = paramName;
  param->eventNames[eventName] = entitySpecific;
  msg->params.push_back(param);

  if (!strncmp(type, "string", 6))
  {
    param->type = nwParamType::STRING;
  }
  else if (!strncmp(type, "text", 4))
  {
    param->type = nwParamType::TEXT;
  }
  else if (!strncmp(type, "vector3f", 8))
  {
    param->type = nwParamType::VECTOR3F;
  }
  else if (!strncmp(type, "bool", 4))
  {
    param->type = nwParamType::BOOL;
  }
  else if (!strncmp(type, "uint8", 5))
  {
    param->type = nwParamType::UINT8;
  }
  else if (!strncmp(type, "uint16", 6))
  {
    param->type = nwParamType::UINT16;
  }
  else if (!strncmp(type, "uint32", 6))
  {
    param->type = nwParamType::UINT32;
  }
  else if (!strncmp(type, "int8", 4))
  {
    param->type = nwParamType::INT8;
  }
  else if (!strncmp(type, "int16", 5))
  {
    param->type = nwParamType::INT16;
  }
  else if (!strncmp(type, "int32", 5))
  {
    param->type = nwParamType::INT32;
  }
  else if (!strncmp(type, "float", 6))
  {
    param->type = nwParamType::FLOAT;
  }
  else if (!strncmp(type, "colour24", 8))
  {
    param->type = nwParamType::COLOUR24;
  }
  else if (!strncmp(type, "list", 4))
  {
    param->type = nwParamType::LIST;
  }

  hasher.process_bytes(msgName, strlen(msgName));
  hasher.process_bytes(paramName, strlen(paramName));
  hasher.process_bytes(type, strlen(type));
}

void nwFactory::createListParameter(const char* msgName, const char* listName, const char* paramName, const char* type)
{
  nwMessage* msg = nw->getMessage(msgName);
  nwParams* param = new nwParams();
  nwParams* list = msg->getParam(listName);
  param->name = paramName;
  list->params.push_back(param);

  if (!strncmp(type, "string", 6))
  {
    param->type = nwParamType::STRING;
  }
  else if (!strncmp(type, "text", 4))
  {
    param->type = nwParamType::TEXT;
  }
  else if (!strncmp(type, "vector3f", 8))
  {
    param->type = nwParamType::VECTOR3F;
  }
  else if (!strncmp(type, "bool", 4))
  {
    param->type = nwParamType::BOOL;
  }
  else if (!strncmp(type, "uint8", 5))
  {
    param->type = nwParamType::UINT8;
  }
  else if (!strncmp(type, "uint16", 6))
  {
    param->type = nwParamType::UINT16;
  }
  else if (!strncmp(type, "uint32", 6))
  {
    param->type = nwParamType::UINT32;
  }
  else if (!strncmp(type, "int8", 5))
  {
    param->type = nwParamType::INT8;
  }
  else if (!strncmp(type, "int16", 6))
  {
    param->type = nwParamType::INT16;
  }
  else if (!strncmp(type, "int32", 6))
  {
    param->type = nwParamType::INT32;
  }
  else if (!strncmp(type, "float", 6))
  {
    param->type = nwParamType::FLOAT;
  }
  else if (!strncmp(type, "colour24", 8))
  {
    param->type = nwParamType::COLOUR24;
  }
  else if (!strncmp(type, "list", 4))
  {
    param->type = nwParamType::LIST;
  }

  hasher.process_bytes(msgName, strlen(msgName));
  hasher.process_bytes(listName, strlen(listName));
  hasher.process_bytes(paramName, strlen(paramName));
  hasher.process_bytes(type, strlen(type));
}

void nwFactory::calcHash(boost::crc_32_type& peerhasher, nwPeer* peer)
{
  peerhasher.process_bytes(peer->name.c_str(), peer->name.length());
  for (size_t i = 0; i < peer->recvMsg.size(); i++)
  {
    nwMessage* msg = peer->recvMsg[i];
    calcHash(peerhasher, msg);
  }
  for (size_t i = 0; i < peer->sendMsg.size(); i++)
  {
    nwMessage* msg = peer->sendMsg[i];
    calcHash(peerhasher, msg);
  }
}

void nwFactory::calcHash(boost::crc_32_type& peerhasher, nwMessage* msg)
{
  peerhasher.process_bytes(&msg->id, sizeof(msg->id));
  peerhasher.process_bytes(msg->name.c_str(), msg->name.length());
  peerhasher.process_bytes(msg->type, sizeof(msg->type));
  for (size_t i = 0; i < msg->params.size(); i++)
  {
    nwParams* param = msg->params[i];
    calcHash(peerhasher, param);
  }
}

void nwFactory::calcHash(boost::crc_32_type& peerhasher, nwParams* param)
{
  peerhasher.process_bytes(param->name.c_str(), param->name.length());
  peerhasher.process_bytes(&param->type, sizeof(param->type));
  for (size_t i = 0; i < param->params.size(); i++)
  {
    nwParams* subparam = param->params[i];
    calcHash(peerhasher, subparam);
  }
}

nwNetwork* nwFactory::getNetwork()
{
  for (size_t i = 0; i < nw->peers.size(); i++)
  {
    boost::crc_32_type peerhasher;
    nwPeer* peer = nw->peers[i];
    calcHash(peerhasher, peer);
    peer->netId = peerhasher.checksum();
  }
  nw->netId = hasher.checksum();
  return nw;
}
