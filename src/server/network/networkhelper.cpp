#include "networkhelper.h"
#include "network.h"

#include "server/entity/character.h"
#include "server/entity/pcentity.h"
#include "server/entity/sectormanager.h"
#include "server/entity/usermanager.h"
#include "server/group/chatmanager.h"
#include "server/server.h"

const User* NetworkHelper::getUser(GenericMessage* msg)
{
  Connection* conn = msg->getConnection();
  if (!conn) 
    return 0;

  return conn->getUser();
}

const PcEntity* NetworkHelper::getPcEntity(GenericMessage* msg)
{
  Connection* conn = msg->getConnection();
  if (!conn) 
    return 0;

  User* user = conn->getUser();
  if (!user) 
    return 0;

  const PcEntity* ent = user->getEntity();
  if (!ent) 
    return 0;

  return ent;
}

const Entity* NetworkHelper::getEntity(GenericMessage* msg)
{
  Connection* conn = msg->getConnection();
  if (!conn) 
    return 0;

  User* user = conn->getUser();
  if (!user) 
    return 0;

  const PcEntity* ent = user->getEntity();
  if (!ent) 
    return 0;

  return ent->getEntity();
}

const Character* NetworkHelper::getCharacter(GenericMessage* msg)
{
  Connection* conn = msg->getConnection();
  if (!conn)
    return 0;

  User* user = conn->getUser();
  if (!user) 
    return 0;

  const PcEntity* pc = user->getEntity();
  if (!pc) 
    return 0;

  return pc->getCharacter();
}

void NetworkHelper::sendMessage(const Character* character , const ByteStream& bs)
{
  const User* user = character->getUser();
  if (!user) 
    return;

  const Connection* conn = user->getConnection();
  if (!conn) 
    return;

  conn->send(bs);
}

void NetworkHelper::sendMessage(const PcEntity* ent, const ByteStream& bs)
{
  const User* user = ent->getUser();
  if (!user) 
    return;

  const Connection* conn = user->getConnection();
  if (!conn) 
    return;

  conn->send(bs);
}

void NetworkHelper::sendMessage(const Entity* ent, const ByteStream& bs)
{
  const PcEntity* pc_ent = ent->getPlayerEntity();
  if (!pc_ent)
    return;

  const User* user = pc_ent->getUser();
  if (!user) 
    return;

  const Connection* conn = user->getConnection();
  if (!conn) 
    return;

  conn->send(bs);
}

void NetworkHelper::sendMessage(const User* user, const ByteStream& bs)
{
  const Connection* conn = user->getConnection();
  if (!conn) 
    return;

  conn->send(bs);
}

void NetworkHelper::localcast(const ByteStream& bs, const Entity* entity)
{
  Server* server = Server::getServer();
  SectorManager* sm = server->getSectorManager();
  ptString region = sm->getRegionName(entity->getSector());

  for (size_t i=0; i<server->getUserManager()->getUserCount(); i++)
  {
    User* user = server->getUserManager()->getUser(i);
    if (user && user->getConnection() && user->getEntity())
    {
      unsigned short sector = user->getEntity()->getEntity()->getSector();
      if (sm->getRegionName(sector) == region)
      {
        user->getConnection()->send(bs);
      }
    }
  }
}

void NetworkHelper::distancecast(const ByteStream& bs, const Entity* entity, unsigned dist)
{
  Server* server = Server::getServer();
  if (!entity) return;
  if (dist == 0) return;

  /* use square-distance multipied by 100m^2 */
  float sqdist = 10000.0f * dist * dist;

  for (size_t i=0; i<server->getUserManager()->getUserCount(); i++)
  {
    User* user = server->getUserManager()->getUser(i);
    if (user && user->getConnection())
    {
      const Entity* other_ent = user->getEntity()->getEntity();
      if (other_ent && entity->getSector() == other_ent->getSector() &&
          entity->getDistanceTo2(other_ent) < sqdist)
        user->getConnection()->send(bs);
    }
  }
}

void NetworkHelper::broadcast(const ByteStream& bs)
{
  Server* server = Server::getServer();
  for (size_t i=0; i<server->getUserManager()->getUserCount(); i++)
  {
    User* user = server->getUserManager()->getUser(i);
    if (user && user->getConnection())
      user->getConnection()->send(bs);
  }
}

Array<const PcEntity*> NetworkHelper::getUserList(const PcEntity* ent, const char* channel)
{
  const ChatGroups* groups = ChatManager::getChatManager();
  Array<const PcEntity*> emptychannel;

  if (!groups || !groups->isUserIn(ent, channel)) return emptychannel;
  return groups->getUserList(channel);

/*
  Array<const PcEntity*> channelusers;
  channelusers.add(ent);
  return channelusers;
*/
}
