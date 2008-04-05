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

void BookHandler::handleBookReadResponse(GenericMessage* msg)
{
  BookReadResponseMessage bookmsg;
  bookmsg.deserialise(msg->getByteStream());

  PT::Events::EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
  csRef<iEvent> bookEvent = evmgr->CreateEvent("book.read", true);
  bookEvent->Add("title", *bookmsg.getBookName());
  bookEvent->Add("text", bookmsg.getText());
  evmgr->AddEvent(bookEvent);
}

void BookHandler::handleBookWriteResponse(GenericMessage* msg)
{
  BookWriteResponseMessage bookmsg;
  bookmsg.deserialise(msg->getByteStream());

  PT::Events::EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
  csRef<iEvent> bookEvent = evmgr->CreateEvent("book.write", true);
  bookEvent->Add("variationId",  bookmsg.getBookId());
  evmgr->AddEvent(bookEvent);
}
