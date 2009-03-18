
#include "networkhelper.h"
#include "network.h"

#include "server/entity/character/character.h"
#include "server/entity/pcentity.h"
#include "server/entity/usermanager.h"
#include "server/group/chatmanager.h"
#include "server/server.h"

boost::shared_ptr<User> NetworkHelper::getUser(GenericMessage* msg)
{
  Connection* conn = msg->getConnection();
  if (!conn) return boost::shared_ptr<User>();

  return conn->GetUser();
}

Entity* NetworkHelper::getEntity(GenericMessage* msg)
{
  Connection* conn = msg->getConnection();
  if (!conn) return 0;

  User* user = conn->GetUser().get();
  if (!user) return 0;

  PcEntity* ent = user->GetEntity();
  if (!ent) return 0;

  return ent;
}

void NetworkHelper::sendMessage(Character* character , const ByteStream& bs)
{
  /*
  const User* user = character->getUser();
  if (!user)
    return;

  const Connection* conn = user->getConnection();
  if (!conn)
    return;

  conn->send(bs);
  */
}

void NetworkHelper::sendMessage(Entity* ent, const ByteStream& bs)
{
  /*
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
  */
}

void NetworkHelper::sendMessage(const User* user, const ByteStream& bs)
{
  /*
  const Connection* conn = user->getConnection();
  if (!conn)
    return;

  conn->send(bs);
  */
}

void NetworkHelper::localcast(const ByteStream& bs, Entity* entity)
{
  /*
  Server* server = Server::getServer();
  SectorManager* sm = server->GetSectorManager();
  ptString region = sm->getRegionName(entity->GetSector());

  for (size_t i=0; i<server->getUserManager()->getUserCount(); i++)
  {
    User* user = server->getUserManager()->getUser(i);
    if (user && user->getConnection() && user->getEntity())
    {
      unsigned short sector = user->getEntity()->getEntity()->GetSector();
      if (sm->getRegionName(sector) == region)
      {
        user->getConnection()->send(bs);
      }
    }
  }
  */
}

void NetworkHelper::distancecast(const ByteStream& bs, Entity* entity, unsigned dist)
{
  /*
  Server* server = Server::getServer();
  if (!entity) return;
  if (dist == 0) return;

  // use square-distance multipied by 100m^2
  float sqdist = 10000.0f * dist * dist;

  for (size_t i=0; i<server->getUserManager()->getUserCount(); i++)
  {
    User* user = server->getUserManager()->getUser(i);
    if (user && user->getConnection())
    {
      const Entity* other_ent = user->getEntity()->getEntity();
      if (other_ent && entity->GetSector() == other_ent->GetSector() &&
          entity->getDistanceTo2(other_ent) < sqdist)
        user->getConnection()->send(bs);
    }
  */
}

void NetworkHelper::broadcast(const ByteStream& bs)
{
  /*
  Server* server = Server::getServer();
  for (size_t i=0; i<server->getUserManager()->getUserCount(); i++)
  {
    User* user = server->getUserManager()->getUser(i);
    if (user && user->getConnection())
      user->getConnection()->send(bs);
  }
  */
}

Array<PcEntity*> NetworkHelper::getUserList(PcEntity* ent, const char* channel)
{
  const ChatGroupSet* groups = ChatManager::getChatManager();
  Array<PcEntity*> emptychannel;

  if (!groups || !groups->isUserIn(ent, channel)) return emptychannel;
  return groups->getUserList(channel);

/*
  Array<const PcEntity*> channelusers;
  channelusers.add(ent);
  return channelusers;
*/
}
