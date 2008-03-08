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
#include "server/entity/character.h"

#include "server/database/database.h"
#include "server/database/table-books.h"

void BookHandler::handleBookReadRequest(GenericMessage* msg)
{
  const Character* playerchar = NetworkHelper::getCharacter(msg);

  BookReadRequestMessage in_msg;
  in_msg.deserialise(msg->getByteStream());

  Server* server = Server::getServer();

  int itemId = in_msg.getItemId();
  int bookId = in_msg.getBookId();

  BooksTable* table = server->getDatabase()->getBooksTable();
  BooksTableVO* book = table->getById(bookId);

  if (!book)
  {
    // Item is no book!
    printf("Item %i is no book!\n", bookId);
    return;
  }

  unsigned char slot = playerchar->getInventory()->getSlot(itemId, bookId);

  if (slot == Inventory::NoSlot)
  {
    // Don't own book!
    printf("Player doesn't have book %i!\n", bookId);
    return;
  }

  BookReadResponseMessage out_msg;
  out_msg.setBookName(book->name);
  out_msg.setText(*book->text);

  ByteStream bs;
  out_msg.serialise(&bs);

  NetworkHelper::sendMessage(playerchar, bs);
}

void BookHandler::handleBookWriteRequest(GenericMessage* msg)
{
  const Character* playerchar = NetworkHelper::getCharacter(msg);

  BookWriteRequestMessage in_msg;
  in_msg.deserialise(msg->getByteStream());

  Server* server = Server::getServer();

  int itemId = in_msg.getItemId();
  int bookId = in_msg.getBookId();

  BooksTable* table = server->getDatabase()->getBooksTable();
  BooksTableVO* book = table->getById(bookId);

  if (!book)
  {
    // Item is no book!
    printf("Item %i is no book!\n", bookId);
    return;
  }
  else if (bookId > 0)
  {
    // Book is not emtpy!
    printf("Book %i is not emtpy!\n", bookId);
    return;
  }

  unsigned char slot = playerchar->getInventory()->getSlot(itemId, bookId);

  if (slot == Inventory::NoSlot)
  {
    // Don't own book!
    printf("Player doesn't have book %i!\n", bookId);
    return;
  }

  book->name = in_msg.getBookName();
  const char* text = in_msg.getText();
  book->text = ptString(text, strlen(text));

  book->id = table->getCount(book->itemId);
  table->insert(book);

  InventoryEntry item = *playerchar->getInventory()->getItem(slot);

  Character* pchar = playerchar->getLock();
  pchar->getInventory()->takeItem(slot);

  item.variation = book->id;

  pchar->getInventory()->addItem(item, slot);
  pchar->freeLock();

  BookWriteResponseMessage out_msg;
  out_msg.setBookId(book->id);

  ByteStream bs;
  out_msg.serialise(&bs);

  NetworkHelper::sendMessage(playerchar, bs);
}
