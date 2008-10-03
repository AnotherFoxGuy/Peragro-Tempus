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

#include <string>

class TiXmlNode;
class TiXmlElement;

class nwFactory;

class nwXmlParser
{
private:
  nwFactory* factory;

  void parseNetwork(TiXmlElement* parentNode);
  void parsePeers(TiXmlElement* parentNode);
  void parsePeer(TiXmlElement* parentNode);
  void parseMessageDefinitions(TiXmlElement* parentNode);
  void parseMsgDefType(TiXmlElement* parentNode);
  void parseMessageImplementations(TiXmlElement* parentNode);
  void parseMessageImplementation(TiXmlElement* parentNode);
  void parseEvent(TiXmlElement* parentNode);
  void parseParams(TiXmlElement* parentNode, const char* msgName, const char* eventName, const char* entitySpecific);
  void parseParamsList(TiXmlElement* parentNode, const char* msgName, const char* listName);

  // for temporary storage between the functions
  std::string fileroot;

public:
  nwXmlParser(nwFactory* factory);
  ~nwXmlParser();

  bool parse(const char* filename);
};


