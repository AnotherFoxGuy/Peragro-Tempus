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

#ifndef _UDP_CONNECTION_H_
#define _UDP_CONNECTION_H_

#include <time.h>

#include "common/network/udp/udpsocket.h"
#include "server/network/connection.h"

class User;

class UdpConnection : public Connection
{
private:
  Socket* socket;
  SocketAddress address;
  int ssid;
  size_t last_seen;

public:
  UdpConnection(Socket* socket, SocketAddress* address, int ssid) 
    : Connection(CONNECTION::UDP), socket(socket), address(*address), ssid(ssid)
  {
    user = 0;
    seen();
  }

  virtual ~UdpConnection()
  {
  }

  size_t sinceLastSeen()
  {
    return (size_t) time(0) - last_seen;
  }

  const SocketAddress* getSocketAddress() const
  {
    return &address;
  }

  void seen()
  {
    last_seen = (size_t) time(0);
  }

  void send(ByteStream& bs)
  {
    socket->publish((const char*)bs.getData(), bs.getSize(), getSocketAddress());
  }

  void peerLost();
};

#endif // _UDP_CONNECTION_H_
