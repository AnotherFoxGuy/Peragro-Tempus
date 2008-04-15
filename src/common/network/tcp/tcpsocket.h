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

#ifndef SOCKET_H
#define SOCKET_H

#include <stdlib.h>

class SocketAddress
{
public:
  unsigned int ip;
  unsigned short port;

  SocketAddress() {}
  virtual ~SocketAddress() {}

  bool operator== (const SocketAddress& o) const 
  {
    return (ip == o.ip && port == o.port);
  }
};

class TcpSocket
{
private:
  bool ready;
  int socket_handler;

  static size_t sent_bytes;
  static size_t received_bytes;
  static size_t last_checked;

public:
  TcpSocket();
  ~TcpSocket();

  bool init(unsigned short port, unsigned ip = 0, bool server = true);
  void kill();

  static int receive(const int socket, char* msg, size_t len);
  static bool publish(const int socket, const char* msg, size_t len);
  static const int* select(const int* sockets, size_t len, size_t& len_out);

  static SocketAddress getSocketAddress(const char* host, unsigned short port);

  void getNetworkStats(size_t& sentbytes, size_t& recvbytes, size_t& lastchecked);

  int getSocket() { return socket_handler; }
};

#endif // SOCKET_H
