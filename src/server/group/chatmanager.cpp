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

#include "server/entity/entity.h"

ChatManager::ChatManager() : ChatGroups()
{
}

CharChats* ChatManager::GetChatObj(const Entity* entity)
{
  return NULL; //TODO
}

void ChatManager::OnEntityAdd(const Entity* entity)
{
  if (!entity) return;
  const PcEntity* user;
  if ( !(user = entity->getPlayerEntity()) ) return;
  CharChats* cchats = GetChatObj(entity);
  if (!cchats)
  {
    cchats = new CharChatsDef();
    //TODO insert cchats object
  }

  for (size_t i=0;  i< cchats->GetDefChannelCount();  i++)
    cchats->JoinChannel(this, cchats->GetDefChannelName(i));
  
}

void ChatManager::OnEntityRemove(const Entity* entity)
{
  if (!entity) return; 
  const PcEntity* user;
  if ( !(user = entity->getPlayerEntity()) ) return;
  const CharChats* cchats = GetChatObj(entity);
  if (!cchats) return;

  for (size_t i=0;  i < cchats->GetChannelCount();  i++)
    delUser(user, cchats->GetChannelName(i));
}
