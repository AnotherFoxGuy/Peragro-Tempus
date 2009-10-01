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

#include <fstream>

class nwNetwork;
class nwType;
class nwPeer;
class nwParams;

class nwWriter
{
  nwNetwork* nw;

  std::string toConst(std::string str);
  std::string toFunction(std::string str);
  std::string toVariable(std::string str);
  void writeLicenceHeader(std::ofstream& out);
  void writeParamDeclaration(std::ofstream& out, nwParams* param);
  void writeParamSerialisation(std::ofstream& out, nwParams* param);
  void writeParamDeserialisation(std::ofstream& out, nwParams* param);
  void writeParamGetterSetter(std::ofstream& out, nwParams* param);
  void writeParamListDefinition(std::ofstream& out, nwParams* param);
  void writeParamListSerialisation(std::ofstream& out, std::string listname, nwParams* param);
  void writeParamListDeserialisation(std::ofstream& out, std::string listname, nwParams* param);
  void writeParamListGetterSetter(std::ofstream& out, std::string listname, nwParams* param);

  void writeParam(std::ofstream& out, nwParams* param, const std::string& eventname, const std::string& arg, size_t indent);
  void writeServerParam(std::ofstream& out, nwParams* param, const std::string& eventname, const std::string& arg, size_t indent);
  std::string toGetFunction(std::string str, std::string arg ="");

public:
  nwWriter(nwNetwork* nw);

  void writeNetwork(std::ofstream& out);
  void writeTypeHead(std::ofstream& out, nwType* type);
  void writeTypeImpl(std::ofstream& out, nwType* type);
  void writeHandler(std::ofstream& out, nwPeer* peer, nwType* type);
  void writeHandlerImplementation(std::ofstream& out, nwPeer* peer, nwType* type);
  void writeServerHandlerImplementation(std::ofstream& out, nwPeer* peer, nwType* type);
};
