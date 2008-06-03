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

#include "common/event/eventmanager.h"
#include "common/event/entityevent.h"

#include "common/reporter/reporter.h"

void DoorHandler::handleOpenDoorResponse(GenericMessage* msg)
{
  OpenDoorResponseMessage door_msg;
  door_msg.deserialise(msg->getByteStream());

  Report(PT::Debug, "EntityHandler: Got open door %d.", door_msg.getDoorId());
  if (!door_msg.getError().isNull())
  {
    Report(PT::Error, "Can't open %d! Reason: '%s'.", door_msg.getDoorId(), *door_msg.getError());
    return;
  }

  using namespace PT::Events;

  PT::Events::EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
  csRef<iEvent> entityEvent = evmgr->CreateEvent(EntityHelper::MakeEntitySpecific("entity.pcpropupdate", door_msg.getDoorId()), true);

  entityEvent->Add("entityId", door_msg.getDoorId());
  entityEvent->Add("pcprop", "Door Open");
  entityEvent->Add("celdata", true);

  evmgr->AddEvent(entityEvent);

}
void DoorHandler::handleCloseDoorResponse(GenericMessage* msg)
{
  CloseDoorResponseMessage door_msg;
  door_msg.deserialise(msg->getByteStream());

  Report(PT::Debug, "EntityHandler: Got close door %d.", door_msg.getDoorId());
  if (!door_msg.getError().isNull())
  {
    Report(PT::Error, "Can't close %d! Reason: '%s'.", door_msg.getDoorId(), *door_msg.getError());
    return;
  }

  using namespace PT::Events;

  PT::Events::EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
  csRef<iEvent> entityEvent = evmgr->CreateEvent(EntityHelper::MakeEntitySpecific("entity.pcpropupdate", door_msg.getDoorId()), true);

  entityEvent->Add("entityId", door_msg.getDoorId());
  entityEvent->Add("pcprop", "Door Open");
  entityEvent->Add("celdata", false);

  evmgr->AddEvent(entityEvent);
}

void DoorHandler::handleLockDoorResponse(GenericMessage* msg)
{
  LockDoorResponseMessage door_msg;
  door_msg.deserialise(msg->getByteStream());

  Report(PT::Debug, "EntityHandler: Got lock door %d.", door_msg.getDoorId());
  if (!door_msg.getError().isNull())
  {
    Report(PT::Error, "Can't lock %d! Reason: '%s'.", door_msg.getDoorId(), *door_msg.getError());
    return;
  }

  using namespace PT::Events;

  PT::Events::EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
  csRef<iEvent> entityEvent = evmgr->CreateEvent(EntityHelper::MakeEntitySpecific("entity.pcpropupdate", door_msg.getDoorId()), true);

  entityEvent->Add("entityId", door_msg.getDoorId());
  entityEvent->Add("pcprop", "Door Locked");
  entityEvent->Add("celdata", true);

  evmgr->AddEvent(entityEvent);
}

void DoorHandler::handleUnlockDoorResponse(GenericMessage* msg)
{
  UnlockDoorResponseMessage door_msg;
  door_msg.deserialise(msg->getByteStream());

  Report(PT::Debug, "EntityHandler: Got unlock door %d.", door_msg.getDoorId());
  if (!door_msg.getError().isNull())
  {
    Report(PT::Error, "Can't unlock %d! Reason: '%s'.", door_msg.getDoorId(), *door_msg.getError());
    return;
  }

  using namespace PT::Events;

  PT::Events::EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
  csRef<iEvent> entityEvent = evmgr->CreateEvent(EntityHelper::MakeEntitySpecific("entity.pcpropupdate", door_msg.getDoorId()), true);

  entityEvent->Add("entityId", door_msg.getDoorId());
  entityEvent->Add("pcprop", "Door Locked");
  entityEvent->Add("celdata", false);

  evmgr->AddEvent(entityEvent);
}
