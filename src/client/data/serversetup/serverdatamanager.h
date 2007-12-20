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

#ifndef PT_SERVERDATAMANAGER_H
#define PT_SERVERDATAMANAGER_H

#include <vector>
#include <string>

namespace PT
{
  namespace Data
  {
    //Forward declarations
    class Server;

    /**
     * @ingroup data_handlers
     * Handles the loading of server setup data from XML definition files.
     */
    class ServerDataManager
    {
    private:

      ///Represents the parent directory of the Peragro Tempus data.
      std::string dataPath;

    public:
      /**
       * Base constructor that doesn't do anything at all.
       */
      ServerDataManager() {}
      /**
       * Convenience constructor allowing for immediate setting of data
       * directory path.
       * @param path Path to the data directory.
       */
      ServerDataManager(const std::string& path) : dataPath(path) {}
      ~ServerDataManager();

      /**
       * Set the data directory path used for loading the server data files.
       */
      void SetDataPath(const std::string& value) { dataPath = value; }
      /**
       * @return Data directory path.
       */
      const std::string& GetDataPath() const { return dataPath; }

      /**
       * Loads all the server information.
       * @return True if successful, false if an error occured.
       */
      bool LoadServerData();
    };
  } // Data namespace
} // PT namespace

#endif // PT_SERVERDATAMANAGER_H
