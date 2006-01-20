/*
    Copyright (C) 2005 Development Team of Peragro Tempus

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

#ifndef _USERACCOUNTMANAGER_H_
#define _USERACCOUNTMANAGER_H_

#include <string>

class Server;
class User;

class UserAccountManager
{
private:
  Server* server;

public:
  UserAccountManager(Server* server) : server(server) {}
  const char* login(const std::string& username, const std::string& password, User*& user);
  const char* signup(const std::string& username, const std::string& password);
};

#endif // _USERACCOUNTMANAGER_H_
