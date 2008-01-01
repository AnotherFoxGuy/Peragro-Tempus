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

#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include "common/util/array.h"
#include "udpconnection.h"
#include "common/util/printhelper.h"

class UdpConnectionManager
{
private:
  Array<UdpConnection*> connections;

public:
  UdpConnectionManager() {}
  ~UdpConnectionManager() 
  {
    connections.delAll();
  }
  size_t getConnectionCount()
  {
    return connections.getCount();
  }

  UdpConnection* getConnection(size_t index)
  {
    return connections.get(index);
  }

  void addConnection(UdpConnection* connection)
  {
    connections.add(connection);
  }

  void delConnection(size_t index)
  {
    printf("delete connection: %" SIZET "\n", index);
    connections.del(index);
  }

  void delConnection(UdpConnection* connection)
  {
    for (size_t i = 0; i<connections.getCount(); i++)
    {
      UdpConnection* conn = connections.get(i);
      if (*conn->getSocketAddress() == *connection->getSocketAddress())
      {
        connections.del(i);
      }
    }
  }

  void delConnection(Connection* connection)
  {
    if (connection->getType() == CONNECTION::UDP)
      delConnection((UdpConnection*) connection);
  }

  UdpConnection* findBySocketAddress(SocketAddress* addr)
  {
    for (size_t i = 0; i<connections.getCount(); i++)
    {
      UdpConnection* conn = connections.get(i);
      if (*conn->getSocketAddress() == *addr)
      {
        return conn;
      }
    }
    return 0;
  }
};

#endif // CONNECTIONMANAGER_H
