#ifndef _CLIENTCONNECTION_H_
#define _CLIENTCONNECTION_H_

#include "server/network/connection.h"

class TcpConnection : public Connection
{
  int socket;

/*
  const char* recvMessage()
  {
    int len = ::recv(socketId, mesg, 1024, 0);
    mesg[len] = '\0';
    return mesg;
  }
*/
public:
  TcpConnection(int socket) 
    : Connection(CONNECTION::TCP), socket(socket)
  {
  }

  virtual ~TcpConnection()
  {
  }

  void send(const ByteStream& bs) const;

  int getSocket() const { return socket; }
};

#endif // _CLIENTCONNECTION_H_
