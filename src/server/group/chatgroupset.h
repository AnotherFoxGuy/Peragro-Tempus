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

#ifndef CHATGROUPSET_H
#define CHATGROUPSET_H

#include <map>
#include <string>
#include <boost/shared_ptr.hpp>

#include "chatgroup.h"

class PcEntity;

class ChatGroupSet
{
public:
  typedef std::map< std::string, boost::shared_ptr<ChatGroup> > ChannelSet;

protected:
  ChannelSet channels;

  const ChatGroup* FindChannel (const char* channel) const;
  ChatGroup* FindChannel (const char* channel);

public: 
  ChatGroupSet() : channels() {}
  ~ChatGroupSet() {}

  bool isUserIn (const PcEntity* user, const char* channel) const;
  bool channelExists (const char* channel) const;

  const ChatGroup::UserList& getUserList (const char* channel) const;
  size_t getChannelCount() const { return channels.size(); }
  size_t getUserCount (const char* channel) const;

  /* the 'prune' option determines whether a channel should be pruned
     from the list if deleting the user leaves the channel empty of users. */
  void delUser (const PcEntity* user, const char* channel, bool prune=true);
  /* add a user to a channel, *only* if it exists already.  Return true
     if the user was added or already present, false otherwise. */
  bool addUser (const PcEntity* user, const char* channel);

  void addChannel (ChatGroup* group);
  void delChannel (const char* channel) { channels.erase(channel); }
  ChatGroup* getChannel (const char* channel) { return FindChannel(channel); }
}; // class ChatGroupSet

#endif // CHATGROUPSET_H
