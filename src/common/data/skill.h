/*
    Copyright (C) 2007 Development Team of Peragro Tempus

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

#ifndef PT_DATASKILL_H
#define PT_DATASKILL_H

#include <string>

namespace PT
{
  namespace Data
  {

    /**
      @ingroup data_handlers
      Helper class that contains information about skill type. Usually not
      created directly, but accessed via SkillDataManager instance. Use the
      Set/Get methods to access the skill properties.
      @see SkillDataManager
      @author Jelle Hellemans
    */
    class Skill
    {
    public:
      struct Effects
      {
        std::string caster;     // The effect to be attached to the casting entity.
        std::string castanim;   // The animation played by the casting entity.
        std::string targetanim; // The animation played by the target entity.
        std::string target;     // The effect to be attached to the target entity.
        std::string weapon;     // The effect to be attached to the weapon.
        std::string projectile; // The effect for the projectile.
      };

      struct Stats
      {
        unsigned int range;       // The range of the skill.
        unsigned int duration;    // How long the skill lasts.
        unsigned int casttime;    // How long it takes to cast.
        unsigned int reusedelay;  // How long it takes to recharge.
      };

      struct Cost
      {
        unsigned int hp;  // How much health points this skill consumes to perform.
        unsigned int mp;  // How much mana points this skill consumes to perform.
        unsigned int sp;  // How much stamina points this skill consumes to perform.
      };

      struct Damage
      {
        unsigned int hp;  // How much health points it takes away from the target.
        unsigned int mp;  // How much mana points it takes away from the target.
        unsigned int sp;  // How much stamina points it takes away from the target.
      };

      struct Requirements
      {
      };

    private:
      ///Unique ID of the skill type (ie '3').
      unsigned int id;
      ///Name of the skill (ie 'Heal').
      std::string name;
      ///name of the icon used for the skill in GUI view (ie
      ///Set:Skills image:Heal').
      std::string icon;
      ///Description of the skill type (ie 'Rejuvenates the target').
      std::string description;
      ///String that is displayed when performing(ie '... starts healing ...').
      std::string startstring;
      ///String that is displayed when cast(ie '... rejuvenates ...').
      std::string completestring;
      ///Effects uses to display this skill.
      Effects effects;
      ///General attributes of the skill.
      Stats stats;
      ///The caster's cost to perform this skill.
      Cost cost;
      ///Damage that will be dealt on the target by this skill.
      Damage damage;
      ///Requirements to be able to perform this skill.
      Requirements requirements;

    public:
      Skill() : id(0) {}

      void SetId(unsigned int value) { id = value; }
      unsigned int GetId() const { return id; }

      void SetName(const std::string& value) { name=value; }
      const std::string& GetName() const { return name; }

      void SetIconName(const std::string& value) { icon=value; }
      const std::string& GetIconName() const { return icon; }

      void SetDescription(const std::string& value) { description=value; }
      const std::string& GetDescription() const { return description; }

      void SetStartString(const std::string& value) { startstring=value; }
      const std::string& GetStartString() const { return startstring; }

      void SetCompleteString(const std::string& value) { completestring=value; }
      const std::string& GetCompleteString() const { return completestring; }

      void SetEffects(const Effects& value) { effects=value; }
      const Effects& GetEffects() const { return effects; }

      void SetStats(const Stats& value) { stats=value; }
      const Stats& GetStats() const { return stats; }

      void SetCost(const Cost& value) { cost=value; }
      const Cost& GetCost() const { return cost; }

      void SetDamage(const Damage& value) { damage=value; }
      const Damage& GetDamage() const { return damage; }

      void SetRequirements(const Requirements& value) { requirements=value; }
      const Requirements& GetRequirements() const { return requirements; }


    };
  } // Data namespace
} // PT namespace

#endif // PT_DATASKILL_H
