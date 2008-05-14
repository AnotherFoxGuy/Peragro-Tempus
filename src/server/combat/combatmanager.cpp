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

#include "cssysdef.h"

#include "iutil/string.h"
#include "iutil/vfs.h"
#include "csutil/ref.h"
#include "csutil/csstring.h"
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

  printf("CombatManager: Got attack request\n");

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
    printf("CombatManager: Invalid target\n");
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
    lockedTarget->freeLock();
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
  float attackResult;
  
  if (!CheckIfReadyToAttack(lockedAttackerCharacter)) {
    // The player needs to wait a bit before attacking again
    lockedAttackerCharacter->freeLock();
    lockedTargetCharacter->freeLock();
    printf("CombatManager: Attacker not ready to attack\n");
    return 0;
  }

  if (!CheckIfTargetIsAttackable(lockedAttackerCharacter,
                                 lockedTargetCharacter)) {
    // Target is not within range or something like that
    lockedAttackerCharacter->freeLock();
    lockedTargetCharacter->freeLock();
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
    for (unsigned char slot = 0; slot < inventory->NoSlot; slot++) {

      const InventoryEntry* entry = inventory->getItem(slot);
      if (!entry || entry->id == 0) {
        continue;
      }
      //Item* item = server->getItemManager()->findById(entry->id);
      //if (!item) {
        //continue;
      //}
      //printf("\n\n\nIn place %c, type %u\n\n", slot, entry->getId());
      // TODO
      // Remove all items from the players slot
      // Make sure to send those udpates
      inventory->takeItem(slot);
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
      e->createFromItem(entry->id, entry->variation);

      Entity* ent = e->getEntity()->getLock();
      // TODO make it being released in a circle pattern
      ent->setPos(lockedTargetCharacter->getEntity()->getPos());
      ent->setSector(lockedTargetCharacter->getEntity()->getSector());
      ent->freeLock();

      Server::getServer()->addEntity(ent, true);
    }

      //if (invitem.id == Item::NoItem) {
      //  continue;
     // }
      //printf("\n\nThe dying character have items: %u\n\n", invitem.id);
     // TODO send die message to all
    NetworkHelper::broadcast(statsbs);
  } else {
    // Only report the damage to the affected player
    if (lockedTargetCharacter->getEntity()->getType() == Entity::PlayerEntityType) {
      NetworkHelper::sendMessage(lockedTargetCharacter, statsbs);
    }
  }

  lockedAttackerCharacter->freeLock();
  lockedTargetCharacter->freeLock();

  //PrepareAttack();
  //CalculateAttack();
  return 1;
}

/**
 * Roll the dice.
 * @return Returns a number between 0 and 100.
 */
int CombatManager::RollDice() {
  return (int) rand() % 101;
}

  
/**
 * Checks if the player is ready to attack.
 * @todo needs to be implemented.
 * @param lockedCharacter the locked version of a character.
 * @return true if ready to attack, otherwise false.
 */
bool CombatManager::CheckIfReadyToAttack(const Character* lockedCharacter) 
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

bool CombatManager::CheckIfTargetIsAttackable(const Character* attacker,
                                              const Character* target)
{
  const float* attackerPos;
  const float* targetPos;
  float attackerRotation;
  float distance = 0;
  float maxAttackDistance;

  attackerPos = attacker->getPos();
  targetPos = target->getPos();

  attackerRotation = attacker->getRotation();

  // TODO
  //maxAttackDistance = GetReach(attacker);

  
  for (int i = 0; i < 3; i++) {
    distance += (attackerPos[i] - targetPos[i]) * (attackerPos[i] - targetPos[i]);
  }

  // Should have used sqrt(distance) but faster to multiply the
  // maxAttackDistance
  if (distance > maxAttackDistance * maxAttackDistance) {
    // Target out of reach
    return false;
  }

  // TODO this should be made better, basically check
  // so the attacker and target is not differing too much
  // in height level.
  if (attackerPos[2] - targetPos[2] > 2) {
    return false;
  }

  return true;
}
 
/**
 * Calculates the attacking character's chance of hitting.
 * @param lockedCharacter The locked version of the attacking character.
 * @return attack chance.
 */
float CombatManager::GetAttackChance(const Character* lockedAttacker,
                                     const Character* lockedTarget) 
{
  return GetAgility(lockedAttacker) * GetSkillBonus(lockedAttacker) - 
     ptminf(GetAgility(lockedTarget), GetSapience(lockedTarget)) *
     ptmaxf(ptmaxf(GetBlock(lockedTarget), GetDodge(lockedTarget)),
         GetParry(lockedTarget));
}
 
/**
 * Returns the combined agility for a character (incl, bonuses).
 * @todo
 * @param lockedCharacter The locked version of the character.
 * @return The characters agility.
 */
float CombatManager::GetAgility(const Character* lockedCharacter) {
  return 0.0f;
}

/**
 * Returns the combined skill bonuses.
 * @todo
 * @param lockedCharacter The locked version of the character.
 * @return The characters skill bonuses.
 */
float CombatManager::GetSkillBonus(const Character* lockedCharacter) {
  return 0.0f;
}
/**
 * Returns the combined sapience for a character (incl, bonuses).
 * @todo
 * @param lockedCharacter The locked version of the character.
 * @return The characters sapience.
 */
float CombatManager::GetSapience(const Character* lockedCharacter) {
  return 0.0f;
}
/**
 * Returns the combined block for a character (incl, bonuses).
 * @todo
 * @param lockedCharacter The locked version of the character.
 * @return The characters block.
 */
float CombatManager::GetBlock(const Character* lockedCharacter) {
  return 0.0f;
}
/**
 * Returns the combined dodge for a character (incl, bonuses).
 * @todo
 * @param lockedCharacter The locked version of the character.
 * @return The characters dodge.
 */
float CombatManager::GetDodge(const Character* lockedCharacter) {
  return 0.0f;
}

/**
 * Returns the combined parry for a character (incl, bonuses).
 * @todo
 * @param lockedCharacter The locked version of the character.
 * @return The characters parry.
 */
float CombatManager::GetParry(const Character* lockedCharacter) {
  return 0.0f;
}

/**
 * Returns the combined strength for a character (incl, bonuses).
 * @todo
 * @param lockedCharacter The locked version of the character.
 * @return The characters strength.
 */
float CombatManager::GetStrength(const Character* lockedCharacter) {
  return 5.0f;
}

/**
 * Calculates the damage given by the character's weapon(s).
 * @todo
 * @param lockedCharacter The locked version of the character.
 * @return The weapon damage.
 */
float CombatManager::GetWeaponDamage(const Character* lockedCharacter) {
  return 10.0f;
}

