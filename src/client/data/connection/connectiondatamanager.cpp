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

#include "connectiondatamanager.h"
#include "server.h"

#include "ext/tinyxml/tinyxml.h"

namespace PT
{
  namespace Data
  {

    ConnectionDataManager::~ConnectionDataManager()
    {
      for (size_t i = 0; i<servers.size(); i++) delete servers[i];
    }

    ///@internal Currently the servers vector is not preallocated, since there's
    ///no efficient way to get the count of servers described in XML file. If need
    ///be, we could traverse the XML file once before adding the actual data in
    ///order to determine the number of servers in file, and preallocate memory
    ///space.
    bool ConnectionDataManager::LoadServerData()
    {
      TiXmlDocument serverlist;
      if (!serverlist.LoadFile((dataPath+"/xml/servers.xml").c_str()))
      {
        printf("Failed to load serverlist.\n");
        return false;
      }

      TiXmlElement* serverNode=serverlist.FirstChildElement( "servers" );
      serverNode = serverNode->FirstChildElement( "server" );
      while (serverNode)
      {
        Server* server = new Server();
        server->SetName(serverNode->GetText());
        server->SetHost(serverNode->Attribute("host"));
        server->SetPort(serverNode->Attribute("port"));
        servers.push_back(server);
        serverNode = serverNode->NextSiblingElement("server");
      } // end while

      return true;
    } // end LoadServerData()

    Server* ConnectionDataManager::GetServerByName(const std::string& name) const
    {
      for (size_t i = 0; i<servers.size(); i++)
        if (servers[i]->GetName() == name) return servers[i];
      return 0;
    } // end GetServerByName()

    Server* ConnectionDataManager::GetServerById(unsigned int id) const
    {
      if (id<servers.size())
        return servers[id];
      return 0;
    } // end GetServerById()

  } // Data namespace
} // PT namespace
