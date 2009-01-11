/*
    Copyright (C) 2007-2008 Development Team of Peragro Tempus

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

#include "vector3.h"

#include <sstream>

namespace Geom
{

  std::istream& operator>>(std::istream& is, Vector3& v)
  {
    is.setf(std::ios_base::skipws);
    is >> v.x;
    is.ignore(256, ',');
    is >> v.y;
    is.ignore(256, ',');
    is >> v.z;
    return is;
  }

  std::ostream& operator<<(std::ostream& os, const Vector3& v)
  {
    os.unsetf(std::ios_base::showpoint);
    os.unsetf(std::ios_base::floatfield);
    os.precision(6);
    os << v.x;
    os << ",";
    os << v.y;
    os << ",";
    os << v.z;
    return os;
  }

  std::string Vector3::ToString() const
  {
    std::stringstream ss;
    ss << (*this);
    return ss.str();
  } // end ToString()

} // namespace Geom

