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

#ifndef UDPNETWORK_H
#define UDPNETWORK_H

#include "common/network/udp/udpreceiver.h"

#include "server/network/udp/udpconnectionhandler.h"
#include "server/network/udp/connectionmanager.h"
#include "server/network/udp/connectionpinger.h"

class UdpNetwork
{
private:
  Socket socket;
  Receiver receiver;
  UdpConnectionManager conn_mgr;
  UdpConnectionPinger  conn_pinger;
  UdpConnectionHandler conn_handler;

public:
  UdpNetwork()
  : receiver(&socket), conn_pinger(&conn_mgr), conn_handler(&conn_mgr, &socket)
  {
  }

  void init(unsigned short port)
  {
    socket.init(port);
    receiver.registerHandler(&conn_handler);
  }

  void start()
  {
    receiver.begin();
    conn_pinger.begin();
  }

  void registerHandler(MessageHandler* msg_h)
  {
    conn_handler.registerHandler(msg_h);
  }

  void getStats(size_t& sentbyte, size_t& recvbyte, size_t& timestamp)
  {
    socket.getNetworkStats(sentbyte, recvbyte, timestamp);
  }

  void shutdown()
  {
    conn_pinger.kill();
    receiver.end();
    socket.kill();
    receiver.kill();
  }
};

#endif // UDPNETWORK_H
