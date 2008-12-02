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

#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H

#include "auth/network/connection.h"

class TcpConnection : public Connection
{
  int socket;
  ByteStream buffer;

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

  void peerLost();

  ByteStream& getBuffer() { return buffer; }
};

#endif // CLIENTCONNECTION_H
