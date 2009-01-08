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

#include <stdlib.h>
#include "server/server.h"
#include "interactionmanager.h"
#include "interaction.h"
#include "server/entity/entitymanager.h"
#include "server/entity/statmanager.h"

#include "common/util/math.h"
#include "common/network/playermessages.h"
#include "server/network/networkhelper.h"
#include "server/database/table-inventory.h"
#include "interactionqueue.h"

#define IM "InteractionManager: "
#define SLEEP 10

InteractionManager::InteractionManager()
{
  interactionQueue = new InteractionQueue();
}

InteractionManager::~InteractionManager()
{
  free(interactionQueue);
}

void InteractionManager::Run()
{
  // TODO this is not good, need to check if we are still running etc.
  while (1)
  {
    Interaction *interaction = NULL;
    // caller must free allocation
    interaction = interactionQueue->GetInteraction();
    if (!interaction) {
      // No character have any outstanding interactions.
      sleep(SLEEP);
      continue;
    }
    PerformInteraction(interaction);
    free(interaction);
  }
}

// TODO support our own char as target? In that case don't mess up locking
bool
InteractionManager::NormalAttack(Interaction *interaction)
{
  unsigned int targetID = 0;
  const Entity* targetEntity = NULL;
  const Character* c_char = NULL;

  ptScopedMonitorable<Character>
    lockedAttacker(interaction->character);

  targetID = lockedAttacker->GetTargetID();

  if (targetID == 0) {
    printf(IM "targetID 0, make sure that's not a legal one\n");
    return false;
  }

  targetEntity = Server::getServer()->getEntityManager()->findById(targetID);

  if (!targetEntity) {
    printf(IM "Invalid target\n");
    return false;
  }

  if (targetEntity->getType() == Entity::PlayerEntityType)
  {
    c_char = targetEntity->getPlayerEntity()->getCharacter();
  }
  else if (targetEntity->getType() == Entity::NPCEntityType)
  {
    c_char = targetEntity->getNpcEntity()->getCharacter();
  }
  else
  {
    // Should not happen, but do not crash on release build, since fake message
    // could bring down the server then.
    printf(IM "Target neither player nor npc\n");
    return false;
  }

  ptScopedMonitorable<Character> lockedTarget(c_char);


  if (!TargetAttackable(lockedAttacker, lockedTarget)) {
    return false;
  }

  // Make sure the character has enough stamina to do action
  if (!DeductStamina(lockedAttacker, interaction)) {
    return false;
  }

  CalculateDamage(lockedAttacker, lockedTarget);

}

bool
InteractionManager::DeductStamina(Character* lockedCharacter)
{
  float currentStamina = 0;
  float staminaDeduction = static_cast<int>(GetWeaponHeft(lockedCharacter) /
    GetStrength(lockedCharacter));

  Stat* stamina = Server::getServer()->getStatManager()->
    findByName(ptString("Stamina", strlen("Stamina")));

  CharacterStats* stats = lockedCharacter->getStats();

  if (!stamina || !stats)
  {
    printf(IM "Can't get stamina stat\n");
    return false;
  }

  currentStamina = stats->getAmount(stamina);

  printf(IM "Stamina before deduction: %f\n",
    currentStamina);

  if (currentStamina < staminaDeduction) {
    // Not enough stamina
    return false;
  }

  stats->takeStat(stamina, static_cast<int>(staminaDeduction));

  SendStatUpdate(stamina, stats, lockedCharacter, "Stamina",
    InteractionManagerSendTo::CHARACTER);

  return true;
}

float
InteractionManager::GetAttackChance(Character* lockedAttacker,
                                     Character* lockedTarget)
{
  return GetAgility(lockedAttacker) * GetSkillBonus(lockedAttacker) -
    PT::Math::MinF(GetAgility(lockedTarget), GetSapience(lockedTarget)) *
    PT::Math::MaxF(PT::Math::MaxF(GetBlock(lockedTarget),
      GetDodge(lockedTarget)), GetParry(lockedTarget));
}

float
InteractionManager::CalculateDamage(Character* lockedAttacker,
                                         Character* lockedTarget)
{

  float damage = 0.0f;
  const int attackResult = RollDice();
  const float attackChance = GetAttackChance(lockedAttacker,
                                             lockedTarget);

  if (attackResult <= attackChance)
  {
    // It was a hit. TODO
    damage = (attackChance - attackResult) *
      GetWeaponDamage(lockedAttacker) +
      GetStrength(lockedAttacker);
  }

  // If attackResult was 10% or less of attackChance, critcal hit.
  if (attackResult <= (attackChance * 0.1))
  {
    // Double the damage.
    printf(IM "Critical hit!\n");
    damage = 2 * damage;
  }

  printf("Damage:%f\n", damage);
  printf("attackChance is set to:%f\n", attackChance);
  printf("attackResult is set to:%d\n", attackResult);
  printf("weapondamage is set to:%f\n", GetWeaponDamage(lockedAttacker));
  printf("strength is set to:%f\n", GetStrength(lockedAttacker));

  return damage;
}

