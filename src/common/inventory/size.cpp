/*
  Copyright (C) 2007 - 2008 Development Team of Peragro Tempus

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

#include "size.h"

namespace Common
{
  namespace Inventory
  {

    Size::Size(int height, int width)
    {
      Size::height = height;
      Size::width = width;
    }

    Size::~Size()
    {
    }

    bool Size::operator==(const Size& size) const
    {
      return height == size.height && width == size.width;
    }

    bool Size::operator!=(const Size& size) const
    {
      return !operator==(size);
    }

    bool Size::operator<(const Size& size) const
    {
      return height < size.height && width < size.width;
    }

    bool Size::operator<=(const Size& size) const
    {
      return height <= size.height && width <= size.width;
    }

    Size& Size::operator=(const Size& size)
    {
      if (this == &size)
      {
        return *this;
      }

      height = size.height;
      width = size.width;
      return *this;
    }

    Size& Size::operator+=(const Size& size)
    {
      height += size.height;
      width += size.width;
      return *this;
    }

    Size& Size::operator-=(const Size& size)
    {

      if (size.height < height)
        height -= size.height;
      else
        height = 1;

      if (size.width < width)
        width -= size.width;
      else
        width = 1;

      return *this;
    }

  }
}
