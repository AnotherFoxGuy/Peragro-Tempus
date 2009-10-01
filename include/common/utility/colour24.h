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
/**
 * @file colour24.h
 *
 * @basic Wrapper around a 24 bit colour.
 */

#ifndef PT_UTILITY_COLOUR24_H
#define PT_UTILITY_COLOUR24_H

#include <iostream>

#include "common/utility/types.h"

namespace PT
{
  /**
   * Wrapper around a 24 bit colour.
   */
  class Colour24
  {
  public:
    typedef uint8_t ColourType;

    /// Default constructor, initialises to "0, 0, 0"
    Colour24();
    /// Constructor.
    Colour24(ColourType r, ColourType g, ColourType b);

    /// Red component.
    ColourType red;
    /// Green component.
    ColourType green;
    /// Blue component.
    ColourType blue;
  };

  inline std::ostream& operator<<(std::ostream& os, const Colour24& colour)
  { os << colour.red << "," << colour.green << "," << colour.blue; return os; }

  inline std::istream& operator>>(std::istream& is, Colour24& colour)
  { is >> colour.red; is.ignore(5, ','); is >> colour.green;
    is.ignore(5, ','); is >> colour.blue; return is; }

  inline Colour24::Colour24()
    : red(0), green(0), blue(0) {}

  inline Colour24::Colour24(ColourType r, ColourType g, ColourType b)
    : red(r), green(g), blue(b) {}

} // PT namespace

#endif
