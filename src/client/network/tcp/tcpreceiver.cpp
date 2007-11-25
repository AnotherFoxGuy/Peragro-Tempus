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

#include "tcpreceiver.h"

#include "common/network/includes.h"

void Receiver::Run()
{
  buffer.setSize(TcpSocket::receive(socket->getSocket(), (char*)buffer.getData() + buffer.getSize(), buffer.getMaxSize()));
  if (buffer.getSize() == 0) return;

  printf("--[NW]-- Received %d bytes\n", buffer.getSize());
  while (true)
  {
    GenericMessage message(&buffer);

    if (buffer.getSize() < message.getSize()) break;

    handlers->handle(&message, socket->getSocket());

    if (buffer.getSize() == message.getSize()) break;
    printf("--[NW]-- Message length %d bytes, remaining %d bytes\n", message.getSize(), buffer.getSize() - message.getSize());

    unsigned char* data = (unsigned char*) buffer.getData();
    memmove(data, data + message.getSize(), buffer.getSize() - message.getSize());
    buffer.setSize(buffer.getSize() - message.getSize());
  }
}
