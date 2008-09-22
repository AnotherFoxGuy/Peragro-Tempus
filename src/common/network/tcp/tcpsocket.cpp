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

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "common/network/includes.h"

#include "tcpsocket.h"
#include "common/util/printhelper.h"

size_t TcpSocket::sent_bytes;
size_t TcpSocket::received_bytes;
size_t TcpSocket::last_checked;


TcpSocket::TcpSocket()
{
#ifdef WIN32
  WSADATA info;
  WSAStartup(MAKEWORD(1,1), &info);
  printf("Max number of sockets: %d\n", info.iMaxSockets);
#endif
  ready = false;

  sent_bytes = 0;
  received_bytes = 0;
  last_checked = (size_t)time(0);
}

bool TcpSocket::init(unsigned short port, unsigned ip, bool server)
{
  socket_handler = (int) socket(AF_INET, SOCK_STREAM, 0);

  int flag = 1;
  setsockopt(socket_handler, IPPROTO_TCP, TCP_NODELAY, (const char*) &flag, sizeof(int));
  setsockopt(socket_handler, SOL_SOCKET, SO_REUSEADDR, (const char*) &flag, sizeof(int));

  struct sockaddr_in addr;

  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(ip);
  addr.sin_port = htons(port);

  if (server)
  {
    if (bind(socket_handler, (struct sockaddr *)&addr, sizeof(struct sockaddr)) == -1)
    {
        perror("bind");
        return false;
    }

    if (listen(socket_handler, 10) == -1)
    {
        perror("listen");
        return false;
    }
  }
  else
  {
    if (connect(socket_handler, (struct sockaddr *)&addr, sizeof(struct sockaddr)) == -1)
    {
        perror("connect");
        return false;
    }
  }
  return true;
}

TcpSocket::~TcpSocket()
{
  kill();
}

void TcpSocket::kill()
{
  ready = false;
#ifdef WIN32
  closesocket(socket_handler);
  //WSACleanup();
#else
  close(socket_handler);
#endif
}

int TcpSocket::receive(const int socket, char* msg, size_t length)
{
  int real_length = recv(socket, msg, (int)length, 0);

  if (real_length < 0)
  {
    size_t error = errno;
#ifdef WIN32
    error = (size_t)WSAGetLastError();
    if (error == WSAEWOULDBLOCK) return -1;
#else
    if (error == EAGAIN) return -1;
#endif
    if (error != 0)
    {
      printf("Error at NetSocket::receive(...): error code: %" SIZET "\n",
             error);
    }
  }

  if (real_length > 0)
  {
    received_bytes += real_length;
    return real_length;
  }

  return 0;
}

bool TcpSocket::publish(const int socket, const char* msg, size_t length)
{
  int rv = send(socket, msg, (int)length, 0);

#ifdef WIN32
  long error = WSAGetLastError();
  if (error != 0)
  {
      printf("Error at NetSocket::publish(...): error code: %d\n", error);
  }
#endif

  if (rv > 0)
  {
    sent_bytes += rv;
  }

  return rv != -1;
}

SocketAddress TcpSocket::getSocketAddress(const char* host, unsigned short port)
{
  SocketAddress addr;
  hostent* h = gethostbyname(host);
  if (h)
    addr.ip = ntohl(*(int*)h->h_addr);
  else
    addr.ip = 0;
  addr.port = port;
  return addr;
}

const int* TcpSocket::select(const int* sockets, size_t len, size_t& len_out)
{
  fd_set socks;
  FD_ZERO(&socks);

  for (size_t i = 0; i < len; i++)
  {
    FD_SET(sockets[i], &socks);
  }

  len_out = ::select(FD_SETSIZE, &socks, 0, 0, 0);

  size_t ready_socks_count = 0;
  int* ready_socks = new int[len_out];
  for (size_t i = 0; i < len; i++)
  {
    if (FD_ISSET(sockets[i], &socks))
    {
      ready_socks[ready_socks_count] = sockets[i];
      ready_socks_count++;
    }
  }

  return ready_socks;
}

void TcpSocket::getNetworkStats(size_t& sentbytes, size_t& recvbytes, size_t& lastchecked)
{
  sentbytes = sent_bytes;
  sent_bytes = 0;
  recvbytes = received_bytes;
  received_bytes = 0;
  lastchecked = last_checked;
  last_checked = (size_t)time(0);
}
