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

#ifndef _DESERIALISER_H_
#define _DESERIALISER_H_

#include "bytestream.h"
#include "common/util/ptstring.h"

class Deserialiser
{
private:
  const unsigned char* data;
  size_t len;
  size_t pos;

public:
  Deserialiser(ByteStream* bs) : data(bs->getData()), len(bs->getSize()), pos(0) {}
  ~Deserialiser() {}

  char getInt8()
  {
    return data[pos++];
  }

  short getInt16()
  {
    short value = data[pos];
    value += data[pos+1] * 0x100;
    pos += 2;
    return value;
  }
  int getInt24()
  {
    int value = data[pos];
    value += data[pos+1] *   0x100;
    value += data[pos+2] * 0x10000;
    pos += 3;
    return value;
  }
  int getInt32()
  {
    int value = data[pos];
    value += data[pos+1] *     0x100;
    value += data[pos+2] *   0x10000;
    value += data[pos+3] * 0x1000000;
    pos += 4;
    return value;
  }
  float getFloat()
  {
    int value = getInt32();
    return *(float*)(&value);
  }
  const ptString getString()
  {
    unsigned char size = data[pos];
    if ( size == 0 || data[pos+size+1] != 0 )
    {
      pos++;
      ptString ret_str(0,0);
      return ret_str;
    }
    const char* str = (const char*) data + pos + 1;
    pos += size + 2;
    ptString ret_str(str, size);
    return ret_str;
  }
  int getString(const char*& str)
  {
    unsigned char size = data[pos];
    if ( size == 0 || data[pos+size+1] != 0 )
    {
      pos++;
      str = 0;
      return 0;
    }
    str = (char*) data + pos + 1;
    pos += size + 2;
    return size;
  }
};

#endif // _DESERIALISER_H_
