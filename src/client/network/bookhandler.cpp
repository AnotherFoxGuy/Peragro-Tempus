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

#include "client/network/bookhandler.h"

#include "common/event/eventmanager.h"
#include "common/event/entityevent.h"

#include "client/pointer/pointer.h"

void BookHandler::handleBookReadResponse(GenericMessage* msg)
{
  BookReadResponseMessage pmsg;
  pmsg.deserialise(msg->getByteStream());

  using namespace PT::Events;
  EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
  {
    csRef<iEvent> pEvent = evmgr->CreateEvent("book.read", true);
    pEvent->Add("bookName", *pmsg.getBookName()?*pmsg.getBookName():"");
    pEvent->Add("text", pmsg.getText());

    evmgr->AddEvent(pEvent);
  }

} // end handleBookReadResponse

void BookHandler::handleBookWriteResponse(GenericMessage* msg)
{
  BookWriteResponseMessage pmsg;
  pmsg.deserialise(msg->getByteStream());

  using namespace PT::Events;
  EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
  {
    csRef<iEvent> pEvent = evmgr->CreateEvent("book.write", true);
    pEvent->Add("bookId", pmsg.getBookId());
    pEvent->Add("error", *pmsg.getError()?*pmsg.getError():"");

    evmgr->AddEvent(pEvent);
  }

} // end handleBookWriteResponse

