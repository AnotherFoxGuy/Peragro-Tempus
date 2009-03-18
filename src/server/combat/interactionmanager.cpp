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


#include <wfmath/point.h>
#include <wfmath/vector.h>
#include <wfmath/stream.h>

#include "server/server.h"
#include "interactionmanager.h"
#include "interaction.h"
#include "server/entity/entitymanager.h"
#include "common/network/entitymessages.h"

#include "common/entity/entity.h"

#include "common/util/math.h"
#include "common/util/sleep.h"
#include "common/network/playermessages.h"
#include "common/network/combatmessages.h"
#include "server/network/networkhelper.h"
#include "server/database/table-inventory.h"
#include "interactionqueue.h"

#include "interaction.h"
#include "interactionutility.h"

#define IM "InteractionManager: "
#define DEBUG(arg) printf(IM #arg "\n");

#define SLEEP 100

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
  while(true) {
    Interaction *interaction = NULL;
    // caller must free allocation
    interaction = interactionQueue->GetInteraction();
    while (!interaction) {
      // No character have any outstanding interactions.
      pt_sleep(SLEEP);
      interaction = interactionQueue->GetInteraction();
    }
    PerformInteraction(interaction);
    free(interaction);
  }
}

bool
InteractionManager::NormalAttack(Interaction *interaction)
{
  DEBUG("NormalAttack");
  unsigned int targetID = 0;
  Character* lockedTarget = 0;
  int damage = 0;

  Character* lockedAttacker = interaction->character;

  targetID = lockedAttacker->GetTargetID();

  if (targetID == 0) {
    printf(IM "\n\n Please file a bug or notify the peragro team at #peragro "
           "irc.freenode.net: targetID 0, make sure that's not a legal one\n");
    return false;
  }

  lockedTarget = GetTargetCharacter(lockedAttacker);
  if (!lockedTarget) return false;

  // For normal attack it is not legal to attack the own character.
  if (lockedAttacker->GetId() == lockedTarget->GetId())
    return false;

  if (!TargetAttackable(lockedAttacker, lockedTarget))
    return false;

  // Make sure the character has enough stamina to do action
  if (!DeductStamina(lockedAttacker, interaction))
    return false;

  damage = CalculateDamage(lockedAttacker, lockedTarget);

  /*
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
  */
  return true;
}

void
InteractionManager::ReportDeath(Character *lockedCharacter)
{
  DEBUG("ReportDeath");
  DeathMessage msg;
  ByteStream statsbs;
  msg.setEntityId(lockedCharacter->GetId());
  msg.serialise(&statsbs);
  // Report the death to everyone nearby.
  NetworkHelper::distancecast(statsbs,
                              lockedCharacter,
                              GetNotificationDistance());
  DropAllItems(lockedCharacter);
}

void InteractionManager::DropAllItems(Character *lockedCharacter)
{
/*
  DEBUG("DropAllItems");

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
      unequip_msg.setEntityId(lockedCharacter->GetId());
      unequip_msg.setSlotId(slot);
      unequip_msg.setItemId(0); // No Item! Item::NoItem
      unequip_msg.setFileName(ptString::Null);
      unequip_msg.setMeshName(ptString::Null);
      ByteStream bs;
      unequip_msg.serialise(&bs);
      NetworkHelper::localcast(bs, lockedCharacter);

      // Create new entity from item.
      ItemEntity* e = new ItemEntity();
      e->createFromItem(itemId, variation);

      ptScopedMonitorable<Entity> ent(e->getEntity());
      // Release items in a circlular pattern
      const float radians = (2.0f*3.14f / itemsToDrop) * itemsDropped;
      const float radius = 0.8f;
      const WFMath::Vector<3> delta(cos(radians) * radius, sin(radians) * radius,
        0.0f);
      ent->SetPosition(WFMath::Point<3>(lockedCharacter->getEntity()->GetPosition() + delta));
      ent->SetSector(lockedCharacter->getEntity()->GetSector());

      itemsDropped++;
      Server::getServer()->addEntity(ent, true);
    }
  }
*/
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
  DEBUG("ReportDamage");
/*
  CharacterStats* stats = lockedCharacter->getStats();
  Stat* hp = Server::getServer()->getStatManager()->
    findByName(ptString("Health", strlen("Health")));

  StatsChangeMessage msg;
  ByteStream statsbs;
  msg.setStatId(hp->GetId());
  msg.setEntityId(lockedCharacter->GetId());
  msg.setName(ptString("Health", strlen("Health")));
  msg.setLevel(stats->getAmount(hp));
  msg.serialise(&statsbs);
  // Report the damage to everyone nearby.
  NetworkHelper::distancecast(statsbs,
                              lockedCharacter,
                              GetNotificationDistance());
*/
}

bool
InteractionManager::DeductStamina(Character* lockedCharacter,
                                  Interaction *interaction)
{
  DEBUG("DeductStamin");

  float currentStamina = 0;
  float staminaDeduction = static_cast<int>(GetWeaponHeft(lockedCharacter) /
                                            GetStrength(lockedCharacter) +
                                            interaction->staminaRequired);
/*
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
*/
  return true;
}

