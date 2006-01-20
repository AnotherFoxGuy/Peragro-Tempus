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

#ifndef _CONNECTIONMANAGER_H_
#define _CONNECTIONMANAGER_H_

#include <vector>

#include "udpconnection.h"

class UdpConnectionManager
{
private:
  std::vector<UdpConnection*> connections;

public:
  UdpConnectionManager() {}

  size_t getConnectionCount() const
  {
    return connections.size();
  }

  UdpConnection* getConnection(size_t index) const
  {
    return connections[index];
  }

  void addConnection(UdpConnection* connection)
  {
    connections.push_back(connection);
  }

  void delConnection(size_t index)
  {
    connections.erase(connections.begin() + index);
  }

  void delConnection(UdpConnection* connection)
  {
    for (size_t i = 0; i < connections.size(); i++)
    {
      UdpConnection* conn = connections[i];
      if (*conn->getSocketAddress() == *connection->getSocketAddress())
      {
        connections.erase(connections.begin() + i);
      }
    }
  }

  void delConnection(Connection* connection)
  {
    if (connection->getType() == CONNECTION::UDP)
      delConnection((UdpConnection*) connection);
  }

  UdpConnection* findBySocketAddress(SocketAddress* addr) const
  {
    for (size_t i = 0; i < connections.size(); i++)
    {
      UdpConnection* conn = connections[i];
      if (*conn->getSocketAddress() == *addr)
      {
        return conn;
      }
    }
    return 0;
  }
};

#endif // _CONNECTIONMANAGER_H_

