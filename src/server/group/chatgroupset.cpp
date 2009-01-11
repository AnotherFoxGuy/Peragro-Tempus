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

#include "chatgroupset.h"

const ChatGroup* ChatGroupSet::FindChannel (const char* channel) const
{
  if (!channel) return NULL;
  ChatGroupSet::ChannelSet::const_iterator iter = channels.find(channel);
  if (iter == channels.end()) return NULL;
  return iter->second.get();
}

ChatGroup* ChatGroupSet::FindChannel (const char* channel)
{
  if (!channel) return NULL;
  ChatGroupSet::ChannelSet::iterator iter = channels.find(channel);
  if (iter == channels.end()) return NULL;
  return iter->second.get();
}

bool ChatGroupSet::isUserIn (const PcEntity* user, const char* channel) const
{
  if (!channel) return false;
  const ChatGroup* ch = FindChannel(channel);
  return (ch) ? ch->isUserIn(user) : false;
}

bool ChatGroupSet::channelExists (const char* channel) const
{
  if (!channel) return false;
  return ( FindChannel(channel) != NULL );
}

const ChatGroup::UserList& ChatGroupSet::getUserList (const char* channel) const
{
  static ChatGroup::UserList emptylist = ChatGroup::UserList();
  if (!channel) return emptylist;
  const ChatGroup* ch = FindChannel(channel);
  return (ch) ? ch->getUserList() : emptylist;
}

size_t ChatGroupSet::getUserCount (const char* channel) const
{
  if (!channel) return 0;
  const ChatGroup* ch = FindChannel(channel);
  return (ch) ? ch->getUserCount() : 0;
}

bool ChatGroupSet::addUser (const PcEntity* user, const char* channel)
{
  if (channelExists(channel))
  {
    boost::shared_ptr<ChatGroup> group = channels[channel];
    if (group) group->addUser(user);
    return true;
  }
  return false;
}

void ChatGroupSet::delUser (const PcEntity* user, const char* channel, bool prune)
{
  if (!channel) return;
  ChatGroup* ch = FindChannel(channel);
  if (ch == NULL) return
  ch->delUser(user);
  if (prune && !ch->getUserCount()) delChannel(channel);
}

void ChatGroupSet::addChannel (ChatGroup* group)
{
  if (dynamic_cast<ChatGroup*> (group) == NULL) return;
  const char* channel = group->getName();
  boost::shared_ptr<ChatGroup> gptr(group);
  if (channel) channels[channel] = gptr;
}
