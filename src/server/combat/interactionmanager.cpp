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

#include "common/network/entitymessages.h"

#include "server/entity/entitymanager.h"
#include "server/entity/character/character.h"

#include "common/util/math.h"
#include "common/util/sleep.h"
#include "common/network/playermessages.h"
#include "common/network/combatmessages.h"
#include "common/network/resourcemessages.h"
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
  while(true) 
  {
    Interaction *interaction = 0;
    // caller must free allocation
    interaction = interactionQueue->GetInteraction();
    while (!interaction) 
    {
      // No character have any outstanding interactions.
      pt_sleep(SLEEP);
      interaction = interactionQueue->GetInteraction();
    }
    PerformInteraction(interaction);
    delete interaction;
  }
}

bool InteractionManager::NormalAttack(Interaction *interaction)
{
  DEBUG("NormalAttack");
  unsigned int targetID = 0;
  boost::shared_ptr<Character> target;
  int damage = 0;

  boost::shared_ptr<Character> attacker = interaction->character.lock();

  targetID = attacker->GetTargetID();

  if (targetID == 0) {
    printf(IM "\n\n Please file a bug or notify the peragro team at #peragro "
           "irc.freenode.net: targetID 0, make sure that's not a legal one\n");
    return false;
  }

  target = GetTargetCharacter(attacker);
  if (!target) return false;

  // For normal attack it is not legal to attack the own character.
  if (attacker->GetId() == target->GetId())
    return false;

  if (!TargetAttackable(attacker, target))
    return false;

  // Make sure the character has enough stamina to do action
  if (!DeductStamina(attacker, interaction))
    return false;

  damage = CalculateDamage(attacker, target);

  printf("CombatManager: HP before deduction: %d\n", target->GetResources()->Get("Hit Points"));
  target->GetResources()->Sub("Hit Points", static_cast<int>(damage));
  printf("CombatManager: HP after deduction: %d\n", target->GetResources()->Get("Hit Points"));

  if (target->GetResources()->Get("Hit Points") <= 0)
  {
    ReportDeath(target);
  } 
  else if (damage > 0 ) 
  {
    ReportDamage(target);
  }

  return true;
}

void InteractionManager::ReportDeath(boost::shared_ptr<Character> character)
{
  DEBUG("ReportDeath");
  DeathMessage msg;
  ByteStream statsbs;
  msg.setEntityId(character->GetId());
  msg.serialise(&statsbs);
  // Report the death to everyone nearby.
  NetworkHelper::localcast(statsbs, character);
  DropAllItems(character);
}

void InteractionManager::DropAllItems(boost::shared_ptr<Character> character)
{
/*
  DEBUG("DropAllItems");

  int itemsToDrop = 0;
  int itemsDropped = 0;

  // Player died, need to spawn all the items.
  Inventory* inventory = character->getInventory();
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
    if (inventory->takeItem(slot))
    {
      // TODO
      // Remove all items from the players slot
      // Make sure to send those updates
      EquipMessage unequip_msg;
      unequip_msg.setEntityId(character->GetId());
      unequip_msg.setSlotId(slot);
      unequip_msg.setItemId(0); // No Item! Item::NoItem
      unequip_msg.setFileName(ptString::Null);
      unequip_msg.setMeshName(ptString::Null);
      ByteStream bs;
      unequip_msg.serialise(&bs);
      NetworkHelper::localcast(bs, character);

      // Create new entity from item.
      ItemEntity* e = new ItemEntity();
      e->createFromItem(itemId);

      ptScopedMonitorable<Entity> ent(e->getEntity());
      // Release items in a circlular pattern
      const float radians = (2.0f*3.14f / itemsToDrop) * itemsDropped;
      const float radius = 0.8f;
      const WFMath::Vector<3> delta(cos(radians) * radius, sin(radians) * radius,
        0.0f);
      ent->SetPosition(WFMath::Point<3>(character->getEntity()->GetPosition() + delta));
      ent->SetSector(character->getEntity()->GetSector());

      itemsDropped++;
      Server::getServer()->addEntity(ent, true);
    }
  }
*/
}

