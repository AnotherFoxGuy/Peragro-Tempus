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
#include <wfmath/axisbox.h>
#include <wfmath/rotmatrix.h>
#include <wfmath/stream.h>

namespace DB
{
  //----------------------------------------------------------------
  namespace Helper
  {
    inline std::string Quote(const std::string& val) { return "\""+val+"\""; }

    template<typename T>
    inline const char* DBType() { return ""; }
    template<> inline const char* DBType<size_t>() { return "INTEGER"; }
    template<> inline const char* DBType<float>() { return "FLOAT"; }
    template<> inline const char* DBType<bool>() { return "BOOLEAN"; }
    template<> inline const char* DBType<std::string>() { return "TEXT"; }
    template<> inline const char* DBType<WFMath::Point<2> >() { return "TEXT"; }
    template<> inline const char* DBType<WFMath::Point<3> >() { return "TEXT"; }
    template<> inline const char* DBType<WFMath::AxisBox<3> >() { return "TEXT"; }
    template<> inline const char* DBType<WFMath::RotMatrix<3> >() { return "TEXT"; }

    template<typename T>
    inline void Convert(T& val, const std::string& str) { val = str; }
    template<> inline void Convert(size_t& val, const std::string& str) { val = atoi(str.c_str()); }
    template<> inline void Convert(float& val, const std::string& str) { val = atof(str.c_str()); }
    template<> inline void Convert(bool& val, const std::string& str) { std::stringstream ss(str); ss >> val; }
    // std::string by default template.
    template<> inline void Convert(WFMath::Point<2>& val, const std::string& str) { std::stringstream ss(str); ss >> val; }
    template<> inline void Convert(WFMath::Point<3>& val, const std::string& str) { std::stringstream ss(str); ss >> val; }
    template<> inline void Convert(WFMath::AxisBox<3>& val, const std::string& str) { std::stringstream ss(str); ss >> val; }
    template<> inline void Convert(WFMath::RotMatrix<3>& val, const std::string& str) { std::stringstream ss(str); ss >> val; }

    template<typename T>
    inline std::string Convert(const T& val) { return val; }
    template<> inline std::string Convert(const size_t& val) { std::stringstream o; o << val; return o.str(); }
    template<> inline std::string Convert(const float& val) { std::stringstream o; o << val; return o.str(); }
    template<> inline std::string Convert(const bool& val) { std::stringstream o; o << val; return o.str(); }
    template<> inline std::string Convert(const std::string& val) { return Quote(val); }
    template<> inline std::string Convert(const WFMath::Point<2>& val) { std::stringstream o; o << val; return Quote(o.str()); }
    template<> inline std::string Convert(const WFMath::Point<3>& val) { std::stringstream o; o << val; return Quote(o.str()); }
    template<> inline std::string Convert(const WFMath::AxisBox<3>& val) { std::stringstream o; o << val; return Quote(o.str()); }
    template<> inline std::string Convert(const WFMath::RotMatrix<3>& val) { std::stringstream o; o << val; return Quote(o.str()); }
  };
  //----------------------------------------------------------------

} // namespace DB

#endif // FIELDHELPER_H
