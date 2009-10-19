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

#include <iosfwd>

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

    /// Get red component cast to int.
    int Red() const;
    /// Get green component cast to int.
    int Green() const;
    /// Get blue component cast to int.
    int Blue() const;

    /// Red component.
    ColourType red;
    /// Green component.
    ColourType green;
    /// Blue component.
    ColourType blue;
  };

  /// Output streaming from Colour24.
  std::ostream& operator<<(std::ostream& os, const Colour24& colour);
  /// Input streaming into Colour24.
  std::istream& operator>>(std::istream& is, Colour24& colour);

  inline int Colour24::Red() const
  { return red; }

  inline int Colour24::Green() const
  { return green; }

  inline int Colour24::Blue() const
  { return blue; }

} // PT namespace

#endif
