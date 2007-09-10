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

#include "client/network/network.h"

#include "client/event/eventmanager.h"
#include "client/event/stateevent.h"

#include "client/reporter/reporter.h"

#include <iutil/eventq.h>
#include "client/client.h"

void ConnectionHandler::handleConnectionResponse(PT::Client *client, GenericMessage* msg)
{
  Report(PT::Notify, "Received ConnectionResponse.");
  ConnectResponseMessage connect_msg(0);
  connect_msg.deserialise(msg->getByteStream());
  if (connect_msg.connectionSucceeded())
  {
    using namespace PT::Events;
    StateConnectedEvent* stateEvent = new StateConnectedEvent();
    PointerLibrary::getInstance()->getEventManager()->AddEvent(stateEvent);
  }
  else
  {
    printf("Client is to old\n");
    csRef<iEventQueue> q = csQueryRegistry<iEventQueue> (client->GetObjectRegistry());
    if (q.IsValid()) q->GetEventOutlet()->Broadcast(csevQuit(client->GetObjectRegistry()));
  }
}

void ConnectionHandler::handlePing(GenericMessage* ping_msg)
{
  PongMessage pong_msg;
  network->send(&pong_msg);
  Report(PT::Debug, "Received Ping.");
}
