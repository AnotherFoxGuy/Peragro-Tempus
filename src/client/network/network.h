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

#include "common/network/tcp/tcpsocket.h"
//#include "common/reporter/reporter.h"
#include "client/network/connectionhandler.h"
#include "client/network/tcp/tcpreceiver.h"

class Network
{
private:
  TcpSocket socket;
  ConnectionHandler conn_handler;
  Receiver receiver;

  SocketAddress serveraddress;

public:
  Network()
    : conn_handler(this), receiver(&socket)
  {
  }

  virtual ~Network()
  {
    if(receiver.isRunning()){receiver.kill();}
  }

  bool init()
  {
    if (socket.init(serveraddress.port, serveraddress.ip, false))
    {
      receiver.registerHandler(&conn_handler);
      receiver.begin();
      return true;
    }
    return false;
  }

  bool isRunning()
  {
    return receiver.isRunning();
  }

  void stop()
  {
    receiver.end();
    socket.kill();
  }

  void setServerAddress(const char* host, unsigned int port)
  {
    SocketAddress addr = TcpSocket::getSocketAddress(host, port);
    serveraddress = addr;
  }

  void send(NetMessage* msg)
  {
    if (!receiver.isRunning()){return;}
    ByteStream bs;
    if (msg->serialise(&bs) == true)
    {
      TcpSocket::publish(socket.getSocket(), (const char*)bs.getData(), bs.getSize());
    }
    else
    {
      //PointerLibrary::getInstance()->getReporter()->Report(PT::Error, "Invalid message, won't send!");
    }
  }
};

#endif // NETWORK_H
