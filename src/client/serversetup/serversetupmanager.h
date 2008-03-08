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

#ifndef PT_SERVERSETUPMANAGER_H
#define PT_SERVERSETUPMANAGER_H

#include <cssysdef.h>

#include <vector>
#include <string>

#include <csutil/ref.h>
#include <iutil/vfs.h>

namespace PT
{
  namespace Data
  {
    class DoorDataManager;
    class ItemDataManager;
    class NpcDataManager;
    class SpawnPointDataManager;
  } // Data namespace

  namespace Misc
  {
    //Forward declaration
    class Network;

    /**
     * @ingroup data_handlers
     * Handles the loading of server setup data from XML definition files.
     */
    class ServerSetupManager
    {
    private:
      PT::Data::DoorDataManager* doorDataManager;
      PT::Data::ItemDataManager* itemDataManager;
      PT::Data::NpcDataManager* npcDataManager;
      PT::Data::SpawnPointDataManager* spawnpointDataManager;

    public:
      PT::Data::ItemDataManager* GetItemDataManager() { return itemDataManager; } // TODO remove

    public:
      /**
       * Base constructor
       */
      ServerSetupManager();
      ~ServerSetupManager();

      /**
       * Loads all the server information and sends it to the server.
       * @return True if successful, false if an error occured.
       */
      bool UploadServerData();

      /**
       * Receives the server information from the server and saves it to files.
       * @return True if successful, false if an error occured.
       */
      bool DownloadServerData();
    };
  } // Data namespace
} // PT namespace

#endif // PT_SERVERSETUPMANAGER_H
