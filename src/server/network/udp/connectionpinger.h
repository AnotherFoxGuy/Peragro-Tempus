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

#ifndef _CONNECTIONPINGER_H_
#define _CONNECTIONPINGER_H_

#include "common/util/thread.h"
#include "common/util/sleep.h"
#include "common/network/messagehandler.h"
#include "server/network/udp/udpnetwork.h"
#include "server/network/udp/connectionmanager.h"

class UdpConnectionPinger : public Thread
{
private:
  UdpConnectionManager* conn_mgr;

  // Thread implementation
  void Run()
  {
    if (conn_mgr->getConnectionCount() == 0) sleep(1000);

    for (size_t i=0; i<conn_mgr->getConnectionCount(); i++)
    {
      //printf("Pinging Connection: %i\n", i);
      UdpConnection* connection = conn_mgr->getConnection(i);
      size_t last_seen = connection->sinceLastSeen();
      //printf("Connection last seen: %i ", last_seen);
      if (last_seen > 30)
      {
        printf("Connection %i timed out!\n", i);
        connection->peerLost();
        conn_mgr->delConnection(i);
      }
      else if (last_seen > 5)
      {
        //printf("Ping!\n");
        PingMessage ping_msg;
        ByteStream bs;
        ping_msg.serialise(&bs);
        connection->send(bs);
      }
      //else printf("\n");

      if (conn_mgr->getConnectionCount() != 0)
        sleep(5000/(int)conn_mgr->getConnectionCount());
    }
  }

public:
  UdpConnectionPinger(UdpConnectionManager* conn_mgr)
  : conn_mgr(conn_mgr)
  {
  }
};

#endif // _CONNECTIONPINGER_H_
