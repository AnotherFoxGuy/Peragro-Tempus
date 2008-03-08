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

class nwNetwork;

class nwFactory
{
  nwNetwork* nw;

public:
  nwFactory();
  ~nwFactory();

  void createNetwork(int version, int subversion, int microversion);
  void createType(unsigned int id, const char* type);
  void createMessage(unsigned int id, const char* name, const char* typeName);
  void createPeer(const char* name);
  void createPeerSendMsg(const char* peerName, const char* msgName);
  void createPeerReceiveMsg(const char* peerName, const char* msgName);
  void createMessageParameter(const char* msgName, const char* paramName, const char* type);
  void createListParameter(const char* msgName, const char* listName, const char* paramName, const char* type);

  nwNetwork* getNetwork();
};
