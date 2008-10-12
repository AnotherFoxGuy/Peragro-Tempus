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

#include "nwxmlparser.h"
#include "nwfactory.h"

#include "network.h"

#include "ext/tinyxml/tinyxml.h"

#include <iostream>
#include <sstream>

nwXmlParser::nwXmlParser(nwFactory* factory) : factory(factory)
{
}

nwXmlParser::~nwXmlParser()
{
}

void nwXmlParser::parseNetwork(TiXmlElement* node)
{
  factory->createNetwork();

  parseMessageDefinitions(node->FirstChildElement("message-definitions"));
  parseMessageImplementations(node->FirstChildElement("message-implementations"));
  parsePeers(node->FirstChildElement("peers"));
}

void nwXmlParser::parsePeers(TiXmlElement* parentNode)
{
  TiXmlElement* node = parentNode->FirstChildElement("peer");
  while ( node )
  {
    const char* src = node->Attribute("src");
    if (src)
    {
      TiXmlDocument doc(fileroot.substr().append(src).c_str());
      if (doc.LoadFile())
      {
        TiXmlHandle docHandle(&doc);
        TiXmlElement* childNode = docHandle.FirstChild("peer").Element();
        parsePeer(childNode);
      }
    }
    node = node->NextSiblingElement("peer");
  }
}

void nwXmlParser::parsePeer(TiXmlElement* parentNode)
{
  const char* peerName = parentNode->Attribute("name");

  factory->createPeer(peerName);

  TiXmlElement* messageNode = 0;

  TiXmlElement* sendNode = parentNode->FirstChildElement("send");
  messageNode = sendNode->FirstChildElement("message");
  while ( messageNode )
  {
    const char* msgName = messageNode->Attribute("name");
    factory->createPeerSendMsg(peerName, msgName);
    messageNode = messageNode->NextSiblingElement("message");
  }

  TiXmlElement* recvNode = parentNode->FirstChildElement("receive");
  messageNode = recvNode->FirstChildElement("message");
  while ( messageNode )
  {
    const char* msgName = messageNode->Attribute("name");
    factory->createPeerReceiveMsg(peerName, msgName);
    messageNode = messageNode->NextSiblingElement("message");
  }
}

void nwXmlParser::parseMessageDefinitions(TiXmlElement* parentNode)
{
  TiXmlElement* node = parentNode->FirstChildElement("type");
  while ( node )
  {
    parseMsgDefType(node);
    node = node->NextSiblingElement("type");
  }
}

void nwXmlParser::parseMsgDefType(TiXmlElement* parentNode)
{
  unsigned int typeId = (unsigned int) atoi(parentNode->Attribute("id"));
  const char* type = parentNode->Attribute("name");
  factory->createType(typeId, type);

  TiXmlElement* node = parentNode->FirstChildElement("message");
  while ( node )
  {
    unsigned int msgId = (unsigned int) atoi(node->Attribute("id"));
    const char* name = node->Attribute("name");
    factory->createMessage(msgId, name, type);
    node = node->NextSiblingElement("message");
  }
}

void nwXmlParser::parseMessageImplementations(TiXmlElement* parentNode)
{
  TiXmlElement* node = parentNode->FirstChildElement("message-implementation");
  while ( node )
  {
    const char* src = node->Attribute("src");
    if (src)
    {
      TiXmlDocument doc(fileroot.substr().append(src).c_str());
      if (doc.LoadFile())
      {
        TiXmlHandle docHandle(&doc);
        TiXmlElement* childNode = docHandle.FirstChild("message-implementation").Element();
        parseMessageImplementation(childNode);
      }
      else
      {
        std::cout << doc.ErrorDesc();
      }
    }
    node = node->NextSiblingElement("message-implementation");
  }
}

void nwXmlParser::parseMessageImplementation(TiXmlElement* parentNode)
{
  TiXmlElement* node = parentNode->FirstChildElement("message");
  while ( node )
  {
    parseEvent(node);
    node = node->NextSiblingElement("message");
  }
}

void nwXmlParser::parseEvent(TiXmlElement* parentNode)
{
  const char* msgName = parentNode->Attribute("name");

  TiXmlElement* node = parentNode->FirstChildElement("event");
  if (!node)
    parseParams(parentNode, msgName, "", "");
  while ( node )
  {
    const char* eventName = node->Attribute("name");
    const char* entitySpecific = node->Attribute("specific");
    parseParams(node, msgName, eventName, entitySpecific);
    node = node->NextSiblingElement("event");
  }
}

void nwXmlParser::parseParams(TiXmlElement* parentNode, const char* msgName, const char* eventName, const char* entitySpecific)
{
  TiXmlElement* node = parentNode->FirstChildElement();

  factory->getNetwork()->getMessage(msgName)->eventNames[eventName]= false;

  while ( node )
  {
    TiXmlText* text = node->FirstChild()->ToText();

    if (text)
    {
      const char* paramName = text->Value();
      nwParams* param = factory->getNetwork()->getMessage(msgName)->getParam(paramName);
      bool spec = (strcmp(paramName, entitySpecific) == 0);
      if (param)
      {
        param->eventNames[eventName]= spec;
      }
      else
      {
      factory->createMessageParameter(msgName, paramName, node->Value(), eventName, spec);
      if (!strncmp(node->Value(), "list", 4))
        parseParamsList(node, msgName, paramName);
      }
    }
    else
    {
      std::ostringstream name;
      if (!node->Attribute("name"))
      {
        static int nameless = 0;
        nameless++;
        name << "unnamed" << nameless;
        node->SetAttribute("name", name.str().c_str());
      }
      bool spec = (strcmp(node->Attribute("name"), entitySpecific) == 0);
      factory->createMessageParameter(msgName, node->Attribute("name"), node->Value(), eventName, spec);
      if (!strncmp(node->Value(), "list", 4))
        parseParamsList(node, msgName, node->Attribute("name"));
    }

    node = node->NextSiblingElement();
  }
}

void nwXmlParser::parseParamsList(TiXmlElement* parentNode, const char* msgName, const char* listName)
{
  TiXmlElement* node = parentNode->FirstChildElement();
  while ( node )
  {
    TiXmlText* text = node->FirstChild()->ToText();

    if (text)
    {
      const char* paramName = text->Value();
      factory->createListParameter(msgName, listName, paramName, node->Value());
      if (!strncmp(node->Value(), "list", 4))
        parseParamsList(node, msgName, paramName);
    }

    node = node->NextSiblingElement();
  }
}

bool nwXmlParser::parse(const char* filename)
{
  fileroot = filename;
  size_t end = fileroot.find_last_of("/");
  fileroot = fileroot.substr(0,end+1);

  TiXmlDocument doc(filename);
  if (!doc.LoadFile())
    return false;

  TiXmlHandle docHandle(&doc);
  TiXmlNode* root = docHandle.FirstChild("pt-gen").Node();

  parseNetwork(root->ToElement());

  return true;
}
