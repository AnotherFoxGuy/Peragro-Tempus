/*
    Copyright (C) 2005 Development Team of Peragro Tempus

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

#include <string>
#include <vector>
#include "common/util/array.h"

/// contains the return value of a sql query
class ResultSet
{
private:
  ///array of rows where as each row is an array of strings.
  std::vector<std::vector<std::string>> data;
public:
  ResultSet() {}
  ResultSet(const ResultSet& o)
  { 
    //colname = o.colname;
    data = o.data;
  }

  ~ResultSet()
  {
  }

  size_t GetRowCount() { return data.size(); }
  size_t GetColCount() { return data[0].size(); }

  void AddData(size_t row, size_t col, const char* string)
  {
    if (data.size() <= row)
    {
      data.resize(data.size() + 1);
    }

    if (data[row].size() <= col)
    {
      data[row].resize(data[row].size() + 1);
    }
    data[row][col] = string?string:"";
  }

  std::string GetData(size_t row, size_t col)
  {
    return data[row][col];
  }
};
