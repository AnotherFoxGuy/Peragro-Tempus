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

//#include "cssysdef.h"

//#include "iutil/string.h"
//#include "iutil/vfs.h"
//#include "csutil/ref.h"
//#include "csutil/csstring.h"
#include "src/common/ptmath.h"
#include <stdlib.h> // random

#include "src/server/entity/entity.h"

#include "server/server.h"
#include "src/server/entity/statmanager.h"
#include "src/common/network/playermessages.h"
#include "src/server/network/networkhelper.h"
#include "src/server/combat/combatmanager.h"
#include "src/server/entity/itemmanager.h"
#include "src/server/database/table-inventory.h"
#include "src/common/network/entitymessages.h"
#include "src/server/entity/itementity.h"
#include "src/server/entity/entitymanager.h"


CombatManager::CombatManager() {
}


CombatManager::~CombatManager() {
}

unsigned int CombatManager::GetMaxLife(CharacterStats *lockedStats) {
  Stat* endurance = Server::getServer()->getStatManager()->
                                  findByName(ptString("Endurance",
                                             strlen("Endurance")));
  // Max life is 1 * endurance
  return lockedStats->getAmount(endurance);
}

/**
 * Handles a request from a player to attack a target.
 * @param attackerEntity The PcEntity for the attacking player.
 * @param targetID the Entity ID of the target.
 * @return 0 upon failure.
 */
int CombatManager::AttackRequest(const PcEntity *attackerEntity,
                                 unsigned int targetID) 
{
  const Entity* targetEntity;
  Character* lockedAttacker;
  Character* lockedTarget;
  const Character* c_char;
  int status = 0;

  printf("CombatManager: Got attack request, target: %d\n", targetID);

  if (!attackerEntity || !attackerEntity->getCharacter()) {
    // Invalid attacker.
    printf("CombatManager: Invalid attacker\n");
    return 0;
  }

  if (!(lockedAttacker = attackerEntity->getCharacter()->getLock())) {
    printf("CombatManager: Unable to lock attacker\n");
    return 0;
  }

  // Find the targets character
  targetEntity = Server::getServer()->getEntityManager()->findById(targetID);
  if (!targetEntity) {
    // Invalid target.
    lockedAttacker->freeLock();
    printf("CombatManager: Invalid target %d\n", targetID);
    return 0;
  }
  if (targetEntity->getType() == Entity::PlayerEntityType) {
    c_char = targetEntity->getPlayerEntity()->getCharacter();
    lockedTarget = c_char->getLock();
  } else if (targetEntity->getType() == Entity::NPCEntityType) {
    c_char = targetEntity->getNpcEntity()->getCharacter();
    lockedTarget = c_char->getLock();
  } else {
    // Should not happen, but do not crash on release build, since fake message
    // could bring down the server then
    lockedAttacker->freeLock();
    printf("CombatManager: Target neither player nor npc\n");
    return 0;
  }

  status = AttackRequest(lockedAttacker, lockedTarget);
  lockedAttacker->freeLock();
  lockedTarget->freeLock();
  return status;
}
  
/**
 * Handles a request from a player to attack a target
 * @todo This function is just way to big, divide it.
 * @param lockedAttackerCharacter The attacking character.
 * @param lockedTargetCharacter The attacked character.
 * @return 0 upon failure
 */
