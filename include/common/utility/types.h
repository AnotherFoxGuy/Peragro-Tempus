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
 * @file types.h
 *
 * @basic Import boost types into the PT namespace.
 */

#ifndef PT_UTILITY_TYPES_H
#define PT_UTILITY_TYPES_H

#include <boost/cstdint.hpp>

namespace PT
{
  using boost::int8_t;
  using boost::uint8_t;
  using boost::int16_t;
  using boost::uint16_t;
  using boost::int32_t;
  using boost::uint32_t;
  using boost::int64_t;
  using boost::uint64_t;
  using boost::intmax_t;
  using boost::uintmax_t;

} // PT namespace

#endif
