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

#ifndef _SERIALISER_H_
#define _SERIALISER_H_

#include "bytestream.h"

class Serialiser
{
private:
  ByteStream* bs;

public:
  Serialiser(ByteStream* bs) : bs(bs) {}
  ~Serialiser() {}

  void setInt8(char value)
  {
    bs->data[bs->size] = value;
    bs->size++;
  }
  void setInt16(short value)
  {
    bs->data[bs->size]   = unsigned char( value & 0x00FF);
    bs->data[bs->size+1] = unsigned char((value & 0xFF00) / 0x100);
    bs->size += 2;
  }
  void setInt24(int value)
  {
    bs->data[bs->size]   = unsigned char( value & 0x0000FF);
    bs->data[bs->size+1] = unsigned char((value & 0x00FF00) /   0x100);
    bs->data[bs->size+2] = unsigned char((value & 0xFF0000) / 0x10000);
    bs->size += 4;
  }
  void setInt32(int value)
  {
    bs->data[bs->size]   = unsigned char( value & 0x000000FF);
    bs->data[bs->size+1] = unsigned char((value & 0x0000FF00) /     0x100);
    bs->data[bs->size+2] = unsigned char((value & 0x00FF0000) /   0x10000);
    bs->data[bs->size+3] = unsigned char((value & 0xFF000000) / 0x1000000);
    bs->size += 4;
  }
  void setFloat(float value)
  {
    setInt32(*(int*)(&value));
  }
  void setString(const char* value)
  {
    if (value == 0)
    {
      bs->data[bs->size] = 0;
      bs->size++;
    }
    else
    {
      bs->data[bs->size] = (char)strlen(value);
      strncpy((char*)bs->data+bs->size+1, value, strlen(value));
      *(bs->data+bs->size+strlen(value)+1) = 0;
      bs->size += strlen(value)+2;
    }
  }
};

#endif // _SERIALISER_H_
