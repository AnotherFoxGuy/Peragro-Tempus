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
#include "common/network/entitymessages.h"

#include "common/util/math.h"
#include "common/network/playermessages.h"
#include "common/network/combatmessages.h"
#include "server/network/networkhelper.h"
#include "server/database/table-inventory.h"
#include "interactionqueue.h"

#include "interaction.h"
#include "interactionutility.h"

#define IM "InteractionManager: "

#ifdef WIN32
  // To support sleep()
  #include "Windows.h"
  #define sleep(arg) \
    Sleep(arg);
#endif

#define SLEEP 10

extern "C" void __cxa_pure_virtual()
{
    int *a = NULL;

    printf(IM "Detected virtual funciton - will segfault.\n");
    *a = 5;
}

InteractionManager::InteractionManager()
{
  interactionQueue = new InteractionQueue();
  notificationDistance = 10;
  begin();
}

InteractionManager::~InteractionManager()
{
  kill();
  delete interactionQueue;
}

void InteractionManager::shutdown()
{
  kill();
}

void InteractionManager::Run()
{
  Interaction *interaction = NULL;
  // caller must free allocation
  interaction = interactionQueue->GetInteraction();
  if (!interaction) {
    // No character have any outstanding interactions.
    sleep(SLEEP);
  }
  PerformInteraction(interaction);
  free(interaction);
}

bool
InteractionManager::NormalAttack(Interaction *interaction)
{
  unsigned int targetID = 0;
  const Character* c_char = NULL;
  int damage = 0;

  ptScopedMonitorable<Character>
    lockedAttacker(interaction->character);

  targetID = lockedAttacker->GetTargetID();

  if (targetID == 0) {
    printf(IM "\n\n Please file a bug or notify the peragro team at #peragro "
           "irc.freenode.net: targetID 0, make sure that's not a legal one\n");
    return false;
  }

  c_char = GetTargetCharacter(lockedAttacker);

  if (!c_char) {
    return false;
  }

  ptScopedMonitorable<Character> lockedTarget(c_char);

  // For normal attack it is not legal to attack the own character.
  if (lockedAttacker->getId() == lockedTarget->getId()) {
    return false;
  }

  if (!TargetAttackable(lockedAttacker, lockedTarget)) {
    return false;
  }

  // Make sure the character has enough stamina to do action
  if (!DeductStamina(lockedAttacker, interaction)) {
    return false;
  }

  damage = CalculateDamage(lockedAttacker, lockedTarget);

  Stat* hp = Server::getServer()->getStatManager()->
    findByName(ptString("Health", strlen("Health")));

  CharacterStats* stats = lockedAttacker->getStats();
  printf("CombatManager: HP before deduction: %d\n", stats->getAmount(hp));
  stats->takeStat(hp, static_cast<int>(damage));
  printf("CombatManager: HP after deduction: %d\n", stats->getAmount(hp));



  if (static_cast<int>(stats->getAmount(hp)) < damage)
  {
    ReportDeath(lockedTarget);
  } else {
    if (damage > 0 ) {
      ReportDamage(lockedTarget);
    }
  }
  return true;
}

void
InteractionManager::ReportDeath(Character *lockedCharacter)
{
  DeathMessage msg;
  ByteStream statsbs;
  msg.setEntityId(lockedCharacter->getEntity()->getId());
  msg.serialise(&statsbs);
  // Report the death to everyone nearby.
  NetworkHelper::distancecast(statsbs, 
                              lockedCharacter->getEntity(), 
                              GetNotificationDistance());
  DropAllItems(lockedCharacter);
}

