
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

void NetworkHelper::localcast(const ByteStream& bs, boost::shared_ptr<Entity> entity, size_t distance)
{
  Server* server = Server::getServer();
  EntityManager* em = server->getEntityManager();

  std::list<Common::Entity::Entityp> entities = em->Query(WFMath::Ball<3>(entity->GetPosition(), distance));
  std::list<Common::Entity::Entityp>::iterator it;
  for ( it=entities.begin() ; it != entities.end(); it++ )
  {
    boost::shared_ptr<PcEntity> target = boost::shared_dynamic_cast<PcEntity>(*it);
    if (target && target->GetUser() && target->GetUser()->GetConnection())
    {
      target->GetUser()->GetConnection()->send(bs);
    }
  }
}

void NetworkHelper::broadcast(const ByteStream& bs)
{
  UserManager* um = Server::getServer()->getUserManager();
  for (UserManager::const_UserMapIter i=um->GetUsers().begin(); 
       i!=um->GetUsers().end();  i++)
  {
    boost::shared_ptr<User> user = (*i).second;
    if (user && user->GetConnection())
      user->GetConnection()->send(bs);
  }
}

Array<PcEntity*> NetworkHelper::getUserList(PcEntity* ent, const char* channel)
{
  const ChatGroupSet* groups = ChatManager::getChatManager();
  Array<PcEntity*> emptychannel;

  if (!groups || !groups->isUserIn(ent, channel)) return emptychannel;
  return groups->getUserList(channel);

}
