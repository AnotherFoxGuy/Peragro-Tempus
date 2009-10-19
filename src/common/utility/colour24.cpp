
/*
    Copyright (C) 2009 Development Team of Peragro Tempus

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

#include "common/utility/colour24.h"

#include <iostream>

namespace PT
{
  Colour24::Colour24()
    : red(0), green(0), blue(0)
  {
  }

  Colour24::Colour24(ColourType r, ColourType g, ColourType b)
    : red(r), green(g), blue(b)
  {
  }

  std::ostream& operator<<(std::ostream& os, const Colour24& colour)
  {
    os << static_cast<int>(colour.red) << ","
       << static_cast<int>(colour.green) << ","
       << static_cast<int>(colour.blue);
    return os;
  }

  std::istream& operator>>(std::istream& is, Colour24& colour)
  {
    int red = colour.red;
    int green = colour.green;
    int blue = colour.blue;
    is >> red; is.ignore(5, ',');
    is >> green; is.ignore(5, ',');
    is >> blue;
    colour.red = red;
    colour.green = green;
    colour.blue = blue;
    return is;
  }

} // PT namespace

