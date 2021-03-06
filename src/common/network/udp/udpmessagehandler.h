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

#ifndef UDPMESSAGEHANDLER_H
#define UDPMESSAGEHANDLER_H

#include "common/network/messagehandler.h"

class GenericMessage;

class UdpMessageHandler
{
public:
  virtual ~UdpMessageHandler() {}
  virtual void handle(GenericMessage* msg, SocketAddress& sock_addr) = 0;
  virtual char getType() = 0;
};

#endif // UDPMESSAGEHANDLER_H