int
CombatManager::AttackRequest(Character* lockedAttackerCharacter,
                             Character* lockedTargetCharacter)
{
  float damage = 0;
  float attackChance = 0.0;
  CharacterStats* stats;
  int attackResult;
  int itemsToDrop = 0;
  int itemsDropped = 0;
  
  if (!CheckIfReadyToAttack(lockedAttackerCharacter)) {
    // The player needs to wait a bit before attacking again
    printf("CombatManager: Attacker not ready to attack\n");
    return 0;
  }

  if (!CheckIfTargetIsAttackable(lockedAttackerCharacter,
                                 lockedTargetCharacter)) {
    // Target is not within range or something like that
    printf("CombatManager: Target not attackable\n");
    return 0;
  }

  attackChance = GetAttackChance(lockedAttackerCharacter,
                                 lockedTargetCharacter);

  
  attackResult = RollDice();
  // TODO
  //DeductStamina(lockedAttackerCharacter);

  if (attackResult > attackChance) {
    // Miss
    damage = 0;
  } else {
    // TODO
    damage = (attackChance - attackResult) * 
             GetWeaponDamage(lockedAttackerCharacter) + 
             GetStrength(lockedAttackerCharacter);
  }

  printf("Damage:%f\n", damage);
  printf("attackChance is set to:%f\n", attackChance);
  printf("attackResult is set to:%d\n", attackResult);
  printf("weapondamage is set to:%f\n", GetWeaponDamage(lockedAttackerCharacter));
  printf("strength is set to:%f\n", GetStrength(lockedAttackerCharacter));

  // If attackResult was 10% or less of attacChance, critcal hit
  if (attackResult <= (attackChance * 0.1)) {
    // Double the damage
    damage = 2 * damage;
  }

  // TODO
  //SetCharacterWaitTime(lockedAttackerCharacter);


  Stat* hp = Server::getServer()->getStatManager()->
                                  findByName(ptString("Health",
                                             strlen("Health")));

  stats = lockedAttackerCharacter->getStats();
  printf("CombatManager: HP before deduction: %d\n", stats->getAmount(hp));
  stats->takeStat(hp, (int)damage);
  printf("CombatManager: HP after deduction: %d\n", stats->getAmount(hp));

  StatsChangeMessage msg;
  ByteStream statsbs;
  msg.setStatId(hp->getId());
  msg.setEntityId(lockedTargetCharacter->getEntity()->getId());
  msg.setName(ptString("Health", strlen("Health")));
  msg.setLevel(stats->getAmount(hp));
  msg.serialise(&statsbs);

  //Server* server = Server::getServer();
  
  if (stats->getAmount(hp) < damage) {
    // Player died
    // Need to spawn all the items
    Inventory* inventory = lockedTargetCharacter->getInventory();
    for (unsigned char slot = 0; slot < inventory->NoSlot; slot++) 
    {
      const InventoryEntry* entry = inventory->getItem(slot);
      if (entry && entry->id != 0)
      {
        itemsToDrop++;
      }
    }
    
    for (unsigned char slot = 0; slot < inventory->NoSlot; slot++) 
    {
      const InventoryEntry* entry = inventory->getItem(slot);
      if (!entry || entry->id == 0) 
      {
        continue;
      }
      // Save this information here since the entry 
      // will get invalid, if we take the item.
      unsigned int itemId = entry->id;
      unsigned int variation = entry->variation;
      if (inventory->takeItem(slot))
      {
        //Item* item = server->getItemManager()->findById(entry->id);
        //if (!item) {
        //continue;
        //}
        //printf("\n\n\nIn place %c, type %u\n\n", slot, entry->getId());
        // TODO
        // Remove all items from the players slot
        // Make sure to send those updates
        EquipMessage unequip_msg;
        unequip_msg.setEntityId(lockedTargetCharacter->getEntity()->getId());
        unequip_msg.setSlotId(slot);
        unequip_msg.setItemId(Item::NoItem); // No Item!
        unequip_msg.setFile(ptString::Null);
        unequip_msg.setMesh(ptString::Null);
        ByteStream bs;
        unequip_msg.serialise(&bs);
        NetworkHelper::localcast(bs, lockedTargetCharacter->getEntity());


        // Create new entity from item.
        ItemEntity* e = new ItemEntity();
        e->createFromItem(itemId, variation);

        Entity* ent = e->getEntity()->getLock();
        // Release items in a circlular pattern
        float radians = (2.0f*3.14f / itemsToDrop) * itemsDropped;
        float radius = 0.8f;
        float deltaX = cos(radians) * radius;
        float deltaY = sin(radians) * radius;
        float deltaZ = 0.0f;
        const float *tmp = lockedTargetCharacter->getEntity()->getPos();
        ent->setPos(tmp[0] + deltaX, tmp[1] + deltaY, tmp[2] + deltaZ);
        ent->setSector(lockedTargetCharacter->getEntity()->getSector());

        itemsDropped++;
        Server::getServer()->addEntity(ent, true);
        ent->freeLock();
      } // end if
    } // end for

      //if (invitem.id == Item::NoItem) {
      //  continue;
     // }
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
  return 1;
}

/**
 * Roll the dice.
 * @return Returns a number between 0 and 100.
 */
int CombatManager::RollDice() 
{
  return rand() % 101;
}

  
/**
 * Checks if the player is ready to attack.
 * @todo needs to be implemented.
 * @param lockedCharacter the locked version of a character.
 * @return true if ready to attack, otherwise false.
 */
bool CombatManager::CheckIfReadyToAttack(Character* lockedCharacter) 
{
  return true;
}

/**
 *@todo Is this really needed
 */
int CombatManager::PrepareAttack()
{
  // Arm bow etc
  return 100;
}

int CombatManager::CalculateAttack()
{
  //CalculateAttackChance();
  //DecreaseItemDurability();
  return 100;
}

/**
 * Checks if the target is attackable, in reach etc.
 * @todo
 * @param attacker The attacking character.
 * @param target The target character.
 * @return True if target is attackable, otherwise false.
 */
bool CombatManager::CheckIfTargetIsAttackable(Character* attacker,
                                              Character* target)
{
  const float* attackerPos;
  const float* targetPos;
  float attackerRotation;
  float distance = 0;
  // TODO should not be 200, but calculated by GetReach(attacker);
  float maxAttackDistance = 200;

  attackerPos = attacker->getPos();
  targetPos = target->getPos();

  attackerRotation = attacker->getRotation();

  // TODO
  //maxAttackDistance = GetReach(attacker);
  
  for (int i = 0; i < 3; i++) 
  {
    distance += (attackerPos[i] - targetPos[i]) * (attackerPos[i] - targetPos[i]);
  }

  // Should have used sqrt(distance) but faster to multiply the
  // maxAttackDistance
  if (distance > maxAttackDistance * maxAttackDistance) 
  {
    // Target out of reach
    printf("CombatManager: Target not attackable, distance %f, "
           "maxAttackDistance: %f\n", distance, maxAttackDistance);
    return false;
  }

  // TODO this should be made better, basically check
  // so the attacker and target is not differing too much
  // in height level.
  if (abs((int)(attackerPos[2] - targetPos[2])) > 200) 
  {
    printf("CombatManager: To big hight difference\n");
    return false;
  }

  return true;
}
 
/**
 * Calculates the attacking character's chance of hitting.
 * @param lockedCharacter The locked version of the attacking character.
 * @return attack chance.
 */
float CombatManager::GetAttackChance(Character* lockedAttacker,
                                     Character* lockedTarget) 
{
  return GetAgility(lockedAttacker) * GetSkillBonus(lockedAttacker) - 
     ptminf(GetAgility(lockedTarget), GetSapience(lockedTarget)) *
     ptmaxf(ptmaxf(GetBlock(lockedTarget), GetDodge(lockedTarget)),
         GetParry(lockedTarget));
}
 
/**
 * Returns the combined agility for a character (incl, bonuses).
 * @param lockedCharacter The locked version of the character.
 * @return The characters agility.
 */
float CombatManager::GetAgility(Character* lockedCharacter) 
{
  return GetStatValue(lockedCharacter, "Agility") +
         GetStatValueForAllEquipedItems(lockedCharacter,
                                        "Agility");
}

/**
 * Returns the combined skill bonuses.
 * @todo
 * @param lockedCharacter The locked version of the character.
 * @return The characters skill bonuses.
 */
float CombatManager::GetSkillBonus(Character* lockedCharacter) 
{
  return 8.0f;
}
/**
 * Returns the combined sapience for a character (incl, bonuses).
 * @param lockedCharacter The locked version of the character.
 * @return The characters sapience.
 */
float CombatManager::GetSapience(Character* lockedCharacter) 
{
  return GetStatValue(lockedCharacter, "Sapience") +
         GetStatValueForAllEquipedItems(lockedCharacter,
                                        "Sapience");;
}
/**
 * Returns the combined block for a character (incl, bonuses).
 * @param lockedCharacter The locked version of the character.
 * @return The characters block.
 */
float CombatManager::GetBlock(Character* lockedCharacter) 
{
  return GetStatValue(lockedCharacter, "Block") +
         GetStatValueForAllEquipedItems(lockedCharacter,
                                        "Block");;
}
/**
 * Returns the combined dodge for a character (incl, bonuses).
 * @param lockedCharacter The locked version of the character.
 * @return The characters dodge.
 */
float CombatManager::GetDodge(Character* lockedCharacter) 
{
   return GetStatValue(lockedCharacter, "Dodge") +
          GetStatValueForAllEquipedItems(lockedCharacter,
                                        "Dodge");
}

/**
 * Returns the combined parry for a character (incl, bonuses).
 * @param lockedCharacter The locked version of the character.
 * @return The characters parry.
 */
float CombatManager::GetParry(Character* lockedCharacter) 
{
  return GetStatValue(lockedCharacter, "Parry") +
         GetStatValueForAllEquipedItems(lockedCharacter,
                                        "Parry");
}

/**
 * Returns the combined strength for a character (incl, bonuses).
 * @param lockedCharacter The locked version of the character.
 * @return The characters strength.
 */
float CombatManager::GetStrength(Character* lockedCharacter) 
{
  return GetStatValue(lockedCharacter, "Strength") +
         GetStatValueForAllEquipedItems(lockedCharacter,
                                        "Strength");
}

/**
 * Calculates the damage given by the character's weapon(s).
 * @param lockedCharacter The locked version of the character.
 * @return The weapon damage.
 */
float CombatManager::GetWeaponDamage(Character* lockedCharacter) 
{
  return GetStatValueForEquipedWeapons(lockedCharacter,
                                       "Damage");
}

/**
 * Queries the stat for a character based on the stats name.
 * @param lockedCharacter The locked version of the character.
 * @param statName The stat's name.
 * @return The stats current value.
 */
float CombatManager::GetStatValue(Character* lockedCharacter,
                                  const char* statName) 
{
  // It would be tempting to use this function directly, however a character
  // can get bonuses, therefore this function should be called from the
  // different getters for stats.
  Server *server = Server::getServer();

  Stat* stat = server->getStatManager()->findByName(ptString(statName, 
    strlen(statName)));
  if (!stat) { 
    server->getStatManager()->dumpAllStatNames();
    printf("BUG: Unable to find stat: %s\n", statName);
    return 0.0f;
  }
  return (float)lockedCharacter->getStats()->getAmount(stat);
}

/**
 * Queries the stat value from an item based on stat name.
 * @param item The item to return the stat value for.
 * @param statName The stat's name.
 * @return The items stat based on the statName.
 */
float CombatManager::GetStatValueForItem(Item* item,
                                         const char* statName)
{
  Server *server = Server::getServer();
  Stat* stat = server->getStatManager()->findByName(ptString(statName, 
                                                    strlen(statName)));
  if (!stat) { 
    server->getStatManager()->dumpAllStatNames();
    printf("BUG: Unable to find stat: %s\n", statName);
    return 0.0f;
  }
  return (float)item->getStats()->getAmount(stat);
}

/**
 * Used to get an equiped item based on the slot number.
 * @param lockedCharacter The locked version of the character.
 * @param slot Which item, is decided by the slot its equiped on.
 * @return The item in that slot, or NULL if none found.
 */
Item* CombatManager::GetItem(Character* lockedCharacter, 
                             unsigned char slot)
{
  Server *server = Server::getServer();
  Inventory* inventory = lockedCharacter->getInventory();
    
  if (!inventory) 
  {
    return NULL;
  }

  if ( slot >= inventory->NoSlot) 
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

/**
 * Combines the character's items stat values, based on statName
 * @param lockedCharacter The locked version of the character.
 * @param statName The stat's name.
 * @return The items bonuses given a special stat.
 */
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
  }
  
  return value;
}

