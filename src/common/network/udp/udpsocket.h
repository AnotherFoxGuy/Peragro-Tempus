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

#ifndef _SOCKET_H_
#define _SOCKET_H_

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

class Socket
{
private:
  bool ready;
  int socket_handler;

  size_t sent_bytes;
  size_t received_bytes;
  size_t last_checked;

public:
  Socket();
  ~Socket();

  void init(unsigned short port, unsigned ip = 0);
  void kill();

  int receive(char* msg, size_t len, SocketAddress* addr);
  bool publish(const char* msg, size_t len, const SocketAddress* addr);

  static SocketAddress getSocketAddress(const char* host, unsigned short port);

  void getNetworkStats(size_t& sentbytes, size_t& recvbytes, size_t& lastchecked);
};

#endif // _SOCKET_H_
