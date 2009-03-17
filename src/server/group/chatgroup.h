/*
    Copyright (C) 2008 Development Team of Peragro Tempus

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

#ifndef CHATGROUP_H
#define CHATGROUP_H

#include "common/util/array.h"

class PcEntity;

class ChatGroup
{
protected:
  std::string name;

public:
  typedef Array<PcEntity*> UserList;

  ChatGroup(const char* channelname);
  virtual ~ChatGroup();

  const char* getName () const { return name.c_str(); }

  virtual const UserList& getUserList () const = 0;
  bool isUserIn (PcEntity* user) const;
  size_t getUserCount () const;

  virtual void delUser (PcEntity* user) = 0;
  // addUser is expected to gracefully handle duplicates
  virtual void addUser (PcEntity* user) = 0;

  // handle the given message, checking for control commands, and sending
  // the message out to the members of the group, if appropriate
  virtual void process(PcEntity* user, const char* message) = 0;

  // send the given message to all of the members of the group
  virtual void broadcast(PcEntity* user, const char* message) const = 0;
}; // class ChatGroup

#endif // CHATGROUP_H