/**
 * Combines the characters weapon stat values, based on statName
 * @param lockedCharacter The locked version of the character.
 * @param statName The stat's name.
 * @return The weapons bonuses given a special stat.
 */
float CombatManager::GetStatValueForEquipedWeapons(Character* lockedCharacter,
                                                   const char* statName)
{
  float value = 0.0f;
  Item* item;
  Inventory* inventory = lockedCharacter->getInventory();

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
    if (item->getType() == ptString("Weapon", strlen("Weapon"))) {
      value += GetStatValueForItem(item, statName);
    }
  }
  
  return value;
}

/**
 * Deducts the players stamina after an attack
 * @param lockedCharacter The character to deduct stamina for.
 */
void CombatManager::DeductStamina(Character* lockedCharacter)
{
  float staminaDeduction = 0;
  Stat* stamina = 0;
  CharacterStats* stats = 0;

  staminaDeduction = (int)GetWeaponHeft(lockedCharacter) /
                     GetStrength(lockedCharacter);

  stamina = Server::getServer()->getStatManager()->
                                 findByName(ptString("Stamina",
                                            strlen("Stamina")));
  stats = lockedCharacter->getStats();
  printf("CombatManager: Stamina before deduction: %d\n",
         stats->getAmount(stamina));
  stats->takeStat(stamina, (int)staminaDeduction); 

  SendStatUpdate(stamina, stats, lockedCharacter,
                 "Stamina", CombatManagerSendTo::CHARACTER);
}

/**
 * Wrapper function to send stat update.
 * @param stat The type of stat to update.
 * @param stats The characters stat container.
 * @param name The name of the stat.
 * @param target Whom to send to.
 */
void CombatManager::SendStatUpdate(Stat* stat, 
                                   CharacterStats* stats,
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
}

/**
 * Function to get the weapon(s) heft value.
 * @param lockedCharacter The owner of the weapon
 * @return The weapon(s) heft value.
 */
float CombatManager::GetWeaponHeft(Character* lockedCharacter)
{
  return GetStatValueForEquipedWeapons(lockedCharacter, "Heft");
}

