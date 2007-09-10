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

#ifndef _CONNECTIONHANDLER_H_
#define _CONNECTIONHANDLER_H_

#include "stdio.h"

#include "common/network/udp/udpmessagehandler.h"
#include "common/network/netmessage.h"
#include "common/network/udp/udpconnectionmessages.h"

#include "server/network/udp/connectionmanager.h"
#include "common/version.h"

class UdpConnectionHandler : public UdpMessageHandler
{
private:
  UdpConnectionManager* updConnMgr;
  MessageHandler* handlers[MSG_HANDLER_COUNT];
  Socket* socket;

public:
  UdpConnectionHandler(UdpConnectionManager* updConnMgr, Socket* socket)
  : updConnMgr(updConnMgr), socket(socket)
  {
  }

  void handle(GenericMessage* msg, SocketAddress& sock_addr)
  {
    char type = msg->getMsgType();
    char id = msg->getMsgId();

    if (type == 0 && id == CONNECTION::REQUEST) 
    {
      ConnectRequestMessage request_msg(0);
      request_msg.deserialise(msg->getByteStream());

      // Check so client has the correct version
      if (request_msg.getVersion() < CLIENTMINVERSION)
      {
        printf("Client is not good enough, its to old\n");
        printf("Client version %d, min version %d\n", 
          request_msg.getVersion(), CLIENTMINVERSION);
        // Do not let this client connect its outdated
        handleConnectionRequest(&sock_addr, false);
      }
      else
      {
        printf("Client is good enough\n");
        handleConnectionRequest(&sock_addr, true);
      }
    }
    else
    {
      UdpConnection* conn = updConnMgr->findBySocketAddress(&sock_addr);
      if (!conn) return; //No connection, no game.

      conn->seen();

      msg->setConnection(conn);

      if (type == 0)
      {
        if (id == CONNECTION::PONG) 
        {
          handlePong(msg);
        }
        else if (id == CONNECTION::PING) 
        {
          handlePing(msg);
        }
      }
      else if (type < MSG_HANDLER_COUNT)
      {
        handlers[(int)type]->handle(msg);
      }
    }
  }

  char getType()
  {
    return MESSAGES::CONNECTION;
  }

  void registerHandler(MessageHandler* msg_h)
  {
    handlers[(int)msg_h->getType()] = msg_h;
  }

private:
  void handleConnectionRequest(SocketAddress* sock_addr, bool succeeded);

  void handlePing(GenericMessage* msg)
  {
    printf("Received PingMessage\n");
    // If the connection were null or if it weren't UdpConnection, this function wouldn't be called in the first place.
    PongMessage pong_msg;
    ByteStream bs;
    pong_msg.serialise(&bs);
    msg->getConnection()->send(bs);
  }

  void handlePong(GenericMessage* msg)
  {
    printf("Received PongMessage\n");
    // If the connection were null or if it weren't UdpConnection, this function wouldn't be called in the first place.
    ((UdpConnection*)msg->getConnection())->seen();
  }
};

#endif // _CONNECTIONHANDLER_H_
