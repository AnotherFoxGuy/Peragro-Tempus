#ifndef _CLIENTCONNECTIONMANAGER_H_
#define _CLIENTCONNECTIONMANAGER_H_

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

#endif // _CLIENTCONNECTIONMANAGER_H_