unsigned int
InteractionManager::GetAttackChance(Character* lockedAttacker,
                                     Character* lockedTarget)
{
  DEBUG("GetAttackChance");
  return GetAgility(lockedAttacker) * GetSkillBonus(lockedAttacker) -
    PT::Math::MinUI(GetAgility(lockedTarget), GetSapience(lockedTarget)) *
    PT::Math::MaxUI(PT::Math::MaxUI(GetBlock(lockedTarget),
      GetDodge(lockedTarget)), GetParry(lockedTarget));
}

int
InteractionManager::CalculateDamage(Character* lockedAttacker,
                                         Character* lockedTarget)
{
  DEBUG("CalculateDamage");

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
  DEBUG("PerformInteraction");
  if (!interaction) {
    printf(IM "BUG this should not have happened, interaction  == NULL\n");
    return false;
  }

  printf("\nPerforming interaction with ID: %d\n", interaction->interactionID);

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
  DEBUG("TargetAttackable");
  float maxAttackDistance = 0;
  float distance = 0;
  float attackerRotation = 0;
  float attackAngle = 0;
  float angleDiff = 0;
  static const float allowedAngle = PT_PI * 0.3f;
  const WFMath::Point<3> attackerPos(lockedAttacker->GetPosition());
  const WFMath::Point<3> tarGetPosition(lockedTarget->GetPosition());

  maxAttackDistance = GetReach(lockedAttacker);
  distance = Distance(attackerPos, tarGetPosition);

  printf(IM "attackerPos: %s, tarGetPosition: %s, distance %f, "
    "maxAttackDistance: %f\n", WFMath::ToString(attackerPos).c_str(),
    WFMath::ToString(tarGetPosition).c_str(), distance, maxAttackDistance);

  if (distance > maxAttackDistance)
  {
    // Target out of reach
    printf(IM "Target out of range\n");
    return false;
  }

  WFMath::Point<3> difference(attackerPos - tarGetPosition);

  attackerRotation =
    PT::Math::NormalizeAngle(lockedAttacker->GetRotation());

  if (difference[0] == 0.0f) difference[0] = PT_EPSILON;
  attackAngle = PT::Math::NormalizeAngle(atan2(difference[0], difference[2]));

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

bool InteractionManager::SelectTarget(PcEntity* sourceEntity,
                                 unsigned int targetID)
{
  DEBUG("SelectTarget");
  printf(IM "Got selection request, target: %d'n", targetID);

  if (!sourceEntity)
  {
    // Invalid source.
    printf(IM "Invalid source.\n");
    return false;
  }

  interactionQueue->RemoveAllInteractions(sourceEntity);
  sourceEntity->SetTargetID(targetID);
  return true;
}

Character* InteractionManager::GetTargetCharacter(Character* lockedCharacter)
{
  DEBUG("GetTargetCharacter");
  Common::Entity::Entityp targetEntity = Server::getServer()->getEntityManager()->
                              FindById(lockedCharacter->GetTargetID());

  if (!targetEntity)
  {
    printf(IM "Invalid target\n");
    return 0;
  }

  if ((targetEntity->GetType() != Common::Entity::PlayerEntityType)
      || (targetEntity->GetType() != Common::Entity::NPCEntityType))
  {
    // Should not happen, but do not crash on release build, since fake message
    // could bring down the server then.
    printf(IM "Target neither player nor npc\n");
    return 0;
  }

  return (Character*)targetEntity.get();
}

bool InteractionManager::QueueInteraction(PcEntity *sourceEntity,
                                          unsigned int interactionID)
{
  DEBUG("QueueInteraction");
  Interaction* interaction = new Interaction();

  printf(IM "Got queueInteraction request, interaction: %d\n", interactionID);

  if (!sourceEntity)
  {
    // Invalid source.
    printf(IM "Invalid source.\n");
    return false;
  }

  interaction->interactionID = interactionID;
  interaction->character = sourceEntity;

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
                                                            "Reach") +
         5;   // TODO This should not be here, just till the DB is setup
              // to correctly set Reach to something...
}

unsigned int InteractionManager::GetWeaponDamage(Character* lockedCharacter)
{
  return GetStatValueForEquipedWeapons(lockedCharacter, "Damage");
}

unsigned int InteractionManager::GetStatValueForEquipedWeapons(Character* lockedCharacter,
                                                   const char* statName)
{
  unsigned int value = 0;
  boost::shared_ptr<Inventory> inventory = lockedCharacter->GetInventory();
  if (!inventory)
  {
    return value;
  }

/*
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
*/

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

/*
void
InteractionManager::SendStatUpdate(const Stat* stat,
                                   const CharacterStats* stats,
                                   Character* lockedCharacter,
                                   const char* name,
                                   int target)
{
  DEBUG("SendStatUpdate");
  StatsChangeMessage msg;
  ByteStream statsbs;
  msg.setStatId(stat->GetId());
  msg.setEntityId(lockedCharacter->GetId());
  msg.setName(ptString(name, strlen(name)));
  msg.setLevel(stats->getAmount(stat));
  msg.serialise(&statsbs);
  if (target == InteractionManagerSendTo::BROADCAST)
  {
    NetworkHelper::broadcast(statsbs);
  }
  else if (target == InteractionManagerSendTo::LOCALCAST)
  {
    NetworkHelper::localcast(statsbs, lockedCharacter);
  }
  else if (target == InteractionManagerSendTo::CHARACTER)
  {
    NetworkHelper::sendMessage(lockedCharacter, statsbs);
  }
}
*/
