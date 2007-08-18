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

#include "network.h"
#include "networkhelper.h"

#include "server/entity/entitymanager.h"
#include "server/entity/pcentity.h"

void BookHandler::handleBookReadRequest(GenericMessage* msg)
{
  const Character* playerchar = NetworkHelper::getCharacter(msg);

  BookReadRequestMessage in_msg;
  in_msg.deserialise(msg->getByteStream());

  // -- INSERT IMPLEMENTATION HERE --

  // TODO: Temp implementation...
  const char* text = "There is no text in the book of the void!";
  ptString pttext(text, strlen(text));

  BookReadResponseMessage out_msg;
  out_msg.setText(pttext);

  ByteStream bs;
  out_msg.serialise(&bs);

  NetworkHelper::sendMessage(playerchar, bs);
}

void BookHandler::handleBookWriteRequest(GenericMessage* msg)
{
  const Character* playerchar = NetworkHelper::getCharacter(msg);

  BookWriteRequestMessage in_msg;
  in_msg.deserialise(msg->getByteStream());

  // -- INSERT IMPLEMENTATION HERE --

  BookWriteResponseMessage out_msg;
  out_msg.setBookId(123); // dummy implementation

  ByteStream bs;
  out_msg.serialise(&bs);

  NetworkHelper::sendMessage(playerchar, bs);
}
