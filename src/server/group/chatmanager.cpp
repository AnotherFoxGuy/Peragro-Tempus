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

#include "chatmanager.h"
#include "chgrpsimple.h"

#include "server/entity/entity.h"
#include "server/entity/pcentity.h"
#include "server/entity/entitymanager.h"
#include "server/server.h"

ChatManager::ChatManager() : ChatGroupSet()
{
}

ChatManager::~ChatManager()
{
  if (Server::getServer() && Server::getServer()->getEntityManager())
    Server::getServer()->getEntityManager()->RemoveEntityCallback(this);
}

ChatManager* ChatManager::getChatManager()
{
  static ChatManager* chatmanager = new ChatManager();
  static bool initialized = false;

  // register a callback with the EntityManager for entity add/remove
  if (!initialized && Server::getServer() && Server::getServer()->getEntityManager())
  {
    Server::getServer()->getEntityManager()->AddEntityCallback(chatmanager);
    initialized = true;
  }

  return chatmanager;
}

void ChatManager::createChannel(const char* channel)
{
  if (!channel) return;
  ChatGroup* chgrp = new ChatGroupSimple(channel);
  addChannel(chgrp);
}

void ChatManager::OnEntityAdd(Common::Entity::Entityp entity)
{
  /*
  if (!entity) return;
  const PcEntity* user;
  if ( !(user = entity->getPlayerEntity()) ) return;
  const CharChats* c_cchats = user->getCharChats();
  if (!c_cchats)
  {
    ptScopedMonitorable<PcEntity> u (user);
    u->setCharChats(new CharChatsDef(user));
    c_cchats = u->getCharChats();
  }
  if (!c_cchats) return;

  ptScopedMonitorable<CharChats> cchats (c_cchats);
  for (size_t i=0;  i< cchats->GetDefChannelCount();  i++)
  {
    const char* channel = cchats->GetDefChannelName(i);
    if (channel)
    {
      if (!channelExists(channel)) createChannel(channel);
      addUser(user, channel);
      cchats->JoinChannel(channel, &getUserList(channel));
    }
  }
  */
}

void ChatManager::OnEntityRemove(Common::Entity::Entityp entity)
{
  /*
  if (!entity) return;
  const PcEntity* user;
  if ( !(user = entity->getPlayerEntity()) ) return;
  const CharChats* c_cchats = user->getCharChats();
  if (!c_cchats) return;


  ptScopedMonitorable<CharChats> cchats (c_cchats);
  for (size_t i=0;  i < cchats->GetChannelCount();  i++)
    delUser(user, cchats->GetChannelName(i).c_str());
    */
}
