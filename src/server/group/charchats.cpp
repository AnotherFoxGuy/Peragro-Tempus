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

#include "charchats.h"

// TODO put this table in an XML file
const char* PTCharDefaultChatsTable[] = 
{
  "#general",  // server-wide mainchat
  "#auction",  // trade channel
  "#recruit",  // guild and party recruitment
  "#help",     // general game help
//  "#dev",      // for discussing the game's development
  "#gm"        // contact a game master
};

size_t CharChatsDef::GetDefChannelCount() const
{
  return sizeof(PTCharDefaultChatsTable) / sizeof(const char*);
}

const char* CharChatsDef::GetDefChannelName(size_t idx) const
{
  assert (idx < GetDefChannelCount());
  return PTCharDefaultChatsTable[idx];
}

void CharChatsDef::JoinChannel(const char* channel, const ChatGroup::UserList* ulist)
{
  // TODO
  if (!channel || !ulist) return;
  if (FindChannel(channel)) return;
  Channel newchannel(channel, ulist);
  channels.add(newchannel);
}

void CharChatsDef::QuitChannel(const char* channel)
{
  // TODO
  if (!channel) return;
  for (size_t i = 0;  i < channels.getCount();  i++)
    if (channels[i].first == channel)
    {
      channels.remove(i);
      break;
    }
}

const ChatGroup::UserList* CharChatsDef::FindChannel(const char* channel)
{
  if (!channel) return NULL;
  for (size_t i = 0;  i < channels.getCount();  i++)
    if (channels[i].first == channel) return channels[i].second;
  return NULL;
}
