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

#include <vector>

class nwNetwork;
class nwPeer;
class nwType;
class nwMessage;
class nwParams;

class nwNetwork
{
public:
  std::vector<nwPeer*> peers;
  std::vector<nwMessage*> msgs;
  std::vector<nwType*> types;

  size_t version, subversion, microversion;

  nwPeer* getPeer(const char* name);
  nwMessage* getMessage(const char* name);
  nwType* getType(const char* name);
};

class nwPeer
{
public:
  std::string name;
  std::vector<nwMessage*> sendMsg;
  std::vector<nwMessage*> recvMsg;
};

class nwType
{
public:
  int id;
  std::string name;

  std::vector<nwMessage*> msgs;
};

class nwMessage
{
public:
  int id;
  std::string name;
  nwType* type;

  std::vector<nwParams*> params;

  nwParams* getParam(const char*);
};

namespace nwParamType
{
  enum Type
  {
    STRING,
    TEXT,
    VECTOR3F,
    BOOL,
    UINT8,
    UINT16,
    UINT32,
    FLOAT,
    COLOUR24,
    LIST
  };
}

class nwParams
{
public:
  std::string name;
  nwParamType::Type type;

  // only used for lists
  std::vector<nwParams*> params;
};