bool
InteractionManager::PerformInteraction(Interaction* interaction)
{
  if (!interaction) {
    return false;
  }

  switch(interaction->actionID) {
    case InteractionID::NORMAL_ATTACK:
      NormalAttack(interaction);
      break;
    case InteractionID::HEAL:
      break;
  }
}

bool
InteractionManager::TargetAttackable(Character* lockedAttacker,
                                          Character* lockedTarget)
{
  float maxAttackDistance = 0;
  float distance = 0;
  float attackerRotation = 0;
  float attackAngle = 0;
  float angleDiff = 0;
  static const float allowedAngle = PT_PI * 0.3f;
  const PtVector3 attackerPos(lockedAttacker->getEntity()->getPos());
  const PtVector3 targetPos(lockedTarget->getEntity()->getPos());

  maxAttackDistance = GetReach(lockedAttacker);
  distance = Distance(attackerPos, targetPos);

  printf(IM "attackerPos: %s, targetPos: %s, distance %f, "
    "maxAttackDistance: %f\n", attackerPos.ToString().c_str(),
    targetPos.ToString().c_str(), distance, maxAttackDistance);

  if (distance > maxAttackDistance)
  {
    // Target out of reach
    printf(IM "Target out of range\n");
    return false;
  }

  PtVector3 difference = attackerPos - targetPos;

  if (fabs(difference.y) > GetHightDeviation(lockedAttacker,
                                             lockedTarget))
  {
    printf(IM "To much height difference\n");
    return false;
  }

  attackerRotation =
    PT::Math::NormalizeAngle(lockedAttacker->getEntity()->getRotation());

  if (difference.x == 0.0f) difference.x = PT_EPSILON;
  attackAngle =
    PT::Math::NormalizeAngle(atan2(difference.x, difference.z));

  angleDiff = attackAngle - attackerRotation;

  printf("attackerRotation: %f, attackAngle: %f angleDiff: %f allowedAngle: %f\n",
    attackerRotation, attackAngle, angleDiff, allowedAngle);

  if (fabs(angleDiff) > allowedAngle)
  {
    printf(IM "Not facing the target\n");
    return false;
  }

  return true;
}

bool
InteractionManager::SelectTarget(const PcEntity *sourceEntity,
                                      unsigned int targetID)
{
  printf(IM "Got selection request, target: %d'n", targetID);

  if (!sourceEntity || !sourceEntity->getCharacter())
  {
    // Invalid source.
    printf(IM "Invalid source.\n");
    return false;
  }

  ptScopedMonitorable<Character>
    lockedSource(sourceEntity->getCharacter());

  if (!lockedSource)
  {
    printf(IM "Unable to lock source.\n");
    return false;
  }

  lockedSource->SetTargetID(targetID);
}

const Character*
InteractionManager::GetTargetCharacter(Character* lockedCharacter)
{
  return Server::getServer()->getEntityManager()->
                              findById(lockedCharacter->GetTargetID());

}

bool
InteractionManager::QueueAction(const PcEntity *sourceEntity,
                                     unsigned int actionID)
{
  Interaction *interaction = new Interaction();

  printf(IM "Got queueAction request, actionID: %d'n", actionID);

  if (!sourceEntity || !sourceEntity->getCharacter())
  {
    // Invalid source.
    printf(IM "Invalid source.\n");
    return false;
  }

  ptScopedMonitorable<Character>
    lockedSource(sourceEntity->getCharacter());

  if (!lockedSource)
  {
    printf(IM "Unable to lock source.\n");
    return false;
  }

  interaction->actionID = actionID;
  interaction->character = sourceEntity->getCharacter();

  // Caller must alloc interaction
  interactionQueue->SetInteraction(interaction);
}

float InteractionManager::GetBlock(Character* lockedCharacter)
{
  return GetStatValue(lockedCharacter, "Block") +
    GetStatValueForAllEquipedItems(lockedCharacter, "Block");
} 

float InteractionManager::GetDodge(Character* lockedCharacter)
{
  return GetStatValue(lockedCharacter, "Dodge") +
    GetStatValueForAllEquipedItems(lockedCharacter, "Dodge");
}

float InteractionManager::GetParry(Character* lockedCharacter)
{
  return GetStatValue(lockedCharacter, "Parry") +
    GetStatValueForAllEquipedItems(lockedCharacter, "Parry");
}

float InteractionManager::GetStrength(Character* lockedCharacter)
{
  return GetStatValue(lockedCharacter, "Strength") +
    GetStatValueForAllEquipedItems(lockedCharacter, "Strength");
}

