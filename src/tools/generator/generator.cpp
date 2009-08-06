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

#include "generator.h"

#include "nwfactory.h"
#include "nwxmlparser.h"
#include "nwwriter.h"
#include "network.h"

#include <fstream>
#include <algorithm>
#include <iostream>

bool Generator::generateNetwork()
{
  nwFactory factory;
  nwXmlParser parser(&factory);

  if (!parser.parse("data/generate/network/netmessage.xml"))
    return false;

  nwNetwork* network = factory.getNetwork();

  std::ofstream nw_file("src/common/network/nwtypes.h");
  std::cout << "- writing src/common/network/nwtypes.h\n";

  nwWriter writer(network);
  writer.writeNetwork(nw_file);

  for ( size_t i=1; i<network->types.size(); i++)
  {
    std::string file("src/common/network/");
    std::string filename = network->types[i]->name;
    std::transform(filename.begin(), filename.end(), filename.begin(), tolower);
    file.append(filename);
    file.append("messages.h");
    std::ofstream type_file(file.c_str());
    std::cout << "- writing " << file <<"\n";
    writer.writeTypeHead(type_file, network->types[i]);
  }

  for ( size_t i=1; i<network->types.size(); i++)
  {
    std::string file("src/common/network/");
    std::string filename = network->types[i]->name;
    std::transform(filename.begin(), filename.end(), filename.begin(), tolower);
    file.append(filename);
    file.append("messages.cpp");
    std::ofstream type_file(file.c_str());
    std::cout << "- writing " << file <<"\n";
    writer.writeTypeImpl(type_file, network->types[i]);
  }

  for ( size_t i=0; i<network->peers.size(); i++)
  {
    for ( size_t j=1; j<network->types.size(); j++)
    {
      bool hasMessages = false;
      for (size_t x = 0; x < network->peers[i]->recvMsg.size(); x++)
      {
        nwMessage* msg = network->peers[i]->recvMsg[x];
        if (!msg || msg->type != network->types[j])
          continue;
        else
        {
          hasMessages = true;
          break;
        }
      }
      if (!hasMessages) continue;

      std::string file("src/");
      std::string pathname = network->peers[i]->name;
      std::transform(pathname.begin(), pathname.end(), pathname.begin(), tolower);
      file.append(pathname);
      file.append("/network/");
      std::string filename = network->types[j]->name;
      std::transform(filename.begin(), filename.end(), filename.begin(), tolower);
      file.append(filename);
      file.append("handler.h");
      std::ofstream type_file(file.c_str());
      std::cout << "- writing " << file <<"\n";
      writer.writeHandler(type_file, network->peers[i], network->types[j]);
    }
  }

  // EXPERIMENTAL
  nwPeer* client = network->getPeer("Client");
  for ( size_t j=1; j<network->types.size(); j++)
  {
    std::string file("src/");
    std::string pathname = client->name;
    std::transform(pathname.begin(), pathname.end(), pathname.begin(), tolower);
    file.append(pathname);
    file.append("/network/");
    std::string filename = network->types[j]->name;
    std::transform(filename.begin(), filename.end(), filename.begin(), tolower);
    file.append(filename);
    file.append("handler.cpp");
    std::ofstream type_file(file.c_str());
    std::cout << "- writing " << file <<"\n";
    writer.writeHandlerImplementation(type_file, client, network->types[j]);
  }

  return true;
}
