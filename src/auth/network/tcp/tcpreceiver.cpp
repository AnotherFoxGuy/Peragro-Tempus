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

#include "tcpreceiver.h"

#include "common/network/includes.h"

void Receiver::Run()
{
  const int* connection_sockets = tcpConnMgr->getSockets();
  size_t len = tcpConnMgr->getConnectionCount() + 1;
  size_t len_out = 0;

  int* sockets = new int[len+1];
  sockets[0] = socket->getSocket();
  for (size_t i = 0; i < len && connection_sockets != 0; i++)
  {
    sockets[i+1] = connection_sockets[i];
  }

  const int* ready_sockets = TcpSocket::select(sockets, len, len_out);

  for (size_t i = 0; i < len_out; i++)
  {
    if (ready_sockets[i] == socket->getSocket())
    {
      int newsocket = (int) ::accept(ready_sockets[i], 0, 0);
      TcpConnection* connection = new TcpConnection(newsocket);
      tcpConnMgr->addConnection(connection);
      printf("New connection on socket %d\n", newsocket);
    }
    else
    {
      TcpConnection* conn = tcpConnMgr->findBySocket(ready_sockets[i]);

      ByteStream& buffer = conn->getBuffer();

      int size = TcpSocket::receive(ready_sockets[i], (char*)buffer.getData() + buffer.getSize(), buffer.getMaxSize() - buffer.getSize());

      buffer.setSize(size + buffer.getSize());

      if (buffer.getSize() == 0)
      {
        if (conn != 0)
        {
          conn->peerLost();
          tcpConnMgr->removeConnection(conn);
          delete conn;
          printf("Disconnect of socket %d\n", ready_sockets[i]);
        }
      }
      else
      {
        //printf("--[NW]-- Received %d bytes\n", buffer.getSize());
        while (true)
        {
          GenericMessage message(&buffer);

          if (buffer.getSize() < message.getSize()) break;

          handlers->handle(&message, ready_sockets[i]);

          //printf("--[NW]-- Message length %d bytes, remaining %d bytes\n", message.getSize(), buffer.getSize() - message.getSize());

          if (buffer.getSize() == message.getSize())
          {
            buffer.clear();
            break;
          }

          unsigned char* data = (unsigned char*) buffer.getData();
          memmove(data, data + message.getSize(), buffer.getSize() - message.getSize());
          buffer.setSize(buffer.getSize() - message.getSize());
        }
      }
    }
  }

  delete [] sockets;
  delete [] ready_sockets;
}
