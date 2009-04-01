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
#include "client/entity/character/resource/resource.h"

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
    class CharacterEntity : public ::Client::Entity::Entity
    {
    protected:
      virtual void Initialize(const iEvent& ev);

    protected:
      ///Equipment that character entity carries/owns.
      Equipment equipment;
      ///Time of last update of character's statistics.
      csTicks lastStatUpdate;

      boost::shared_ptr<ResourcesFactory> resourcesFact;
      boost::shared_ptr<Resources> resources;

    protected:
      /**
       * Constructor.
       */
      CharacterEntity(Common::Entity::EntityType type);
      virtual ~CharacterEntity() {}

      ///Specifies whether character is sitting or not.
      ///@todo It's not very logical to have boats sit, is it? (boats will
      ///probably be MountEntities).
      bool sitting;
      ///Whether character is on a mount or not.
      bool hasMount;

    public:
      void Teleport(const WFMath::Point<3>& pos, float rotation, const std::string& sector);

      ///@todo 
      boost::shared_ptr<Resources> GetResources() const { return resources; }
      bool GetResourcesReady() const { return resourcesFact->listRecieved; }

      ///@return Reference to character's equipment.
      ///@todo Maybe move 'equipment' property to public?
      Equipment& GetEquipment() { return equipment; }

      bool IsSitting() { return sitting; }

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
      void PlayAnimation(const char* animationName, float blend_factor = 0.01f,
                         bool loop = false, bool stopOthers = false);
      /**
       * Updates all character statistics.
       * @todo This currently only does stamina. Whole statistics system needs
       * some change.
       */
      void UpdatePlayerStats();

      /**
       * Notes if the character is on a mount.
       * @param v Value
       */
      void SetHasMount(bool v){ hasMount=v; }
      /**
       * Tells us if the character is on a mount.
       * @return If the character is on a mount
       */
      bool GetHasMount(){ return hasMount; }
    };
  }
}

#endif // PTCHARACTERENTITY_H
