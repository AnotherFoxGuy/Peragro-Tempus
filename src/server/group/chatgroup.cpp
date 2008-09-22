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

#include "chatgroup.h"

bool ChatGroups::isUserIn (const PcEntity* user, const char* channel) const
{
  ChatGroups::ChannelSet::const_iterator iter = channels.find(channel);
  if (iter == channels.end()) return false;
  return iter->second.find(user) != iter->second.getCount();
}

bool ChatGroups::channelExists (const char* channel) const
{
  return channels.find(channel) != channels.end();
}

const ChatGroups::UserList& ChatGroups::getUserList (const char* channel) const
{
  static UserList emptylist = UserList();
  ChatGroups::ChannelSet::const_iterator iter = channels.find(channel);
  if (iter == channels.end()) return emptylist;
  return iter->second;
}

size_t ChatGroups::getUserCount (const char* channel) const
{
  ChatGroups::ChannelSet::const_iterator iter = channels.find(channel);
  if (iter == channels.end()) return 0;
  return iter->second.getCount();
}

void ChatGroups::addUser (const PcEntity* user, const char* channel)
{
  UserList& list = channels[channel];
  if (list.find(user) == list.getCount()) list.add(user);
}

void ChatGroups::delUser (const PcEntity* user, const char* channel, bool prune)
{
  ChatGroups::ChannelSet::iterator iter = channels.find(channel);
  if (iter == channels.end()) return;
  size_t idx = iter->second.find(user);
  if (idx != iter->second.getCount()) iter->second.remove(idx);
  if (prune && !iter->second.getCount()) delChannel(channel);
}
