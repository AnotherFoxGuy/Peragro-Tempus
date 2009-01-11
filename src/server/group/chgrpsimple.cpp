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

#include "chgrpsimple.h"

#include "server/entity/pcentity.h"
#include "server/network/networkhelper.h"
#include "common/network/chatmessages.h"
#include "common/util/ptstring.h"

ChatGroupSimple::ChatGroupSimple(const char* channelname) :
  ChatGroup(channelname), userlist()
{
}

ChatGroupSimple::~ChatGroupSimple()
{
}

const ChatGroup::UserList& ChatGroupSimple::getUserList () const
{
  return userlist;
}

void ChatGroupSimple::delUser (const PcEntity* user)
{
  size_t idx = userlist.find(user);
  if (idx != getUserCount()) userlist.remove(idx);
}

void ChatGroupSimple::addUser (const PcEntity* user)
{
  if (!isUserIn(user)) userlist.add(user);
}

void ChatGroupSimple::process(const PcEntity* user, const char* message)
{
  if (!user || !message) return;
  if (!isUserIn(user)) return;

  broadcast(user, message);
}

void ChatGroupSimple::broadcast(const PcEntity* user, const char* message) const
{
  if (!user || !message) return;

  GroupMessage out_msg;
  const ptString name = user->getEntity()->getName();
  const ptString channel = ptString::create(this->name);
  out_msg.setMessage(message);
  out_msg.setChannel(channel);
  out_msg.setSpeakerName(name);

  ByteStream bs;
  out_msg.serialise(&bs);

  for (unsigned i=0;  i<getUserCount();  i++)
    NetworkHelper::sendMessage(getUserList()[i], bs);
}
