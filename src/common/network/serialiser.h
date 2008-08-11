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

#ifndef SERIALISER_H
#define SERIALISER_H

#include "bytestream.h"
#include "common/util/ptstring.h"

class Serialiser
{
private:
  ByteStream* bs;

  bool valid;

public:
  Serialiser(ByteStream* bs) : bs(bs), valid(true)
  {
    bs->size = 2;
  }

  ~Serialiser()
  {
    bs->data[0]   = (unsigned char)(bs->size & 0x00FF);
    bs->data[1] = (unsigned char)((bs->size & 0xFF00) / 0x100);
  }

  bool isValid()
  {
    return valid;
  }

  void setInt8(char value)
  {
    if (bs->size + 1 >= MAX_STREAM_SIZE || valid == false)
    {
      valid = false;
      return;
    }
    bs->data[bs->size] = value;
    bs->size++;
  }
  void setInt16(short value)
  {
    if (bs->size + 2 >= MAX_STREAM_SIZE || valid == false)
    {
      valid = false;
      return;
    }
    bs->data[bs->size]   = (unsigned char)(value & 0x00FF);
    bs->data[bs->size+1] = (unsigned char)((value & 0xFF00) / 0x100);
    bs->size += 2;
  }
  void setInt24(int value)
  {
    if (bs->size + 3 >= MAX_STREAM_SIZE || valid == false)
    {
      valid = false;
      return;
    }
    bs->data[bs->size]   = (unsigned char)( value & 0x0000FF);
    bs->data[bs->size+1] = (unsigned char)((value & 0x00FF00) /   0x100);
    bs->data[bs->size+2] = (unsigned char)((value & 0xFF0000) / 0x10000);
    bs->size += 3;
  }
  void setInt32(int value)
  {
    if (bs->size + 4 >= MAX_STREAM_SIZE || valid == false)
    {
      valid = false;
      return;
    }
    bs->data[bs->size]   = (unsigned char)( value & 0x000000FF);
    bs->data[bs->size+1] = (unsigned char)((value & 0x0000FF00) /     0x100);
    bs->data[bs->size+2] = (unsigned char)((value & 0x00FF0000) /   0x10000);
    bs->data[bs->size+3] = (unsigned char)((value & 0xFF000000) / 0x1000000);
    bs->size += 4;
  }
  void setFloat(float value)
  {
    if (bs->size + 4 >= MAX_STREAM_SIZE || valid == false)
    {
      valid = false;
      return;
    }
    int temp = 0;
    memcpy(&temp, &value, sizeof(int));
    setInt32(temp);
  }
  void setString(ptString value)
  {
    const char* str = *value;
    if (str == 0)
    {
      if (bs->size + 1 >= MAX_STREAM_SIZE || valid == false)
      {
        valid = false;
        return;
      }
      bs->data[bs->size] = 0;
      bs->size++;
    }
    else
    {
      size_t len = strlen(str);

      if (bs->size + len >= MAX_STREAM_SIZE || len > 255 || valid == false)
      {
        valid = false;
        return;
      }
      bs->data[bs->size] = (unsigned char) len;
      strncpy((char*)bs->data + bs->size + 1, str, len);
      bs->data[bs->size + len + 1] = 0;
      bs->size += len + 2;
    }
  }
  void setString(const char* str)
  {
    if (str == 0 || strlen(str) == 0)
    {
      if (bs->size + 1 >= MAX_STREAM_SIZE || valid == false)
      {
        valid = false;
        return;
      }
      bs->data[bs->size] = 0;
      bs->size++;
    }
    else
    {
      size_t len = strlen(str);

      if (bs->size + len >= MAX_STREAM_SIZE || len > 255 || valid == false)
      {
        valid = false;
        return;
      }
      bs->data[bs->size] = (unsigned char) len;
      strncpy((char*)bs->data + bs->size + 1, str, len);
      bs->data[bs->size + len + 1] = 0;
      bs->size += len + 2;
    }
  }
};

#endif // SERIALISER_H
