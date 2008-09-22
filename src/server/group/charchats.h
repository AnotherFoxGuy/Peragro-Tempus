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

#ifndef CHARCHATS_H
#define CHARCHATS_H

#include <utility>
#include <string>

#include "common/util/array.h"
#include "common/util/monitorable.h"

#include "chatgroup.h"

class CharChats : public ptMonitorable<CharChats>
{
public:
  typedef std::pair< std::string, const ChatGroups::UserList* > Channel;

  CharChats() {}
  virtual ~CharChats() {}

  virtual size_t GetChannelCount() const = 0;
  virtual const char* GetChannelName (size_t idx) const = 0;

  // names of default channels to join apon creation
  virtual size_t GetDefChannelCount() const = 0;
  virtual const char* GetDefChannelName (size_t idx) const = 0;

  virtual void JoinChannel(const char* channel, const ChatGroups::UserList* ulist) = 0;

}; // class CharChats

class CharChatsDef : public CharChats
{
protected:
  Array< Channel > channels;
  const PcEntity* parent;

public:
  CharChatsDef(const PcEntity* p) : CharChats(), channels(), parent(p) {}
  virtual ~CharChatsDef() {}

  virtual size_t GetChannelCount() const { return channels.getCount(); };
  virtual const char* GetChannelName (size_t idx) const
  { return channels.get(idx).first.c_str(); }

  virtual size_t GetDefChannelCount() const;
  virtual const char* GetDefChannelName (size_t idx) const;

  virtual void JoinChannel(const char* channel, const ChatGroups::UserList* ulist);

  const PcEntity* GetParent() const { return parent; }
}; // class CharChatsDef

#endif // CHARCHATS_H
