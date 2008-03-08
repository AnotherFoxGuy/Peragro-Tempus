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

#ifndef TCP_CONNECTIONMANAGER_H
#define TCP_CONNECTIONMANAGER_H

#include "common/util/array.h"
#include "common/util/thread.h"

#include "tcpconnection.h"

#include "common/network/includes.h"

class TcpConnectionManager
{
  Array<int> sockets;
  Array<TcpConnection*> connections;

public:

  TcpConnectionManager()
  {
  }

  ~TcpConnectionManager() {}

  void addConnection(TcpConnection* conn)
  {
    int socket = conn->getSocket();
    connections.add(conn);
    sockets.add(socket);
  }

  void removeConnection(TcpConnection* conn) 
  {
    connections.remove(connections.find(conn));
    sockets.remove(sockets.find(conn->getSocket()));
  }

  size_t getConnectionCount() { return connections.getCount(); }
  const int* getSockets() { return sockets.getRawArray(); }

  TcpConnection* findBySocket(int socket)
  {
    for (size_t i = 0; i < connections.getCount(); i++)
    {
      if (connections.get(i)->getSocket() == socket)
      {
        return connections.get(i);
      }
    }
    return 0;
  }
};

#endif // TCP_CONNECTIONMANAGER_H
