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
  struct Helper
  {
    static std::string Quote(const std::string& val) { return "\""+val+"\""; }

    template<typename T>
    static const char* DBType() { return ""; }
    template<> static const char* DBType<size_t>() { return "INTEGER"; }
    template<> static const char* DBType<float>() { return "FLOAT"; }
    template<> static const char* DBType<bool>() { return "BOOLEAN"; }
    template<> static const char* DBType<std::string>() { return "TEXT"; }
    template<> static const char* DBType<WFMath::Point<3> >() { return "TEXT"; }

    template<typename T>
    static void Convert(T& val, const std::string& str) { val = str; }
    template<> static void Convert(size_t& val, const std::string& str) { val = atoi(str.c_str()); }
    template<> static void Convert(float& val, const std::string& str) { val = atof(str.c_str()); }
    template<> static void Convert(bool& val, const std::string& str) { std::istringstream(str) >> val; }
    // std::string by default template.
    template<> static void Convert(WFMath::Point<3>& val, const std::string& str) { std::istringstream(str) >> val; }

    template<typename T>
    static std::string Convert(T val) { return val; }
    template<> static std::string Convert(size_t val) { std::stringstream o; o<<val; std::string s(o.str()); return s; }
    template<> static std::string Convert(float val) { std::stringstream o; o<<val; std::string s(o.str()); return s; }
    template<> static std::string Convert(bool val) { std::stringstream o; o<<val; std::string s(o.str()); return s; }
    template<> static std::string Convert(std::string val) { return Quote(val); }
    template<> static std::string Convert(WFMath::Point<3> val) { std::stringstream o; o<<val; std::string s(o.str()); return Quote(s); }
  };
  //----------------------------------------------------------------

} // namespace DB

#endif // FIELDHELPER_H
