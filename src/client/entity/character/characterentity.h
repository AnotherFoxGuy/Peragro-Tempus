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

#ifndef PTCHARACTERENTITY_H
#define PTCHARACTERENTITY_H

#include "client/entity/entity.h"

#include "client/entity/character/equipment/equipment.h"

namespace PT
{
  namespace Entity
  {
    /**
     * @ingroup entities
     * Implements an abstract class for various character-like entities (like
     * players, NPCs, horses, or even boats). These entities have in common that
     * they can have equipment, can move etc.
     */
    class CharacterEntity : public Entity
    {
    private:
      ///Equipment that character entity carries/owns.
      Equipment equipment;
      ///Time of last update of character's statistics.
      csTicks lastStatUpdate;

    protected:
      /**
       * Convenience constructor for use in children classes.
       */
      CharacterEntity() : equipment(this) {}
      /**
       * Constructor that sets up the character using the information provided
       * by EntityAddEvent event.
       * @see Entity::Entity(const Events::EntityAddEvent&)
       * @param ev Event used for initialising the character properties.
       */
      CharacterEntity(const Events::EntityAddEvent& ev);

      ///@todo Stats shouldn't be defined like this.
      float maxStamina;
      ///@todo Stats shouldn't be defined like this.
      float currentStamina;
      ///@todo Stats shouldn't be defined like this.
      float recoverStamina; // + stamina/ms
      ///@todo Stats shouldn't be defined like this.
      float drainStamina;   // - stamina/ms
      ///Specifies whether character is sitting or not.
      ///@todo It's not very logical to have boats sit, is it? (boats will
      ///probably be MountEntities).
      bool sitting;

    public:
      void Move(const MovementData& movement);
      bool MoveTo(MoveToData* moveTo);
      void DrUpdate(const DrUpdateData& drupdate);
      void Teleport(const csVector3& pos, const std::string& sector);

      ///@todo Stats shouldn't be defined like this.
      float GetCurrentStamina() const { return currentStamina; }
      ///@todo Stats shouldn't be defined like this.
      void SetCurrentStamina(float x);
      ///@todo Stats shouldn't be defined like this.
      float GetMaxStamina() const { return maxStamina; }
      ///@todo Stats shouldn't be defined like this.
      void SetMaxStamina(float x) { maxStamina = x; }

      ///@return Reference to character's equipment.
      ///@todo Maybe move 'equipment' property to public?
      Equipment& GetEquipment() { return equipment; }

      void Pose(unsigned int poseId);

      /**
       * Plays a character animation.
       * @todo Document what blend_factor is.
       * @param animationName Name of the animation to be played.
       * @param blend_factor Blah?
       * @param loop Specify whether the animation should loop or not.
       * @param stopOthers Specify whether other animations should be stopped or
       * not
       */
      void PlayAnimation(const char* animationName, float blend_factor = 0.0f,
                         bool loop = false, bool stopOthers = false);
      /**
       * Updates all character statistics.
       * @todo This currently only does stamina. Whole statistics system needs
       * some change.
       */
      void UpdatePlayerStats();
    };
  }
}

#endif // PTCHARACTERENTITY_H
