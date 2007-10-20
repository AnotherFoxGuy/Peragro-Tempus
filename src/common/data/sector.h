/*
    Copyright (C) 2007 Development Team of Peragro Tempus

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

#ifndef PT_DATASECTOR_H
#define PT_DATASECTOR_H

#include <string>

namespace PT
{
  namespace Data
  {
    /**
      @ingroup data_handlers
      Helper class that contains basic information about sector. Usually not
      created directly, but accessed via SectorDataManager instance. Use the
      Set/Get methods to access the sector properties.
      @see SectorDataManager
      @author Branko Majic <branko.majic@gmail.com>
    */
    class Sector
    {
    private:
      ///Unique ID of the sector (ie '1').
      unsigned int id;
      ///Name of the sector (ie 'room').
      std::string name;

    public:
      Sector() : id(0) {}

      void SetId(unsigned int value) { id = value; }
      unsigned int GetId() const { return id; }

      void SetName(const std::string& value) { name = value; }
      const std::string& GetName() const { return name; }
    };
  } // Data namespace
} // PT namespace

#endif // PT_DATASECTOR_H
