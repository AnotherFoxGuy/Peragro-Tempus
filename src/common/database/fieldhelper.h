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

#ifndef FIELDHELPER_H
#define FIELDHELPER_H

// Helpers
#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/preprocessor/comparison/equal.hpp>
#include <sstream>
#include <istream>

// Types
#include <string>
#include <wfmath/point.h>
#include <wfmath/stream.h>

namespace DB
{
  //----------------------------------------------------------------
  namespace Helper
  {
    static std::string Quote(const std::string& val) { return "\""+val+"\""; }

    template<typename T>
    static const char* DBType() { return ""; }
    template<> const char* DBType<size_t>() { return "INTEGER"; }
    template<> const char* DBType<float>() { return "FLOAT"; }
    template<> const char* DBType<bool>() { return "BOOLEAN"; }
    template<> const char* DBType<std::string>() { return "TEXT"; }
    template<> const char* DBType<WFMath::Point<3> >() { return "TEXT"; }

    template<typename T>
    static void Convert(T& val, const std::string& str) { val = str; }
    template<> void Convert(size_t& val, const std::string& str) { val = atoi(str.c_str()); }
    template<> void Convert(float& val, const std::string& str) { val = atof(str.c_str()); }
    template<> void Convert(bool& val, const std::string& str) { std::stringstream ss(str); ss >> val; }
    // std::string by default template.
    template<> void Convert(WFMath::Point<3>& val, const std::string& str) { std::stringstream ss(str); ss >> val; }

    template<typename T>
    static std::string Convert(T val) { return val; }
    template<> std::string Convert(size_t val) { std::stringstream o; o << val; return o.str(); }
    template<> std::string Convert(float val) { std::stringstream o; o << val; return o.str(); }
    template<> std::string Convert(bool val) { std::stringstream o; o << val; return o.str(); }
    template<> std::string Convert(std::string val) { return Quote(val); }
    template<> std::string Convert(WFMath::Point<3> val) { std::stringstream o; o << val; return Quote(o.str()); }
  };
  //----------------------------------------------------------------

} // namespace DB

#endif // FIELDHELPER_H
