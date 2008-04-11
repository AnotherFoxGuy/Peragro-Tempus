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

#include <cssysdef.h>
#include <iutil/document.h>

#include "client/reporter/reporter.h"
#include "client/pointer/pointer.h"
#include "connectiondatamanager.h"
#include "server.h"

namespace PT
{
  namespace Data
  {

    ConnectionDataManager::ConnectionDataManager(PointerLibrary* ptrlib) : DataManager(ptrlib)
    {
      file = "/peragro/xml/servers.xml";
    }

    ConnectionDataManager::~ConnectionDataManager()
    {
      for (size_t i = 0; i<servers.size(); i++) delete servers[i];
    }

    bool ConnectionDataManager::parseElement(iDocumentNode* node)
    {
      Server* server = new Server();
      server->SetName(node->GetContentsValue());
      server->SetHost(node->GetAttributeValue("host"));
      server->SetPort(node->GetAttributeValue("port"));
      servers.push_back(server);

      return true;
    } // end parseElement()

    Server* ConnectionDataManager::GetServerByName(const std::string& name) const
    {
      for (size_t i = 0; i < servers.size(); i++)
      {
        if (servers[i]->GetName() == name)
          return servers[i];
      } // end for

      return 0;
    } // end GetServerByName()

    Server* ConnectionDataManager::GetServerById(unsigned int id) const
    {
      if (id < servers.size())
        return servers[id];
      return 0;
    } // end GetServerById()

  } // Data namespace
} // PT namespace
