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

#include "network.h"

nwPeer* nwNetwork::getPeer(const char* name)
{
  for (size_t i=0; i<peers.size(); i++)
  {
    nwPeer* peer = peers[i];
    if (! peer->name.compare(name))
    {
      return peer;
    }
  }
  return 0;
}

nwMessage* nwNetwork::getMessage(const char* name)
{
  for (size_t i=0; i<msgs.size(); i++)
  {
    nwMessage* msg = msgs[i];
    if (! msg->name.compare(name))
    {
      return msg;
    }
  }
  return 0;
}

nwType* nwNetwork::getType(const char* name)
{
  for (size_t i=0; i<types.size(); i++)
  {
    nwType* type = types[i];
    if (! type->name.compare(name))
    {
      return type;
    }
  }
  return 0;
}

nwParams* nwMessage::getParam(const char* name)
{
  for (size_t i=0; i<params.size(); i++)
  {
    nwParams* param = params[i];
    if (param->name.compare(name) == 0)
    {
      return param;
    }
  }
  return 0;
}
