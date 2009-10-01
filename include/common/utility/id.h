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
 * @file id.h
 *
 * @basic Wrapper for ID numbers.
 */

#ifndef PT_UTILITY_ID_H
#define PT_UTILITY_ID_H

#include <iostream>

#include <boost/operators.hpp>

#include "common/utility/types.h"

namespace PT
{
  namespace Utility
  {
    /**
     * Wrapper for ID numbers. T is a 'tag' type, to make comparisons only
     *   valid with the same tagged type.
     */
    template<typename T, typename V = uint32_t, V Invalid = 0>
    class Id : public boost::totally_ordered<Id<T,V,Invalid> >
    {
    public:
      typedef V ValueType;

      /// Default constructor, sets the value to Invalid.
      Id() : value(Invalid) {}
      /// Constructor.
      Id(ValueType v) : value(v) {}

      /// Check validity.
      bool IsValid() const { return value != Invalid; }
      /// Mark invalid.
      void Invalidate() { value = Invalid; }

      /// The value.
      ValueType value;
    };

    template<typename T, typename V, V Invalid>
    inline bool operator==(const Id<T,V,Invalid>& lhs, const Id<T,V,Invalid>& rhs)
    { return lhs.value == rhs.value; }

    template<typename T, typename V, V Invalid>
    inline bool operator<(const Id<T,V,Invalid>& lhs, const Id<T,V,Invalid>& rhs)
    { return lhs.value < rhs.value; }

    template<typename T, typename V, V Invalid>
    inline std::istream& operator>>(std::istream& is, Id<T,V,Invalid>& id)
    { is >> id.value; return is; }

    template<typename T, typename V, V Invalid>
    inline std::ostream& operator<<(std::ostream& os, const Id<T,V,Invalid>& id)
    { os << id.value; return os; }

  }
}

#endif
