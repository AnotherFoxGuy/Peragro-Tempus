#include "common/network/tcp/tcpsocket.h"
#include "tcpconnection.h"

void TcpConnection::send(const ByteStream& bs) const
{
  const char* data = (const char*) bs.getData();
  TcpSocket::publish(socket, data, (int) bs.getSize());
}
