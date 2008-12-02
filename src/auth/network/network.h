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

#ifndef NETWORK_H
#define NETWORK_H

#include "server/server.h"

#include "auth/network/tcp/tcpreceiver.h"
#include "auth/network/tcp/tcpnetwork.h"

#include "common/network/messagehandler.h"

#include "auth/network/authhandler.h"

class Network
{
private:
  AuthHandler auth_handler;
  TcpNetwork tcp_nw;

public:
  Network()
  : tcp_nw()
  {
  }

  void init(unsigned short port)
  {
    tcp_nw.init(port);
    tcp_nw.registerHandler(&auth_handler);
    tcp_nw.start();
  }

  void getStats(size_t& sentbyte, size_t& recvbyte, size_t& timestamp)
  {
    tcp_nw.getStats(sentbyte, recvbyte, timestamp);
  }

  void shutdown()
  {
    tcp_nw.shutdown();
  }
};

#endif // NETWORK_H