void 
InteractionManager::DropAllItems(Character *lockedCharacter) {
  
  int itemsToDrop = 0;
  int itemsDropped = 0;

  // Player died, need to spawn all the items.
  Inventory* inventory = lockedCharacter->getInventory();
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
    // Save this information here since the entry will get invalid, if we
    // take the item.
    unsigned int itemId = entry->id;
    unsigned int variation = entry->variation;
    if (inventory->takeItem(slot))
    {
      // TODO
      // Remove all items from the players slot
      // Make sure to send those updates
      EquipMessage unequip_msg;
      unequip_msg.setEntityId(lockedCharacter->getEntity()->getId());
      unequip_msg.setSlotId(slot);
      unequip_msg.setItemId(Item::NoItem); // No Item!
      unequip_msg.setFileName(ptString::Null);
      unequip_msg.setMeshName(ptString::Null);
      ByteStream bs;
      unequip_msg.serialise(&bs);
      NetworkHelper::localcast(bs, lockedCharacter->getEntity());

      // Create new entity from item.
      ItemEntity* e = new ItemEntity();
      e->createFromItem(itemId, variation);

      ptScopedMonitorable<Entity> ent(e->getEntity());
      // Release items in a circlular pattern
      const float radians = (2.0f*3.14f / itemsToDrop) * itemsDropped;
      const float radius = 0.8f;
      const PtVector3 delta(cos(radians) * radius, sin(radians) * radius,
        0.0f);
      ent->setPos(lockedCharacter->getEntity()->getPos() + delta);
      ent->setSector(lockedCharacter->getEntity()->getSector());

      itemsDropped++;
      Server::getServer()->addEntity(ent, true);
    }
  }
}

void InteractionManager::SetNotificationDistance(unsigned int distance)
{
  notificationDistance = distance;
}

unsigned int InteractionManager::GetNotificationDistance()
{
  return notificationDistance;
}

void
InteractionManager::ReportDamage(Character *lockedCharacter)
{
  CharacterStats* stats = lockedCharacter->getStats();
  Stat* hp = Server::getServer()->getStatManager()->
    findByName(ptString("Health", strlen("Health")));

  StatsChangeMessage msg;
  ByteStream statsbs;
  msg.setStatId(hp->getId());
  msg.setEntityId(lockedCharacter->getEntity()->getId());
  msg.setName(ptString("Health", strlen("Health")));
  msg.setLevel(stats->getAmount(hp));
  msg.serialise(&statsbs);
  // Report the damage to everyone nearby.
  NetworkHelper::distancecast(statsbs, 
                              lockedCharacter->getEntity(), 
                              GetNotificationDistance());
}

