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

#ifndef BYTESTREAM_H
#define BYTESTREAM_H

#define MAX_STREAM_SIZE 32768

class ByteStream
{
  friend class Serialiser;
  friend class Deserialiser;

private:
  unsigned char data[MAX_STREAM_SIZE];
  size_t size;

public:
  ByteStream() : size(0) {}
  ~ByteStream() {}

  size_t getSize() const
  {
    return size;
  }

  size_t getMaxSize() const
  {
    return MAX_STREAM_SIZE;
  }

  void setSize(size_t s)
  {
    size = s;
  }

  const unsigned char* getData() const
  {
    return data;
  }

  void clear()
  {
    size = 0;
  }

};

#endif // BYTESTREAM_H
