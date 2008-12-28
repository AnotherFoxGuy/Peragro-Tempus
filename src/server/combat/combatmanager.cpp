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

#include "server/combat/combatmanager.h"

#include <stdlib.h>

#include "common/util/math.h"
#include "common/geom/vector3.h"
#include "common/network/playermessages.h"
#include "common/network/entitymessages.h"

#include "server/server.h"
#include "server/entity/entity.h"
#include "server/entity/statmanager.h"
#include "server/entity/itemmanager.h"
#include "server/entity/itementity.h"
#include "server/entity/entitymanager.h"
#include "server/network/networkhelper.h"
#include "server/database/table-inventory.h"

CombatManager::CombatManager()
{
} // end CombatManager()

CombatManager::~CombatManager()
{
} // end ~CombatManager()

unsigned int CombatManager::GetMaxLife(CharacterStats* lockedStats)
{
  Stat* endurance = Server::getServer()->getStatManager()->
    findByName(ptString("Endurance", strlen("Endurance")));
  // Max life is 1 * endurance.
  return lockedStats->getAmount(endurance);
} // end GetMaxLife()

bool CombatManager::AttackRequest(const PcEntity *attackerEntity,
                                 unsigned int targetID)
{
  printf("CombatManager: Got attack request, target: %d\n", targetID);

  if (!attackerEntity || !attackerEntity->getCharacter())
  {
    // Invalid attacker.
    printf("CombatManager: Invalid attacker\n");
    return false;
  }

  ptScopedMonitorable<Character>
    lockedAttacker(attackerEntity->getCharacter());
  if (!lockedAttacker)
  {
    printf("CombatManager: Unable to lock attacker\n");
    return false;
  }

  // Find the targets character
  const Entity* targetEntity =
    Server::getServer()->getEntityManager()->findById(targetID);
  if (!targetEntity)
  {
    // Invalid target.
    printf("CombatManager: Invalid target %d\n", targetID);
    return false;
  }

  const Character* c_char;
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
    printf("CombatManager: Target neither player nor npc\n");
    return false;
  }
  ptScopedMonitorable<Character> lockedTarget(c_char);

  const bool status = AttackRequest(lockedAttacker, lockedTarget);

  printf("B: AttackRequest()\n");

  return status;
} // end AttackRequest()