bool
InteractionManager::DeductStamina(Character* lockedCharacter,
                                  Interaction *interaction)
{
  float currentStamina = 0;
  float staminaDeduction = static_cast<int>(GetWeaponHeft(lockedCharacter) /
                                            GetStrength(lockedCharacter) +
                                            interaction->staminaRequired);

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

unsigned int
InteractionManager::GetAttackChance(Character* lockedAttacker,
                                     Character* lockedTarget)
{
  return GetAgility(lockedAttacker) * GetSkillBonus(lockedAttacker) -
    PT::Math::MinUI(GetAgility(lockedTarget), GetSapience(lockedTarget)) *
    PT::Math::MaxUI(PT::Math::MaxUI(GetBlock(lockedTarget),
      GetDodge(lockedTarget)), GetParry(lockedTarget));
}

int
InteractionManager::CalculateDamage(Character* lockedAttacker,
                                         Character* lockedTarget)
{

  int damage = 0;
  const unsigned int attackResult = RollDice();
  const unsigned int attackChance = GetAttackChance(lockedAttacker,
                                                    lockedTarget);

  if (attackResult <= attackChance)
  {
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

  printf("Damage:%d\n", damage);
  printf("attackChance is set to:%d\n", attackChance);
  printf("attackResult is set to:%d\n", attackResult);
  printf("weapondamage is set to:%d\n", GetWeaponDamage(lockedAttacker));
  printf("strength is set to:%d\n", GetStrength(lockedAttacker));

  return damage;
}

bool
InteractionManager::PerformInteraction(Interaction* interaction)
{
  if (!interaction) {
    return false;
  }

  switch(interaction->interactionID) {
    case InteractionID::NORMAL_ATTACK:
      NormalAttack(interaction);
      break;
    case InteractionID::HEAL:
      break;
  }
  return true;
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

  attackerRotation = 
    PT::Math::NormalizeAngle(lockedAttacker->getEntity()->getRotation());

  if (difference.x == 0.0f) difference.x = PT_EPSILON;
  attackAngle = PT::Math::NormalizeAngle(atan2(difference.x, difference.z));

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
     
  interactionQueue->RemoveAllInteractions(lockedSource);
  lockedSource->SetTargetID(targetID);
  return true;
}

const Character*
InteractionManager::GetTargetCharacter(Character* lockedCharacter)
{
  const Character* c_char = NULL;
  const Entity* targetEntity = Server::getServer()->getEntityManager()->
                              findById(lockedCharacter->GetTargetID());

  if (!targetEntity) {
    printf(IM "Invalid target\n");
    return NULL;
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
  }
  return c_char;

}

bool
InteractionManager::QueueInteraction(const PcEntity *sourceEntity,
                                     unsigned int interactionID)
{
  Interaction *interaction = new Interaction();

  printf(IM "Got queueInteraction request, interaction: %d'n", interactionID);

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

  interaction->interactionID = interactionID;
  interaction->character = sourceEntity->getCharacter();

  // Caller must alloc interaction
  interactionQueue->SetInteraction(interaction);
  return true;
}

unsigned int InteractionManager::GetBlock(Character* lockedCharacter)
{
  return InteractionUtility::GetStatValue(lockedCharacter, "Block") +
         InteractionUtility::GetStatValueForAllEquipedItems(lockedCharacter,
                                                            "Block");
} 

unsigned int InteractionManager::GetDodge(Character* lockedCharacter)
{
  return InteractionUtility::GetStatValue(lockedCharacter, "Dodge") +
         InteractionUtility::GetStatValueForAllEquipedItems(lockedCharacter,
                                                            "Dodge");
}

unsigned int InteractionManager::GetParry(Character* lockedCharacter)
{
  return InteractionUtility::GetStatValue(lockedCharacter, "Parry") +
         InteractionUtility::GetStatValueForAllEquipedItems(lockedCharacter,
                                                            "Parry");
}

unsigned int InteractionManager::GetStrength(Character* lockedCharacter)
{
  return InteractionUtility::GetStatValue(lockedCharacter, "Strength") +
         InteractionUtility::GetStatValueForAllEquipedItems(lockedCharacter,
                                                            "Strength");
}

unsigned int InteractionManager::GetReach(Character* lockedCharacter)
{
  return InteractionUtility::GetStatValue(lockedCharacter, "Reach") +
         InteractionUtility::GetStatValueForAllEquipedItems(lockedCharacter, 
                                                            "Reach");
}

unsigned int InteractionManager::GetWeaponDamage(Character* lockedCharacter)
{
  return GetStatValueForEquipedWeapons(lockedCharacter, "Damage");
}

unsigned int InteractionManager::GetStatValueForEquipedWeapons(Character* lockedCharacter,
                                                   const char* statName)
{
  Inventory* inventory = lockedCharacter->getInventory();
  if (!inventory)
  {
    return 0;
  }

  unsigned int value = 0;
  for (unsigned char slot = 0; slot < inventory->NoSlot; slot++)
  {
    Item* item = InteractionUtility::GetItem(lockedCharacter, slot);
    if (!item)
    {
      continue;
    }
    if (item->getType() == ptString("Weapon", strlen("Weapon")))
    {
      value += InteractionUtility::GetStatValueForItem(item, statName);
    }
  }

  return value;
}

unsigned int InteractionManager::GetAgility(Character* lockedCharacter)
{
  return InteractionUtility::GetStatValue(lockedCharacter, "Agility") +
         InteractionUtility::GetStatValueForAllEquipedItems(lockedCharacter,
                                                            "Agility");
}

unsigned int InteractionManager::GetSkillBonus(Character* lockedCharacter)
{
  return 8;
}

unsigned int InteractionManager::GetSapience(Character* lockedCharacter)
{
  return InteractionUtility::GetStatValue(lockedCharacter, "Sapience") +
         InteractionUtility::GetStatValueForAllEquipedItems(lockedCharacter, "Sapience");
} 

unsigned int InteractionManager::GetWeaponHeft(Character* lockedCharacter)
{
  return GetStatValueForEquipedWeapons(lockedCharacter, "Heft");
}

unsigned int InteractionManager::RollDice()
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

