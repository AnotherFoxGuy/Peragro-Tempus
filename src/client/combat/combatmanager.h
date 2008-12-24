/*
    Copyright (C) 2005-2008 Development Team of Peragro Tempus

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
/**
 * @file combatmanager.h
 *
 * @basic Client combat manager.
 */

#ifndef COMBATMANAGER_H
#define COMBATMANAGER_H

#include <cssysdef.h>
#include <csutil/ref.h>
#include <iutil/vfs.h>
#include <iengine/engine.h>

#include "physicallayer/pl.h"

#include "client/entity/entity.h"

#include "common/util/mutex.h"

#include "client/network/network.h"

#include "client/pointer/pointer.h"

class Effect;
class Network;

struct iObjectRegistry;
struct iLoader;

namespace PT
{
  namespace GUI
  {
    class GUIManager;
  } // GUI namespace

  namespace Combat
  {
    /**
     * Client combat manager.
     */
    class CombatManager
    {
    private:

      /// The CS engine.
      csRef<iEngine> engine;
      /// The CS virtual files system.
      csRef<iVFS> vfs;
      /// The CS loader.
      csRef<iThreadedLoader> loader;
      /// The CEL physical layer.
      csRef<iCelPlLayer> pl;

      /// Pointer array of event handling callbacks.
      csRefArray<PT::Events::EventHandlerCallback> eventHandlers;

      /// The entity manager.
      PT::Entity::EntityManager* entityManager;
      /// The gui manager.
      PT::GUI::GUIManager* guiManager;
      /// The skill manager.
      PT::Data::SkillDataManager* skillManager;
      /// The network.
      Network* network;

      /**
       * Get a CS mesh wrapper from a PT entity.
       * @param entity The PT entity.
       * @return The mesh wrapper.
       */
      iMeshWrapper* GetMesh(PT::Entity::Entity* entity);

      /// Handler for the Hit event.
      bool ActionHit(iEvent& ev);
      /// Will send attack request to the server.
      bool ActionAttackTarget(iEvent& ev);
      /// Adds a stat for the player.
      bool AddStatPlayer(iEvent& ev);
      /// Updates a character stat.
      bool UpdateStat(iEvent& ev);

    public:
      /**
       * Constructor.
       */
      CombatManager();
      /**
       * Destructor.
       */
      ~CombatManager();

      /**
       * Initialize the pointers, register for events.
       * @return True for success.
       */
      bool Initialize();

      /**
       * Do damage to a target.
       * @param target The target entity.
       * @param damage The amount of damage to deal.
       */
      void Hit(PT::Entity::Entity* target, int damage);

      /**
       * Make an entity die.
       * @param target The entity to kill.
       */
      void Die(PT::Entity::Entity* target);

      /**
       * Show the level up effect.
       * @param targetId The ID of the entity.
       */
      void LevelUp(int targetId);

      /**
       * Add experience to the player character.
       * @param exp The amount of experience to add.
       */
      void Experience(int exp);

     /**
      * Start using a skill, so we create an effect on the caster.
      * @param casterId The entity ID of the skill user.
      * @param targetId The entity ID of the skill target.
      * @param skillId The skill ID.
      * @param error If an error happened, the message.
      */
      void SkillUsageStart(unsigned int casterId, unsigned int targetId,
                           int skillId, ptString error);

      /**
       * Skill completed succesfully, so we create the effect on the target.
       * @param casterId The entity ID of the skill user.
       * @param targetId The entity ID of the skill target.
       * @param skillId The skill ID.
       */
      void SkillUsageComplete(unsigned int casterId, unsigned int targetId,
                              int skillId);

      /**
       * Lookup the PT entity ID from a CEL entity and call
       * RequestSkillUsageStart(targetId, skillId).
       * @param target The target CEL entity.
       * @param skillId The skill ID.
       */
      void RequestSkillUsageStart(iCelEntity* target, unsigned int skillId);

      /**
       * Request the server to start the using a skill on the supplied target.
       * @param targetId The entity ID of the skill target.
       * @param skillId The skill ID.
       */
      void RequestSkillUsageStart(unsigned int targetId, unsigned int skillId);
    };
  } // Combat namespace
} // PT namespace

#endif // COMBATMANAGER_H