float InteractionManager::GetReach(Character* lockedCharacter)
{
  return GetStatValue(lockedCharacter, "Reach") +
    GetStatValueForAllEquipedItems(lockedCharacter, "Reach");
}

float InteractionManager::GetWeaponDamage(Character* lockedCharacter)
{
  return GetStatValueForEquipedWeapons(lockedCharacter, "Damage");
}

float InteractionManager::GetStatValueForEquipedWeapons(Character* lockedCharacter,
                                                   const char* statName)
{
  Inventory* inventory = lockedCharacter->getInventory();
  if (!inventory)
  {
    return 0.0f;
  }

  float value = 0.0f;
  for (unsigned char slot = 0; slot < inventory->NoSlot; slot++)
  {
    Item* item = GetItem(lockedCharacter, slot);
    if (!item)
    {
      continue;
    }
    if (item->getType() == ptString("Weapon", strlen("Weapon")))
    {
      value += GetStatValueForItem(item, statName);
    }
  } // end for

  return value;
}

float InteractionManager::GetStatValueForItem(const Item* item,
                                             const char* statName)
{
  Server *server = Server::getServer();
  const Stat* stat = server->getStatManager()->
    findByName(ptString(statName, strlen(statName)));
  if (!stat)
  {
    server->getStatManager()->dumpAllStatNames();
    printf("BUG: Unable to find stat: %s\n", statName);
    return 0.0f;
  }
  return static_cast<float>(item->getStats()->getAmount(stat));
} 

float InteractionManager::GetStatValue(Character* lockedCharacter,
                                  const char* statName)
{
  Server* server = Server::getServer();

  const Stat* stat = server->getStatManager()->findByName(ptString(statName,
    strlen(statName)));
  if (!stat)
  {
    server->getStatManager()->dumpAllStatNames();
    printf("BUG: Unable to find stat: %s\n", statName);
    return 0.0f;
  }
  return static_cast<float>(lockedCharacter->getStats()->getAmount(stat));
}

float InteractionManager::GetStatValueForAllEquipedItems(Character* lockedCharacter,
                                                    const char* statName)
{
  float value = 0.0f;
  Inventory* inventory = lockedCharacter->getInventory();
  Item* item;

  if (!inventory)
  {
    return 0.0f;
  }

  for (unsigned char slot = 0; slot < inventory->NoSlot; slot++)
  {
    item = GetItem(lockedCharacter, slot);
    if (!item)
    {
      continue;
    }
    value += GetStatValueForItem(item, statName);
  } // end for

  return value;
}

Item* InteractionManager::GetItem(Character* lockedCharacter,
                             unsigned char slot)
{
  Server *server = Server::getServer();
  Inventory* inventory = lockedCharacter->getInventory();

  if (!inventory)
  {
    return NULL;
  }

  if (slot >= inventory->NoSlot)
  {
    return NULL;
  }

  const InventoryEntry* entry = inventory->getItem(slot);
  if (!entry || entry->id == 0)
  {
    return NULL;
  }
  Item* item = server->getItemManager()->findById(entry->id);
  return item;
}

float InteractionManager::GetAgility(Character* lockedCharacter)
{
  return GetStatValue(lockedCharacter, "Agility") +
    GetStatValueForAllEquipedItems(lockedCharacter, "Agility");
}

float InteractionManager::GetSkillBonus(Character* lockedCharacter)
{
  return 8.0f;
}

float InteractionManager::GetSapience(Character* lockedCharacter)
{
  return GetStatValue(lockedCharacter, "Sapience") +
    GetStatValueForAllEquipedItems(lockedCharacter, "Sapience");
} 

float InteractionManager::GetWeaponHeft(Character* lockedCharacter)
{
  return GetStatValueForEquipedWeapons(lockedCharacter, "Heft");
}

int InteractionManager::RollDice()
{
  return rand() % 101;
}

void 
InteractionManager::SendStatUpdate(const Stat* stat,
                                   const CharacterStats* stats,
                                   Character* lockedCharacter,
                                   const char* name,
                                   int target)
{
  StatsChangeMessage msg;
  ByteStream statsbs;
  msg.setStatId(stat->getId());
  msg.setEntityId(lockedCharacter->getEntity()->getId());
  msg.setName(ptString(name, strlen(name)));
  msg.setLevel(stats->getAmount(stat));
  msg.serialise(&statsbs);
  if (target == InteractionManagerSendTo::BROADCAST)
  {
    NetworkHelper::broadcast(statsbs);
  }
  else if (target == InteractionManagerSendTo::LOCALCAST)
  {
    NetworkHelper::localcast(statsbs, lockedCharacter->getEntity());
  }
  else if (target == InteractionManagerSendTo::CHARACTER)
  {
    NetworkHelper::sendMessage(lockedCharacter, statsbs);
  }
}

float GetHightDeviation(const Character* lockedAttacker,
                        const Character* lockedTarget)
{
  // TODO
  return 0;
}
