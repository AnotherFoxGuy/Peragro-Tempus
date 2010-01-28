/*
  Copyright (C) 2010 Development Team of Peragro Tempus

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
    Size::Size()
    {
      valid = false;
    }

    Size::Size(unsigned int width, unsigned int height)
    {
      Size::height = height;
      Size::width = width;
      valid = true;
    }

    Size::~Size()
    {
    }

    bool Size::operator==(const Size& reference) const
    {
      return height == reference.height && width == reference.width;
    }

    bool Size::operator!=(const Size& reference) const
    {
      return !operator==(reference);
    }

    Size& Size::operator=(const Size& reference)
    {
      if (this == &reference)
      {
        return *this;
      }

      height = reference.height;
      width = reference.width;
      valid = reference.valid;
      return *this;
    }

    Size& Size::operator+=(const Size& reference)
    {
      height += reference.height;
      width += reference.width;
      return *this;
    }

    Size& Size::operator-=(const Size& reference)
    {

      if (reference.height <= height)
        height -= reference.height;
      else
        height = 0;

      if (reference.width <= width)
        width -= reference.width;
      else
        width = 0;

      return *this;
    }

  }
}