void InteractionManager::ReportDamage(boost::shared_ptr<Character> character)
{
  DEBUG("ReportDamage");
  size_t hpId = Server::getServer()->GetResourcesFactory()->GetID("Hit Points");
  int hp = character->GetResources()->Get("Hit Points");
  int hpMax = character->GetResources()->GetMax("Hit Points");

  ResourceUpdateMessage msg;
  ByteStream statsbs;
  msg.setEntityId(character->GetId());
  msg.setResourceId(hpId);
  msg.setValue(hp);
  msg.setMaxValue(hpMax);
  msg.serialise(&statsbs);
  // Report the damage to everyone nearby.
  NetworkHelper::localcast(statsbs, character);
}

bool InteractionManager::DeductStamina(boost::shared_ptr<Character> character,
                                  Interaction *interaction)
{
  DEBUG("DeductStamin");

  float currentStamina = 0;
  float staminaDeduction = static_cast<int>(GetWeaponHeft(character) /
                                            GetStrength(character) +
                                            interaction->staminaRequired);

  currentStamina = character->GetResources()->Get("Stamina");

  printf(IM "Stamina before deduction: %f\n", currentStamina);

  if (currentStamina < staminaDeduction) 
  { // Not enough stamina
    return false; 
  }

  character->GetResources()->Sub("Stamina", (int)staminaDeduction);

  return true;
}

unsigned int InteractionManager::GetAttackChance(boost::shared_ptr<Character> attacker,
                                                  boost::shared_ptr<Character> target)
{
  DEBUG("GetAttackChance");
  return GetAgility(attacker) * GetSkillBonus(attacker) -
    PT::Math::MinUI(GetAgility(target), GetSapience(target)) *
    PT::Math::MaxUI(PT::Math::MaxUI(GetBlock(target),
      GetDodge(target)), GetParry(target));
}

