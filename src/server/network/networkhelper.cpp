#include "networkhelper.h"
#include "network.h"

User* NetworkHelper::getUser(GenericMessage* msg)
{
  Connection* conn = msg->getConnection();
  if (!conn) 
    return 0;

  return conn->getUser();
}

PcEntity* NetworkHelper::getPcEntity(GenericMessage* msg)
{
  Connection* conn = msg->getConnection();
  if (!conn) 
    return 0;

  User* user = conn->getUser();
  if (!user) 
    return 0;

  PcEntity* ent = user->getEntity();
  if (!ent) 
    return 0;

  return ent;
}

CharacterEntity* NetworkHelper::getCharacterEntity(GenericMessage* msg)
{
  Connection* conn = msg->getConnection();
  if (!conn) 
    return 0;

  User* user = conn->getUser();
  if (!user) 
    return 0;

  PcEntity* ent = user->getEntity();
  if (!ent) 
    return 0;

  return ent;
}

Character* NetworkHelper::getCharacter(GenericMessage* msg)
{
  Connection* conn = msg->getConnection();
  if (!conn)
    return 0;

  User* user = conn->getUser();
  if (!user) 
    return 0;

  PcEntity* pc = user->getEntity();
  if (!pc) 
    return 0;

  return pc->getCharacter();
}

void NetworkHelper::sendMessage(Character* character , const ByteStream& bs)
{
  User* user = character->getUser();
  if (!user) 
    return;

  Connection* conn = user->getConnection();
  if (!conn) 
    return;

  conn->send(bs);
}

void NetworkHelper::sendMessage(CharacterEntity* character, const ByteStream& bs)
{
  if (character->getType() != Entity::PlayerEntity) return;

  User* user = ((PcEntity*)character)->getUser();
  if (!user) 
    return;

  Connection* conn = user->getConnection();
  if (!conn) 
    return;

  conn->send(bs);
}

void NetworkHelper::sendMessage(PcEntity* ent, const ByteStream& bs)
{
  User* user = ent->getUser();
  if (!user) 
    return;

  Connection* conn = user->getConnection();
  if (!conn) 
    return;

  conn->send(bs);
}

void NetworkHelper::sendMessage(User* user, const ByteStream& bs)
{
  Connection* conn = user->getConnection();
  if (!conn) 
    return;

  conn->send(bs);
}

