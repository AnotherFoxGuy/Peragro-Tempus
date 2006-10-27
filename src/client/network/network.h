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
#include "client/network/connectionhandler.h"

class Network
{
private:
  Socket socket;
  ConnectionHandler conn_handler;
  Receiver receiver;

  SocketAddress serveraddress;

public:
  Network(Client* client) 
  : conn_handler(this, client), receiver(&socket)
  {
  }

  virtual ~Network()
  {
    receiver.kill();
  }

  void init()
  {
    socket.init(0);

    receiver.registerHandler(&conn_handler);
    receiver.begin();
  }

  void stop()
  {
    receiver.end();
    socket.kill();
  }

  void setServerAddress(SocketAddress& addr)
  {
    serveraddress = addr;
  }

  void send(NetMessage* msg)
  {
    ByteStream bs;
    msg->serialise(&bs);
    socket.publish((const char*)bs.getData(), bs.getSize(), &serveraddress);
  }
};

#endif // _NETWORK_H_
