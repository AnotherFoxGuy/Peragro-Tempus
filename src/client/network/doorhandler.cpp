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
#include "client/event/entityevent.h"

void DoorHandler::handleOpenDoorResponse(GenericMessage* msg)
{
  OpenDoorResponseMessage door_msg;
  door_msg.deserialise(msg->getByteStream());

  printf("EntityHandler: Got open door %d.\n", door_msg.getDoorId());
  if (!door_msg.getError().isNull())
  {
    printf("E: Can't open %d! Reason: '%s'\n", door_msg.getDoorId(), *door_msg.getError());
    return;
  }

	using namespace PT::Events;
  EntityPcPropUpdateEvent* entityEvent = new EntityPcPropUpdateEvent();

	celData data;
	data.Set(true);

	entityEvent->entityId			= door_msg.getDoorId();
	entityEvent->pcprop				= "Door Open";
	entityEvent->celdata			= data;

	PointerLibrary::getInstance()->getEventManager()->AddEvent(entityEvent);
}
void DoorHandler::handleCloseDoorResponse(GenericMessage* msg)
{
  CloseDoorResponseMessage door_msg;
  door_msg.deserialise(msg->getByteStream());

	printf("EntityHandler: Got close door %d.\n", door_msg.getDoorId());
	if (!door_msg.getError().isNull())
	{
		printf("E: Can't close %d! Reason: '%s'\n", door_msg.getDoorId(), *door_msg.getError());
		return;
	}

	using namespace PT::Events;
  EntityPcPropUpdateEvent* entityEvent = new EntityPcPropUpdateEvent();

	celData data;
	data.Set(false);

	entityEvent->entityId			= door_msg.getDoorId();
	entityEvent->pcprop				= "Door Open";
	entityEvent->celdata			= data;

	PointerLibrary::getInstance()->getEventManager()->AddEvent(entityEvent);
}

void DoorHandler::handleLockDoorResponse(GenericMessage* msg)
{
  // TODO: implement
}

void DoorHandler::handleUnlockDoorResponse(GenericMessage* msg)
{
  // TODO: implement
}
