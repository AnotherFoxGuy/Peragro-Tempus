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

#ifdef WIN32
  #include <winsock.h>
  #define socklen int
#else
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <unistd.h>
  #include <netdb.h>
  #include <sys/ioctl.h>
  #include <errno.h>
  #define socklen socklen_t
#endif

#include "udpsocket.h"

Socket::Socket()
{
#ifdef WIN32    
  WSADATA info;
  WSAStartup(MAKEWORD(1,1), &info);
#endif
  ready = false;
}

void Socket::init(unsigned short port, unsigned ip)
{
  socket_handler = (int) socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
#ifdef WIN32
  long error = WSAGetLastError();
  if (error != 0)
  {
      printf("Error at NetSocket::init(...): error code: %d\n", error);
  }
#endif

  sockaddr_in addr;
  memset(&addr,0,sizeof(sockaddr_in));

  addr.sin_family=AF_INET;
  addr.sin_port=htons(port);
  addr.sin_addr.s_addr=htonl(ip);

  bind(socket_handler,(sockaddr*)&addr,sizeof(sockaddr_in));
#ifdef WIN32
  error = WSAGetLastError();
  if (error != 0)
  {
      printf("Error at NetSocket::init(...): error code: %d\n", error);
  }
#endif
  ready = true;
}

Socket::~Socket()
{
#ifdef WIN32
  closesocket(socket_handler);
  //WSACleanup();
#else
  close(socket_handler);
#endif
}

int Socket::receive(char* msg, size_t length, SocketAddress* sockaddr)
{
  sockaddr_in addr;
  socklen len = sizeof(addr);
  int real_length = recvfrom(socket_handler, msg, (int)length, 0, (struct sockaddr *)&addr, &len);

  sockaddr->port = ntohs(addr.sin_port);
  sockaddr->ip = ntohl(addr.sin_addr.s_addr);

  if (ready && real_length < 0)
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
      printf("Error at NetSocket::receive(...): error code: %d\n", error);
    }
  }

  if (real_length > 0)
  {
    received_bytes += real_length;
    return real_length;
  }

  return 0;
}

bool Socket::publish(const char* msg, size_t length, const SocketAddress* sockaddr)
{
  sockaddr_in addr;
  memset(&addr,0,sizeof(sockaddr_in));
  addr.sin_family=AF_INET;
  addr.sin_port=htons(sockaddr->port);
  addr.sin_addr.s_addr=htonl(sockaddr->ip);

  int rv = sendto(socket_handler, msg, (int)length, 0,
                 (struct sockaddr*)&addr, (int)sizeof(addr));

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

SocketAddress Socket::getSocketAddress(const char* host, unsigned short port)
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

void Socket::getNetworkStats(size_t& sentbytes, size_t& recvbytes, size_t& lastchecked)
{
  sentbytes = sent_bytes;
  sent_bytes = 0;
  recvbytes = received_bytes;
  received_bytes = 0;
  lastchecked = last_checked;
  last_checked = (size_t)time(0);
}
