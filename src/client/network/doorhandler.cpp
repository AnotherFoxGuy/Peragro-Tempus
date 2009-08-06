/*
    Copyright (C) 2008 Development Team of Peragro Tempus

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

//  !! This file was automatically generated by a network code generator   !!
//  !! Do not change this file since all changes will be overwritten later !!
//  !! Instead please change the source files here: peragro/data/generate  !!

#include "client/network/doorhandler.h"

#include "common/event/eventmanager.h"
#include "common/event/entityevent.h"

#include "client/pointer/pointer.h"

void DoorHandler::handleOpenDoorResponse(GenericMessage* msg)
{
  OpenDoorResponseMessage pmsg;
  pmsg.deserialise(msg->getByteStream());

  using namespace PT::Events;
  EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
  {
    csRef<iEvent> pEvent = evmgr->CreateEvent(EntityHelper::MakeEntitySpecific("entity.door.open", pmsg.getDoorId()), true);
    pEvent->Add("doorId", pmsg.getDoorId());
    pEvent->Add("error", *pmsg.getError()?*pmsg.getError():"");

    evmgr->AddEvent(pEvent);
  }

} // end handleOpenDoorResponse

void DoorHandler::handleCloseDoorResponse(GenericMessage* msg)
{
  CloseDoorResponseMessage pmsg;
  pmsg.deserialise(msg->getByteStream());

  using namespace PT::Events;
  EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
  {
    csRef<iEvent> pEvent = evmgr->CreateEvent(EntityHelper::MakeEntitySpecific("entity.door.close", pmsg.getDoorId()), true);
    pEvent->Add("doorId", pmsg.getDoorId());
    pEvent->Add("error", *pmsg.getError()?*pmsg.getError():"");

    evmgr->AddEvent(pEvent);
  }

} // end handleCloseDoorResponse

void DoorHandler::handleLockDoorResponse(GenericMessage* msg)
{
  LockDoorResponseMessage pmsg;
  pmsg.deserialise(msg->getByteStream());

  using namespace PT::Events;
  EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
  {
    csRef<iEvent> pEvent = evmgr->CreateEvent(EntityHelper::MakeEntitySpecific("entity.door.lock", pmsg.getDoorId()), true);
    pEvent->Add("doorId", pmsg.getDoorId());
    pEvent->Add("error", *pmsg.getError()?*pmsg.getError():"");

    evmgr->AddEvent(pEvent);
  }

} // end handleLockDoorResponse

void DoorHandler::handleUnlockDoorResponse(GenericMessage* msg)
{
  UnlockDoorResponseMessage pmsg;
  pmsg.deserialise(msg->getByteStream());

  using namespace PT::Events;
  EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
  {
    csRef<iEvent> pEvent = evmgr->CreateEvent(EntityHelper::MakeEntitySpecific("entity.door.unlock", pmsg.getDoorId()), true);
    pEvent->Add("doorId", pmsg.getDoorId());
    pEvent->Add("error", *pmsg.getError()?*pmsg.getError():"");

    evmgr->AddEvent(pEvent);
  }

} // end handleUnlockDoorResponse

