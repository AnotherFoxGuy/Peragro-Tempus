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

#include <map>

#include "common/util/array.h"

class PcEntity;

class ChatGroups
{
public:
  typedef Array<const PcEntity*> UserList;
  typedef std::map< const char*, UserList > ChannelSet;

protected:
  ChannelSet channels;

public: 
  ChatGroups() : channels() {}
  ~ChatGroups() {}

  bool isUserIn (const PcEntity* user, const char* channel) const;
  bool channelExists (const char* channel) const;

  UserList getUserList (const char* channel) const;
  size_t getChannelCount() const { return channels.size(); }
  size_t getUserCount (const char* channel) const;

  void addUser (const PcEntity* user, const char* channel);
  void delUser (const PcEntity* user, const char* channel);

  void addChannel (const char* channel) { channels[channel]; }
  void delChannel (const char* channel) { channels.erase(channel); }
}; // class ChatGroups

#endif // CHATGROUP_H