bool CombatManager::AttackRequest(Character* lockedAttackerCharacter,
                                 Character* lockedTargetCharacter)
{
  if (!CheckIfReadyToAttack(lockedAttackerCharacter))
  {
    // The player needs to wait a bit before attacking again.
    printf("CombatManager: Attacker not ready to attack\n");
    return false;
  }

  if (!CheckIfTargetIsAttackable(lockedAttackerCharacter,
    lockedTargetCharacter))
  {
    // Target is not within range or something like that.
    printf("CombatManager: Target not attackable\n");
    return false;
  }

  DeductStamina(lockedAttackerCharacter);

  const float attackChance = GetAttackChance(lockedAttackerCharacter,
    lockedTargetCharacter);
  const int attackResult = RollDice();
  float damage = 0.0f;
  if (attackResult <= attackChance)
  {
    // It was a hit. TODO
    damage = (attackChance - attackResult) *
      GetWeaponDamage(lockedAttackerCharacter) +
      GetStrength(lockedAttackerCharacter);
  }

  // If attackResult was 10% or less of attacChance, critcal hit.
  if (attackResult <= (attackChance * 0.1))
  {
    // Double the damage.
    printf("Critical hit!\n");
    damage = 2 * damage;
  }

  printf("Damage:%f\n", damage);
  printf("attackChance is set to:%f\n", attackChance);
  printf("attackResult is set to:%d\n", attackResult);
  printf("weapondamage is set to:%f\n", GetWeaponDamage(lockedAttackerCharacter));
  printf("strength is set to:%f\n", GetStrength(lockedAttackerCharacter));

  // TODO
  //SetCharacterWaitTime(lockedAttackerCharacter);

  Stat* hp = Server::getServer()->getStatManager()->
    findByName(ptString("Health", strlen("Health")));

  CharacterStats* stats = lockedAttackerCharacter->getStats();
  printf("CombatManager: HP before deduction: %d\n", stats->getAmount(hp));
  stats->takeStat(hp, static_cast<int>(damage));
  printf("CombatManager: HP after deduction: %d\n", stats->getAmount(hp));

  StatsChangeMessage msg;
  ByteStream statsbs;
  msg.setStatId(hp->getId());
  msg.setEntityId(lockedTargetCharacter->getEntity()->getId());
  msg.setName(ptString("Health", strlen("Health")));
  msg.setLevel(stats->getAmount(hp));
  msg.serialise(&statsbs);

  int itemsToDrop = 0;
  int itemsDropped = 0;

  if (stats->getAmount(hp) < damage)
  {
    // Player died, need to spawn all the items.
    Inventory* inventory = lockedTargetCharacter->getInventory();
    for (unsigned char slot = 0; slot < inventory->NoSlot; slot++)
    {
      const InventoryEntry* entry = inventory->getItem(slot);
      if (entry && entry->id != 0)
      {
        itemsToDrop++;
      }
    } // end for

    for (unsigned char slot = 0; slot < inventory->NoSlot; slot++)
    {
      const InventoryEntry* entry = inventory->getItem(slot);
      if (!entry || entry->id == 0)
      {
        continue;
      }
      // Save this information here since the entry will get invalid, if we
      // take the item.
      unsigned int itemId = entry->id;
      unsigned int variation = entry->variation;
      if (inventory->takeItem(slot))
      {
        //Item* item = server->getItemManager()->findById(entry->id);
        //if (!item)
        //{
        //  continue;
        //}
        //printf("\n\n\nIn place %c, type %u\n\n", slot, entry->getId());
        // TODO
        // Remove all items from the players slot
        // Make sure to send those updates
        EquipMessage unequip_msg;
        unequip_msg.setEntityId(lockedTargetCharacter->getEntity()->getId());
        unequip_msg.setSlotId(slot);
        unequip_msg.setItemId(Item::NoItem); // No Item!
        unequip_msg.setMeshId(0); // Not used yet!
        unequip_msg.setFileName(ptString::Null);
        unequip_msg.setMeshName(ptString::Null);
        ByteStream bs;
        unequip_msg.serialise(&bs);
        NetworkHelper::localcast(bs, lockedTargetCharacter->getEntity());

        // Create new entity from item.
        ItemEntity* e = new ItemEntity();
        e->createFromItem(itemId, variation);

        ptScopedMonitorable<Entity> ent(e->getEntity());
        // Release items in a circlular pattern
        const float radians = (2.0f*3.14f / itemsToDrop) * itemsDropped;
        const float radius = 0.8f;
        const PtVector3 delta(cos(radians) * radius, sin(radians) * radius,
          0.0f);
        ent->setPos(lockedTargetCharacter->getEntity()->getPos() + delta);
        ent->setSector(lockedTargetCharacter->getEntity()->getSector());

        itemsDropped++;
        Server::getServer()->addEntity(ent, true);
      } // end if
    } // end for

    //if (invitem.id == Item::NoItem)
    //{
    //  continue;
    //}
    //printf("\n\nThe dying character have items: %u\n\n", invitem.id);
    // TODO send die message to all
    NetworkHelper::broadcast(statsbs);
  }
  else
  {
    // Only report the damage to the affected player
    if (lockedTargetCharacter->getEntity()->getType() == Entity::PlayerEntityType)
    {
      NetworkHelper::sendMessage(lockedTargetCharacter, statsbs);
      //TODO: Send a hurt message to the surrounding players for animation purposes??
    }
  }

  //PrepareAttack();
  //CalculateAttack();
  return true;
} // end AttackRequest()

int CombatManager::RollDice()
{
  return rand() % 101;
} // end RollDice()

bool CombatManager::CheckIfReadyToAttack(Character* lockedCharacter)
{
  return true;
} // end CheckIfReadyToAttack()

int CombatManager::PrepareAttack()
{
  // Arm bow etc
  return 100;
} // end PrepareAttack()

int CombatManager::CalculateAttack()
{
  //CalculateAttackChance();
  //DecreaseItemDurability();
  return 100;
} // end CalculateAttack()

bool CombatManager::CheckIfTargetIsAttackable(Character* attacker,
                                              Character* target)
{
  const PtVector3 attackerPos(attacker->getEntity()->getPos());
  const PtVector3 targetPos(target->getEntity()->getPos());

  // TODO should not be 200, but calculated by GetReach(attacker);
  const float maxAttackDistance = 2.0f;
  //const float maxAttackDistance = GetReach(attacker);

  const float distance = Distance(attackerPos, targetPos);

  printf("CombatManager: attackerPos: %s, targetPos: %s, distance %f, "
    "maxAttackDistance: %f\n", attackerPos.ToString().c_str(),
    targetPos.ToString().c_str(), distance, maxAttackDistance);

  if (distance > maxAttackDistance)
  {
    // Target out of reach
    printf("CombatManager: Target out of range\n");
    return false;
  }

  PtVector3 difference = attackerPos - targetPos;
  // TODO this should be made better, basically check
  // so the attacker and target is not differing too much
  // in height level.
  if (fabs(difference.y) > 2.0f)
  {
    printf("CombatManager: To much height difference\n");
    return false;
  }

  const float attackerRotation =
    PT::Math::NormalizeAngle(attacker->getEntity()->getRotation());

  if (difference.x == 0.0f) difference.x = PT_EPSILON;
  const float attackAngle =
    PT::Math::NormalizeAngle(atan2(difference.x, difference.z));

  const float angleDiff = attackAngle - attackerRotation;
  static const float allowedAngle = PT_PI * 0.3f;

  printf("attackerRotation: %f, attackAngle: %f angleDiff: %f allowedAngle: %f\n",
    attackerRotation, attackAngle, angleDiff, allowedAngle);

  if (fabs(angleDiff) > allowedAngle)
  {
    printf("CombatManager: Not facing the target\n");
    return false;
  }

  return true;
} // end CheckIfTargetIsAttackable()

