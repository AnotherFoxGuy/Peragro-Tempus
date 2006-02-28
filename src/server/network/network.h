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

#ifndef _NETWORK_H_
#define _NETWORK_H_

#include "common/network/udp/udpreceiver.h"

#include "server/server.h"

#include "server/network/udp/udpnetwork.h"
#include "server/network/userhandler.h"

class Network
{
private:
  UserHandler user_handler;
  UdpNetwork udp_nw;

public:
  Network(Server* server) 
  : user_handler(server), udp_nw()
  {
    server->setNetwork(this);
  }

  void init(unsigned short port)
  {
    udp_nw.init(port);
    udp_nw.setHandler(&user_handler);
  }

  void getStats(unsigned int& sentbyte, unsigned int& recvbyte, unsigned int& timestamp)
  {
    udp_nw.getStats(sentbyte, recvbyte, timestamp);
  }
};

#endif // _NETWORK_H_
