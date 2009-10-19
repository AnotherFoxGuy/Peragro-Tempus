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
 * @file exception.h
 * @brief Exception creation utility functions.
 */

#ifndef PT_UTILITY_EXCEPTION_H
#define PT_UTILITY_EXCEPTION_H

#include <boost/exception.hpp>
#include <boost/current_function.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor/cat.hpp>
#include <exception>

#define PT_ECE(t) \
  ::boost::enable_current_exception(t)

#define PT_EX(t) \
  PT_ECE(t) \
    << ::boost::throw_function(BOOST_CURRENT_FUNCTION) \
    << ::boost::throw_file(__FILE__) \
    << ::boost::throw_line((int)__LINE__)

#define PT_DEFINE_EXCEPTION_WHAT(n, s) \
  struct n : public virtual ::std::exception, \
    public virtual ::boost::exception \
  { \
    const char* const message; \
    n() : message(BOOST_PP_STRINGIZE(s)) {} \
    n(const char* message) : message(message) {} \
    virtual const char* what() const throw() { return message; } \
  }

#define PT_DEFINE_EXCEPTION(n) \
  PT_DEFINE_EXCEPTION_WHAT(n, n)

#define PT_BOOST_ERRORINFO(n, t) \
  ::boost::error_info<struct BOOST_PP_CAT(ErrorInfo_, n), t>

#define PT_DEFINE_ERRORINFO(n, t) \
  typedef PT_BOOST_ERRORINFO(n, t) BOOST_PP_CAT(n, Info)

namespace PT
{
  PT_DEFINE_EXCEPTION(Exception);
}

#endif
