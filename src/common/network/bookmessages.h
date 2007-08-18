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

#ifndef _BOOKMESSAGES_H_
#define _BOOKMESSAGES_H_

#include "netmessage.h"

namespace BOOK
{
  enum MESSAGES
  {
    BOOKREADREQUEST=0,
    BOOKREADRESPONSE=1,
    BOOKWRITEREQUEST=2,
    BOOKWRITERESPONSE=3
  };
}

class BookReadRequestMessage : public NetMessage
{
  unsigned int itemid;
  unsigned int bookid;

public:
  BookReadRequestMessage() : NetMessage(MESSAGES::BOOK,BOOK::BOOKREADREQUEST)
  {
  }

  ~BookReadRequestMessage()
  {
  }

  void serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  unsigned int getItemId() { return itemid; }
  void setItemId(unsigned int x) { itemid = x; }

  unsigned int getBookId() { return bookid; }
  void setBookId(unsigned int x) { bookid = x; }

};

class BookReadResponseMessage : public NetMessage
{
  const char* text;

public:
  BookReadResponseMessage() : NetMessage(MESSAGES::BOOK,BOOK::BOOKREADRESPONSE)
  {
  }

  ~BookReadResponseMessage()
  {
  }

  void serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  const char* getText() { return text; }
  void setText(const char* x) { text = x; }

};

class BookWriteRequestMessage : public NetMessage
{
  unsigned int itemid;
  unsigned int bookid;
  ptString bookname;
  const char* text;

public:
  BookWriteRequestMessage() : NetMessage(MESSAGES::BOOK,BOOK::BOOKWRITEREQUEST)
  {
  }

  ~BookWriteRequestMessage()
  {
  }

  void serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  unsigned int getItemId() { return itemid; }
  void setItemId(unsigned int x) { itemid = x; }

  unsigned int getBookId() { return bookid; }
  void setBookId(unsigned int x) { bookid = x; }

  ptString getBookName() { return bookname; }
  void setBookName(ptString x) { bookname = x; }

  const char* getText() { return text; }
  void setText(const char* x) { text = x; }

};

class BookWriteResponseMessage : public NetMessage
{
  ptString error;

public:
  BookWriteResponseMessage() : NetMessage(MESSAGES::BOOK,BOOK::BOOKWRITERESPONSE)
  {
  }

  ~BookWriteResponseMessage()
  {
  }

  void serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  ptString getError() { return error; }
  void setError(ptString x) { error = x; }

};

#endif // _BOOKMESSAGES_H_
