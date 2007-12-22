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

#ifndef PT_DATASERVER_H
#define PT_DATASERVER_H

#include <string>

namespace PT
{
  namespace Data
  {
    /**
     * @ingroup data_handlers
     * Helper class that contains basic information about a server. Usually not
     * created directly, but accessed via ConnectionDataManager instance. Use the
     * Set/Get methods to access the server properties.
     * @see ConnectionDataManager
     */
    class Server
    {
    private:
      ///Name of the server.
      std::string name;
      ///Host of the server.
      std::string host;
      ///Port of the server.
      int port;

    public:
      Server() {}

      void SetName(const std::string& value) { name = value; }
      const std::string& GetName() const { return name; }

      void SetHost(const std::string& value) { host = value; }
      const std::string& GetHost() const { return host; }

      void SetPort(const int& value) { port = value; }
      const int& GetPort() const { return port; }
    };
  } // Data namespace
} // PT namespace

#endif // PT_DATASERVER_H
