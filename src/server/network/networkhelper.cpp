
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

boost::shared_ptr<PcEntity> NetworkHelper::GetEntity(GenericMessage* msg)
{
  Connection* conn = msg->getConnection();
  if (!conn) return boost::shared_ptr<PcEntity>();

  User* user = conn->GetUser().get();
  if (!user) return boost::shared_ptr<PcEntity>();

  return user->GetEntity();
}

void NetworkHelper::sendMessage(Entity* ent, const ByteStream& bs)
{
  PcEntity* pc_ent = dynamic_cast<PcEntity*>(ent);
  if (!pc_ent) return;

  User* user = pc_ent->GetUser();
  if (!user) return;

  sendMessage(user, bs);
}

void NetworkHelper::sendMessage(User* user, const ByteStream& bs)
{
  const Connection* conn = user->GetConnection();
  if (!conn) return;

  conn->send(bs);
}

void NetworkHelper::localcast(const ByteStream& bs, boost::shared_ptr<Entity> entity)
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
