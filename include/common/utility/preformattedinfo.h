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
 * @file preformattedinfo.h
 * @brief Macro to add exception info with Boost.Format, which is similar
 *   to printf.
 */

#ifndef PT_UTILITY_PREFORMATTEDINFO_H
#define PT_UTILITY_PREFORMATTEDINFO_H

#include <boost/format.hpp>

#include "exception.h"

#define PT_FORMAT_INFO(f, a) \
  ::PT::PreformattedInfo((::boost::format(f) a ).str())

namespace PT
{
  PT_DEFINE_ERRORINFO(Preformatted, std::string);
}

#endif