int InteractionManager::CalculateDamage(boost::shared_ptr<Character> attacker,
                                         boost::shared_ptr<Character> target)
{
  DEBUG("CalculateDamage");

  int damage = 0;
  const unsigned int attackResult = RollDice();
  const unsigned int attackChance = GetAttackChance(attacker,
                                                    target);

  if (attackResult <= attackChance)
  {
    damage = (attackChance - attackResult) *
      GetWeaponDamage(attacker) +
      GetStrength(attacker);
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
  printf("weapondamage is set to:%d\n", GetWeaponDamage(attacker));
  printf("strength is set to:%d\n", GetStrength(attacker));

  return damage;
}

bool InteractionManager::PerformInteraction(Interaction* interaction)
{
  DEBUG("PerformInteraction");
  if (!interaction) 
  {
    printf(IM "BUG this should not have happened, interaction  == NULL\n");
    return false;
  }

  printf("\nPerforming interaction with ID: %d\n", interaction->interactionID);

  switch(interaction->interactionID) 
  {
    case InteractionID::NORMAL_ATTACK:
      NormalAttack(interaction);
      break;
    case InteractionID::HEAL:
      break;
  }
  return true;
}

bool InteractionManager::TargetAttackable(boost::shared_ptr<Character> attacker,
                                          boost::shared_ptr<Character> target)
{
  DEBUG("TargetAttackable");
  float maxAttackDistance = 0;
  float distance = 0;
  float attackerRotation = 0;
  float attackAngle = 0;
  float angleDiff = 0;
  static const float allowedAngle = PT_PI * 0.3f;
  const WFMath::Point<3> attackerPos(attacker->GetPosition());
  const WFMath::Point<3> tarGetPosition(target->GetPosition());

  maxAttackDistance = GetReach(attacker);
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
    PT::Math::NormalizeAngle(attacker->GetRotation());

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

bool InteractionManager::SelectTarget(boost::shared_ptr<PcEntity> sourceEntity,
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

boost::shared_ptr<Character> InteractionManager::GetTargetCharacter(boost::shared_ptr<Character> character)
{
  DEBUG("GetTargetCharacter");
  Common::Entity::Entityp targetEntity = Server::getServer()->getEntityManager()->
                              FindById(character->GetTargetID());

  if (!targetEntity)
  {
    printf(IM "Invalid target\n");
    return boost::shared_ptr<Character>();
  }

  if ((targetEntity->GetType() != Common::Entity::PlayerEntityType)
      || (targetEntity->GetType() != Common::Entity::NPCEntityType))
  {
    // Should not happen, but do not crash on release build, since fake message
    // could bring down the server then.
    printf(IM "Target neither player nor npc\n");
    return boost::shared_ptr<Character>();
  }

  return boost::shared_dynamic_cast<Character>(targetEntity);
}

bool InteractionManager::QueueInteraction(boost::shared_ptr<PcEntity> sourceEntity,
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

unsigned int InteractionManager::GetBlock(boost::shared_ptr<Character> character)
{
  return InteractionUtility::GetStatValue(character, "Block") +
         InteractionUtility::GetStatValueForAllEquipedItems(character,
                                                            "Block");
}

unsigned int InteractionManager::GetDodge(boost::shared_ptr<Character> character)
{
  return InteractionUtility::GetStatValue(character, "Dodge") +
         InteractionUtility::GetStatValueForAllEquipedItems(character,
                                                            "Dodge");
}

unsigned int InteractionManager::GetParry(boost::shared_ptr<Character> character)
{
  return InteractionUtility::GetStatValue(character, "Parry") +
         InteractionUtility::GetStatValueForAllEquipedItems(character,
                                                            "Parry");
}

unsigned int InteractionManager::GetStrength(boost::shared_ptr<Character> character)
{
  return InteractionUtility::GetStatValue(character, "Strength") +
         InteractionUtility::GetStatValueForAllEquipedItems(character,
                                                            "Strength");
}

unsigned int InteractionManager::GetReach(boost::shared_ptr<Character> character)
{
  return InteractionUtility::GetStatValue(character, "Reach") +
         InteractionUtility::GetStatValueForAllEquipedItems(character,
                                                            "Reach") +
         5;   // TODO This should not be here, just till the DB is setup
              // to correctly set Reach to something...
}

unsigned int InteractionManager::GetWeaponDamage(boost::shared_ptr<Character> character)
{
  return GetStatValueForEquipedWeapons(character, "Damage");
}

unsigned int InteractionManager::GetStatValueForEquipedWeapons(boost::shared_ptr<Character> character,
                                                   const std::string& statName)
{
  unsigned int value = 0;
  boost::shared_ptr<Inventory> inventory = character->GetInventory();
  if (!inventory)
  {
    return value;
  }

/*
  for (unsigned char slot = 0; slot < inventory->NoSlot; slot++)
  {
    Item* item = InteractionUtility::GetItem(character, slot);
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

unsigned int InteractionManager::GetAgility(boost::shared_ptr<Character> character)
{
  return InteractionUtility::GetStatValue(character, "Agility") +
         InteractionUtility::GetStatValueForAllEquipedItems(character,
                                                            "Agility");
}

unsigned int InteractionManager::GetSkillBonus(boost::shared_ptr<Character> character)
{
  return 8;
}

unsigned int InteractionManager::GetSapience(boost::shared_ptr<Character> character)
{
  return InteractionUtility::GetStatValue(character, "Sapience") +
         InteractionUtility::GetStatValueForAllEquipedItems(character, "Sapience");
}

unsigned int InteractionManager::GetWeaponHeft(boost::shared_ptr<Character> character)
{
  return GetStatValueForEquipedWeapons(character, "Heft");
}

unsigned int InteractionManager::RollDice()
{
  return rand() % 101;
}