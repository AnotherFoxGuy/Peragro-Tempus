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

#ifndef CHGRPSIMPLE_H
#define CHGRPSIMPLE_H

#include "chatgroup.h"

class ChatGroupSimple : public ChatGroup
{
protected:
  UserList userlist;

public:
  ChatGroupSimple(const char* channelname);
  virtual ~ChatGroupSimple();

  virtual const UserList& getUserList () const;

  virtual void delUser (PcEntity* user);
  virtual void addUser (PcEntity* user);

  virtual void process(PcEntity* user, const char* message);
  virtual void broadcast(PcEntity* user, const char* message) const;
}; // class ChatGroupSimple

#endif // CHGRPSIMPLE_H
