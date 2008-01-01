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

#ifndef RECEIVER_H
#define RECEIVER_H

#include "common/util/array.h"
#include "common/util/thread.h"
#include "common/network/netmessage.h"
#include "common/network/udp/udpsocket.h"
#include "common/network/udp/udpmessagehandler.h"

class Receiver : public Thread
{
private:
  Socket* socket;
  ByteStream buffer;

  UdpMessageHandler* handlers;

protected:
  void Run()
  {
    SocketAddress addr;

    //printf("ready!\n");

    buffer.clear();
    buffer.setSize(socket->receive((char*)buffer.getData(), buffer.getMaxSize(), &addr));
    if (buffer.getSize() == 0) return;

    GenericMessage message(&buffer);

    //printf("Received Message [%d|%d] from %d:%d\n", message.getMsgType(), message.getMsgId(), addr.ip, addr.port);

    handlers->handle(&message, addr);
  }

public:
  Receiver(Socket* socket) : socket(socket) {}
  virtual ~Receiver() {}

  void registerHandler(UdpMessageHandler* handler)
  {
    handlers = handler;
  }
};

#endif // RECEIVER_H