float CombatManager::GetAttackChance(Character* lockedAttacker,
                                     Character* lockedTarget)
{
  return GetAgility(lockedAttacker) * GetSkillBonus(lockedAttacker) -
    PT::Math::MinF(GetAgility(lockedTarget), GetSapience(lockedTarget)) *
    PT::Math::MaxF(PT::Math::MaxF(GetBlock(lockedTarget),
      GetDodge(lockedTarget)), GetParry(lockedTarget));
} // end GetAttackChance()

float CombatManager::GetAgility(Character* lockedCharacter)
{
  return GetStatValue(lockedCharacter, "Agility") +
    GetStatValueForAllEquipedItems(lockedCharacter, "Agility");
} // end GetAgility()

float CombatManager::GetSkillBonus(Character* lockedCharacter)
{
  return 8.0f;
} // end GetSkillBonus()

float CombatManager::GetSapience(Character* lockedCharacter)
{
  return GetStatValue(lockedCharacter, "Sapience") +
    GetStatValueForAllEquipedItems(lockedCharacter, "Sapience");
} // end GetSapience()

float CombatManager::GetBlock(Character* lockedCharacter)
{
  return GetStatValue(lockedCharacter, "Block") +
    GetStatValueForAllEquipedItems(lockedCharacter, "Block");
} // end GetBlock()

float CombatManager::GetDodge(Character* lockedCharacter)
{
  return GetStatValue(lockedCharacter, "Dodge") +
    GetStatValueForAllEquipedItems(lockedCharacter, "Dodge");
} // end GetDodge()

float CombatManager::GetParry(Character* lockedCharacter)
{
  return GetStatValue(lockedCharacter, "Parry") +
    GetStatValueForAllEquipedItems(lockedCharacter, "Parry");
} // end GetParry()

float CombatManager::GetStrength(Character* lockedCharacter)
{
  return GetStatValue(lockedCharacter, "Strength") +
    GetStatValueForAllEquipedItems(lockedCharacter, "Strength");
} // end GetStrength()

float CombatManager::GetWeaponDamage(Character* lockedCharacter)
{
  return GetStatValueForEquipedWeapons(lockedCharacter, "Damage");
} // end GetWeaponDamage()

float CombatManager::GetStatValue(Character* lockedCharacter,
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
} // end GetStatValue()

float CombatManager::GetStatValueForItem(const Item* item,
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
} // end GetStatValueForItem()

Item* CombatManager::GetItem(Character* lockedCharacter,
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
} // end GetItem()

float CombatManager::GetStatValueForAllEquipedItems(Character* lockedCharacter,
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
} // end GetStatValueForAllEquipedItems()

float CombatManager::GetStatValueForEquipedWeapons(Character* lockedCharacter,
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
} // end GetStatValueForEquipedWeapons()

void CombatManager::DeductStamina(Character* lockedCharacter)
{
  float staminaDeduction = static_cast<int>(GetWeaponHeft(lockedCharacter) /
    GetStrength(lockedCharacter));

  Stat* stamina = Server::getServer()->getStatManager()->
    findByName(ptString("Stamina", strlen("Stamina")));
  CharacterStats* stats = lockedCharacter->getStats();
  if (!stamina || !stats)
  {
    printf("CombatManager: Can't get stamina stat\n");
    return;
  }
  printf("CombatManager: Stamina before deduction: %d\n",
    stats->getAmount(stamina));
  stats->takeStat(stamina, static_cast<int>(staminaDeduction));

  SendStatUpdate(stamina, stats, lockedCharacter, "Stamina",
    CombatManagerSendTo::CHARACTER);
} // end DeductStamina()

void CombatManager::SendStatUpdate(const Stat* stat, const CharacterStats* stats,
                                   Character* lockedCharacter, const char* name,
                                   int target)
{
  StatsChangeMessage msg;
  ByteStream statsbs;
  msg.setStatId(stat->getId());
  msg.setEntityId(lockedCharacter->getEntity()->getId());
  msg.setName(ptString(name, strlen(name)));
  msg.setLevel(stats->getAmount(stat));
  msg.serialise(&statsbs);
  if (target == CombatManagerSendTo::BROADCAST)
  {
    NetworkHelper::broadcast(statsbs);
  }
  else if (target == CombatManagerSendTo::LOCALCAST)
  {
    NetworkHelper::localcast(statsbs, lockedCharacter->getEntity());
  }
  else if (target == CombatManagerSendTo::CHARACTER)
  {
    NetworkHelper::sendMessage(lockedCharacter, statsbs);
  }
} // end SendStatUpdate()

float CombatManager::GetWeaponHeft(Character* lockedCharacter)
{
  return GetStatValueForEquipedWeapons(lockedCharacter, "Heft");
} // end GetWeaponHeft()

