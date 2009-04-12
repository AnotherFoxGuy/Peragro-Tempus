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

#include "exception.h"
#include <sstream>
#include <string>

namespace PT
{
  std::string ExceptionInfo::Format(const boost::exception& ex)
  {
    std::ostringstream info;
    if (const std::exception* stdEx = dynamic_cast<const std::exception*>(&ex))
    {
      info << stdEx->what();
    }

    if (const char* infoStr =
      boost::exception_detail::get_diagnostic_information(ex))
    {
      if (*infoStr)
      {
        info << "\n" << infoStr;
      }
    }

    if (boost::shared_ptr<const char* const> file =
      boost::get_error_info<boost::throw_file>(ex))
    {
      info << "\n" << *file;
      if (boost::shared_ptr<const int> line =
        boost::get_error_info<boost::throw_line>(ex))
      {
        info << ":" << *line;
      }
    }
    if (boost::shared_ptr<const char* const> function =
      boost::get_error_info<boost::throw_function>(ex))
    {
      info << "\n" << *function;
    }
    else
    {
      info << "\n(unknown)";
    }

    info << "\nException type id: " << BOOST_EXCEPTION_DYNAMIC_TYPEID(ex).name();

    return info.str();
  }

}

