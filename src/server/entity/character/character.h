/*
    Copyright (C) 2005 Development Team of Peragro Tempus

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

#ifndef CHARACTER_H
#define CHARACTER_H

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <wfmath/point.h>

#include "common/utility/colour24.h"

#include "../../server.h"

#include "../entity.h"

#include "inventory.h"
#include "equipment.h"

#include "../statistic/skills.h"
#include "speciesabilities.h"
#include "speciesvulnerabilities.h"
#include "../statistic/reputations.h"
#include "resources.h"

class MountEntity;
class Race;

PT_DEFINE_EXCEPTION(InvalidCharacter);
PT_DEFINE_ERRORINFO(MeshId, size_t);

class Character : public Entity
{
public:
  enum State
  {
    StateProne = 0,
    StateSitting,
    StateStanding,
    StateWalking,
    StateHustling,
    StateRunning,
    StateSprinting,
  };

protected:
  State state;

  unsigned int targetID; // Used by interaction manager to determine target.
  boost::weak_ptr<MountEntity> mount;

  bool isWalking;

  WFMath::Point<3> final_dst;
  size_t t_stop;
  WFMath::Point<3> tmp_pos; //used only for temporary calculations!

  PT::Colour24 haircolour;
  PT::Colour24 skincolour;
  PT::Colour24 decalcolour;

  size_t species;
  float age;

  boost::shared_ptr<Inventory> inventory;
  boost::shared_ptr<Equipment> equipment;

  boost::shared_ptr<Skills> skills;
  boost::shared_ptr<SpeciesAbilities> abilities;
  boost::shared_ptr<SpeciesVulnerabilities> vulnerabilities;
  boost::shared_ptr<Reputations> reputations;
  boost::shared_ptr<Resources> resources;

public:
  Character(Common::Entity::EntityType type)
    : Entity(type), state(StateStanding), haircolour(), skincolour(), decalcolour(), species(0), age(0.0f)
  {
    isWalking = false;
    inventory = boost::shared_ptr<Inventory>(new Inventory(this_));
    equipment = Server::getServer()->GetEquipmentFactory()->Create(this);
    skills = Server::getServer()->GetSkillsFactory()->Create(this);
    abilities = boost::shared_ptr<SpeciesAbilities>(new SpeciesAbilities(this));
    vulnerabilities = boost::shared_ptr<SpeciesVulnerabilities>(new SpeciesVulnerabilities(this));
    reputations = Server::getServer()->GetReputationsFactory()->Create(this);
    resources = Server::getServer()->GetResourcesFactory()->Create(this);
  }

  virtual ~Character()
  {
  }

  void SetMount(boost::shared_ptr<MountEntity> mount);
  boost::shared_ptr<MountEntity> GetMount() const { return mount.lock(); }

  void SetState(State state) { this->state = state; }
  State GetState() { return state; }

  void SetTargetID(unsigned int targetID) { this->targetID = targetID; }
  unsigned int GetTargetID() { return targetID; }

  const PT::Colour24& GetSkinColour() const { return skincolour; }
  void SetSkinColour(const PT::Colour24& colour) { skincolour = colour; }

  const PT::Colour24& GetHairColour() const { return haircolour; }
  void SetHairColour(const PT::Colour24& colour) { haircolour = colour; }

  const PT::Colour24& GetDecalColour() const { return decalcolour; }
  void SetDecalColour(const PT::Colour24& colour) { decalcolour = colour; }

  size_t GetSpecies() const { return species; }
  void SetSpecies(size_t species) { this->species = species; }

  float GetAge() const { return age; }
  void SetAge(float age) { this->age = age; }

  boost::shared_ptr<Inventory> GetInventory() const { return inventory; }
  boost::shared_ptr<Equipment> GetEquipment() const { return equipment; }

  boost::shared_ptr<Skills> GetSkills() const { return skills; }
  boost::shared_ptr<SpeciesAbilities> GetAbilities() const { return abilities; }
  boost::shared_ptr<SpeciesVulnerabilities> GetVulnerabilities() const { return vulnerabilities; }
  boost::shared_ptr<Reputations> GetReputations() const { return reputations; }
  boost::shared_ptr<Resources> GetResources() const { return resources; }

  virtual void walkTo(const WFMath::Point<3>& dst_pos, float speed);
  virtual WFMath::Point<3> GetPosition();

  virtual void LoadFromDB();
  virtual void SaveToDB();
};

#endif // CHARACTER_H
