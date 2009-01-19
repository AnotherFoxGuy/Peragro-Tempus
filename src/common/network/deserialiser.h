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

#ifndef DESERIALISER_H
#define DESERIALISER_H

#include <string.h>

#include "bytestream.h"
#include "common/util/ptstring.h"

class Deserialiser
{
private:
  const unsigned char* data;
  size_t len;
  size_t pos;

public:
  Deserialiser(ByteStream* bs) : data(bs->getData()), len(bs->getSize()), pos(2) {}
  ~Deserialiser() {}

  unsigned short getSize()
  {
    short value = data[0];
    value += data[1] * 0x100;
    return value;
  }

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
    float value;
    memcpy((void*)&value, (void*)&data[pos], (size_t)sizeof(float));
    pos += sizeof(float)/sizeof(unsigned char);
    return value;
  }
  const ptString getString()
  {
    size_t size = strnlen((const char*) data + pos, MAX_STREAM_SIZE);
    if ( size == 0 || size >= MAX_STREAM_SIZE || data[pos+size] != 0 )
    {
      pos++;
      ptString ret_str(0,0);
      return ret_str;
    }
    const char* str = (const char*) data + pos;
    pos += size + 1;
    ptString ret_str(str, size);
    return ret_str;
  }
  size_t getString(const char*& str)
  {
    size_t size = strnlen((const char*) data + pos, MAX_STREAM_SIZE);
    if ( size == 0 || size >= MAX_STREAM_SIZE || data[pos+size] != 0 )
    {
      pos++;
      str = 0;
      return 0;
    }
    str = (char*) data + pos;
    pos += size + 1;
    return size;
  }

// Strnlen is a gnu extension that certain versions of OSX do not support.
#if defined(CS_PLATFORM_MACOSX)
  int strnlen(const char *c, int len)
  {
    int n = 0;
    while (*c++ && n < len)
      n++;
    return (n);
  }
#endif
};

#endif // DESERIALISER_H
