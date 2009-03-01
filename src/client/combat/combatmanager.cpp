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

#include "client/combat/combatmanager.h"

#include <iutil/objreg.h>
#include <imap/loader.h>

#include <iengine/mesh.h>

#include <imesh/genmesh.h>
#include <imesh/gmeshskel2.h>
#include <imesh/skeleton.h>

#include <physicallayer/pl.h>
#include <physicallayer/propfact.h>
#include <physicallayer/propclas.h>
#include <physicallayer/entity.h>
#include <propclass/colldet.h>
#include <propclass/mesh.h>
#include <propclass/prop.h>

#include "common/reporter/reporter.h"

#include "client/gui/gui.h"
#include "client/gui/guimanager.h"

#include "client/cursor/cursor.h"

#include "include/effectsmanager.h"

#include "client/entity/entity.h"
#include "client/entity/entitymanager.h"

#include "client/network/network.h"
#include "common/network/netmessage.h"
#include "common/network/combatmessages.h"
#include "common/combat/combat.h"

#include "client/state/statemanager.h"

#include "include/client/component/entity/stat/stats.h"

namespace PT
{
  namespace Combat
  {

    CombatManager::CombatManager()
    {
    } // end CombatManager()

    CombatManager::~CombatManager()
    {
    } // end ~CombatManager()

    iMeshWrapper* CombatManager::GetMesh(::Client::Entity::Entity* entity)
    {
      iCelEntity* celentity = entity->GetCelEntity();
      if (!celentity) return 0;
      csRef<iPcMesh> pcmesh = CEL_QUERY_PROPCLASS_ENT(celentity, iPcMesh);
      if (!pcmesh) return 0;
      csRef<iMeshWrapper> parent = pcmesh->GetMesh();
      if (!parent) return 0;

      return parent;
    } // end GetMesh()

    bool CombatManager::Initialize()
    {
      entityManager = PointerLibrary::getInstance()->getEntityManager();
      guiManager = PointerLibrary::getInstance()->getGUIManager();
      network = PointerLibrary::getInstance()->getNetwork();

      PT_SETUP_HANDLER
      PT_REGISTER_LISTENER(CombatManager, ActionHit, "input.Hit")
      PT_REGISTER_LISTENER(CombatManager, ActionSelectTarget, "input.Attack")
      PT_REGISTER_LISTENER(CombatManager, AddStatPlayer, "entity.stat.add.player")
      PT_REGISTER_LISTENER(CombatManager, UpdateStat, "entity.stat.change")

      if (!entityManager) return Report(PT::Bug,
        "CombatManager: Failed to locate ptEntityManager plugin");

      if (!guiManager) return Report(PT::Bug,
        "CombatManager: Failed to locate GUIManager plugin");

      if (!network) return Report(PT::Bug,
        "CombatManager: Failed to locate Network plugin");

      return true;
    } // end Initialize()

    bool CombatManager::AddStatPlayer(iEvent& ev)
    {
      static int hp = 0;
      static int maxhp = 0;

      //TODO: This can be removed when stats component is fixed. (line 59)
      if (ev.GetName() != PointerLibrary::getInstance()->getEventManager()->
        Retrieve("entity.stat.add.player"))
      {
        return true;
      }

      const char* name;
      unsigned int level = -1;
      ev.Retrieve("name", name);
      ev.Retrieve("level", level);

      if (strncmp("Endurance", name, strlen("Endurance")) == 0)
      {
        maxhp = level;
      }

      if (strncmp("Health", name, strlen("Health")) == 0)
      {
        hp = level;
      }

      using namespace PT::GUI;
      using namespace PT::GUI::Windows;

      GUIManager* guimanager = PointerLibrary::getInstance()->getGUIManager();
      StatsHUDWindow* statshudWindow = guimanager->GetWindow<StatsHUDWindow>(STATSHUDWINDOW);
      statshudWindow->SetHP(hp, maxhp);

      return true;
    } // end AddStatPlayer()

    bool CombatManager::UpdateStat(iEvent& ev)
    {
      using namespace PT::Events;
      using namespace PT::Entity;
      using namespace PT::GUI;
      using namespace PT::GUI::Windows;

      unsigned int entityid = -1;
      ev.Retrieve("entityid", entityid);

      ::Client::Entity::Entity* target = dynamic_cast< ::Client::Entity::Entity*>
        (entityManager->FindById(entityid).get());
      if (!target)
      {
        Report(PT::Error, "CombatManager: Couldn't find entity with ID %d !",
          entityid);
        return true;
      }

      csRef<iStats> stats =
        target->GetComponent<iStats>("peragro.entity.stats");
      if (!stats)
      {
        Report(PT::Error,
          "CombatManager: Couldn't find stats for entity with ID %d !",
          entityid);
        return true;
      }

      unsigned int statId = -1;
      ev.Retrieve("id", statId);

      // Locate the correct stat.
      Stat* stat = stats->GetStat(statId);
      if (!stat)
      {
        Report(PT::Error,
          "CombatManager: Couldn't find stats for entity with ID %d !",
          entityid);
        return true;
      }

      unsigned int oldValue = stat->level;
      ev.Retrieve("level", stat->level);

      if (strncmp("Health", stat->name.c_str(), strlen("Health")) == 0)
      {
        // If the health update belong to the player, update the gui.
        if (target->GetType() == Common::Entity::PlayerEntityType)
        {
          // Life = 1 * endurance
          int maxLife = stats->GetStatLevel("Endurance");

          GUIManager* guimanager =
            PointerLibrary::getInstance()->getGUIManager();
          StatsHUDWindow* statshudWindow = guimanager->GetWindow<StatsHUDWindow>(STATSHUDWINDOW);
          statshudWindow->SetHP(stat->level, maxLife);
        }

        if (oldValue != 0 && stat->level != 0)
        {
          int damage = 0;
          if (ev.AttributeExists("delta"))
            ev.Retrieve("delta", damage);
          else
            damage = oldValue - stat->level;
          Hit(target, damage);
        }
        // If HP = 0, then play the die animation for the dying entity.
        if (stat->level == 0)
          Die(target);
      }

      return true;

    } // end UpdateStat()

    void CombatManager::Hit(::Client::Entity::Entity* target, int damage)
    {
      using namespace PT::Entity;

      // Damage is positive, we got hurt.
      if (damage > 0)
      {
        //effectsManager->CreateEffect("Blood", GetMesh(target));
        if (target->GetType() == Common::Entity::PCEntityType)
        {
          ((PT::Entity::PcEntity*) target)->PlayAnimation("hit", 0.1f);
        }
      }
      // Damage is negative, we got healed.
      else if (damage < 0)
      {
        //effectsManager->CreateEffect("Heal", GetMesh(target));
        //target->SetAction("heal");
      }
      else if (damage == 0)
      {
        //effectsManager->CreateEffect("Deflect", GetMesh(target));
        //target->SetAction("deflect");
      }

      Report(PT::Debug, "You %s %d points!",
        damage < 0 ? "healed" : "got hit for", damage);

    } // end Hit()

    void CombatManager::Die(::Client::Entity::Entity* target)
    {
      using namespace Entity;
      CharacterEntity *character;
      //effectsManager->CreateEffect("Die", GetMesh(target));
      if (target->GetType() == Common::Entity::PCEntityType ||
          target->GetType() == Common::Entity::NPCEntityType ||
          target->GetType() == Common::Entity::PlayerEntityType)
      {
        character = static_cast<CharacterEntity*>(target);
        character->PlayAnimation("die", 0.1f, false, false);
      }
    } // end Die()

    void CombatManager::LevelUp(int tarGetId)
    {
      // Lookup the ID to get the actual entity.
      Common::Entity::Entityp target = entityManager->FindById(tarGetId);

      if (!target)
      {
        Report(PT::Error, "CombatManager: Couldn't find entity with ID %d !",
          tarGetId);
        return;
      }

      //effectsManager->CreateEffect("Levelup", GetMesh(target));
      //guiManager->GetCombatLog()->AddMessage("%s has gained a level.",
      //  target->GetName());

      Report(PT::Debug, "%s has gained a level.", target->GetName().c_str());
    } // end LevelUp()

    void CombatManager::Experience(int exp)
    {
      if (!PT::Entity::PlayerEntity::Instance()) return;
      // Lookup the ID to get the actual entity.
      boost::shared_ptr< ::Client::Entity::Entity> entity = PT::Entity::PlayerEntity::Instance();

      if (!entity)
      {
        Report(PT::Error, "CombatManager: Couldn't find player entity!");
        return;
      }

      // Update the entity's experience(this will update the GUI aswell).
      //entity->SetExp(exp);
      //guiManager->GetStatsWindow()->Setexp(exp);

      // We gained experience.
      if (exp >= 0)
      {
        //guiManager->GetCombatLog()->
        //  AddMessage("You gained %d experience points", exp);
        Report(PT::Debug, "You gained %d experience points!", exp);
      }
      // We lost experience.
      else if (exp < 0)
      {
        //guiManager->GetCombatLog()->
        //  AddMessage("You lost %d experience points", exp);
        Report(PT::Debug, "You lost %d experience points!", exp);
      }
    } // end Experience()

    void CombatManager::SkillUsageStart(unsigned int casterId,
                                        unsigned int tarGetId,
                                        int skillId, ptString error)
    {
      using namespace PT::GUI;
      using namespace PT::GUI::Windows;

      if (ptString(0,0) == error)
      {
        Report(PT::Debug, "CombatManager: %d cast %d on %d, error: %s\n",
          casterId, tarGetId, skillId, *error);
      }
      else
      {
        Report(PT::Notify, "CombatManager: %s \n", *error);
        char msg[1024];
        snprintf(msg, 1024, "%s", *error);
        ChatWindow* chatWindow = guiManager->GetWindow<ChatWindow>(CHATWINDOW);
        chatWindow->AddMessage(msg);
        return;
      }

      // Lookup the IDs to get the actual entities.
      Common::Entity::Entityp caster = entityManager->FindById(casterId);
      Common::Entity::Entityp target = entityManager->FindById(tarGetId);

      if (!target)
      {
        Report(PT::Error, "CombatManager: Couldn't find target with ID %d !", tarGetId);
        return;
      }
      if (!caster)
      {
        Report(PT::Error, "CombatManager: Couldn't find caster with ID %d !", casterId);
        return;
      }

      std::string caststring;

      char msg[1024];
      snprintf(msg, 1024, "%s %s %s.", caster->GetName().c_str(),
                               caststring.c_str(),
                               target->GetName().c_str());
      ChatWindow* chatWindow = guiManager->GetWindow<ChatWindow>(CHATWINDOW);
      chatWindow->AddMessage(msg);

    } // end SkillUsageStart()

    void CombatManager::SkillUsageComplete(unsigned int casterId, unsigned int tarGetId, int skillId)
    {
      using namespace PT::GUI;
      using namespace PT::GUI::Windows;

      // Lookup the IDs to get the actual entities.
      Common::Entity::Entityp caster = entityManager->FindById(casterId);
      Common::Entity::Entityp target = entityManager->FindById(tarGetId);

      if (!target)
      {
        Report(PT::Error, "CombatManager: Couldn't find target with ID %d !", tarGetId);
        return;
      }
      if (!caster)
      {
        Report(PT::Error, "CombatManager: Couldn't find attacker with ID %d !", casterId);
        return;
      }

      std::string caststring;

      char msg[1024];
      snprintf(msg, 1024, "%s %s %s.", caster->GetName().c_str(),
        caststring.c_str(), target->GetName().c_str());
      ChatWindow* chatWindow = guiManager->GetWindow<ChatWindow>(CHATWINDOW);
      chatWindow->AddMessage(msg);

    } // end SkillUsageComplete()

    void CombatManager::RequestSkillUsageStart(iCelEntity* target, unsigned int skillId)
    {
      // Lookup the ID to get the actual entity.
      csRef<iPcProperties> pcprop = CEL_QUERY_PROPCLASS_ENT(target, iPcProperties);
      if (!pcprop)
      {
        Report(PT::Error, "CombatManager: Couldn't find pcprop for target!");
        return;
      }

      unsigned int tarGetId = pcprop->GetPropertyLong(pcprop->GetPropertyIndex("Entity ID"));

      RequestSkillUsageStart(tarGetId, skillId);
    } // end RequestSkillUsageStart()

    void CombatManager::RequestSkillUsageStart(unsigned int tarGetId,
                                               unsigned int skillId)
    {
      // if (!skillId)
      // {
      //   Report(PT::Error, "CombatManager: skillId is 0!");
      //   return;
      // }

      if (!PT::Entity::PlayerEntity::Instance()) return;

      // Get your own entity.
      boost::shared_ptr< ::Client::Entity::Entity> attacker = PT::Entity::PlayerEntity::Instance();

      if (!tarGetId)
      {
        Report(PT::Error, "CombatManager: Couldn't find ID for target!");
        return;
      }
      if (!attacker)
      {
        Report(PT::Error, "CombatManager: Couldn't find attacker entity!");
        return;
      }

      // TODO clean this code up, the entire function...
      SelectTargetMessage selectMsg;
      selectMsg.setTargetID(tarGetId);
      network->send(&selectMsg);

      AttackRequestMessage requestMsg;
      requestMsg.setAttackType(InteractionID::NORMAL_ATTACK);
      network->send(&requestMsg);

      /* Next are a couple of checks to not stress the server/network
       * with attacks it will reject anyway.
       * Of course it still needs to be implemented on the server
       * to avoid cheating.
       */

      /*
      // Calculate the distance between both enemies.
      float distance = distance(target, attacker);
      // See if the attacker is actually facing the target
      bool lookat = lookat(attacker, target);
      if (distance > 10.0f)
      {
        guiManager->GetCombatLog()->AddMessage("Target is out of range!");
        return;
      }
      if (!lookat)
      {
        guiManager->GetCombatLog()->AddMessage("You should face your target!");
        return;
      }
      */

      // Prepare and send the network message.
      //AttackRequestMessage msg;
      // msg.setTargetID(tarGetId);
      // msg.setAttackType(skillId);
      // network->send(&msg);

      // Report(PT::Debug, "CombatManager: Sent SkillUsageStartRequestMessage target: %d skillid: %d.", tarGetId, skillId);
    } // end RequestSkillUsageStart()

    bool CombatManager::ActionHit(iEvent& ev)
    {
      using namespace PT::Events;

      if (PointerLibrary::getInstance()->getStateManager()->GetState() == PT::STATE_PLAY)
      {
        if (InputHelper::GetButtonDown(&ev))
        {
          Hit(PT::Entity::PlayerEntity::Instance().get(), 20);
        }
      }
      return true;
    } // end ActionHit()

    void CombatManager::AttackRequest(int attackType)
    {
        // Send message to server requesting an attack
        AttackRequestMessage msg;
        msg.setAttackType(attackType);
        network->send(&msg);
    }

    bool CombatManager::ActionSelectTarget(iEvent& ev)
    {
      using namespace PT::Events;

      // Only attack upon button down events.
      if (InputHelper::GetButtonDown(&ev))
      {

        csRef<iCelEntity> ent = PointerLibrary::getInstance()->getCursor()->GetSelectedEntity();
        if (!ent) return false;
        csRef<iPcProperties> pcprop = CEL_QUERY_PROPCLASS_ENT(ent, iPcProperties);
        if (!pcprop) return false;

        unsigned int id = pcprop->GetPropertyLong(pcprop->GetPropertyIndex("Entity ID"));

        Report(PT::Debug, "CombatManager: Sending AttackRequest, target: %u\n",id);

        // Send message to server requesting an attack
        SelectTargetMessage msg;
        msg.setTargetID(id);
        network->send(&msg);

        // TODO
        // Do local check as well (to check if we can attack yet etc) and if so
        // play animation.
        return true;
      }
      return false;
    } // end ActionAttackTarget()

  } // Combat namespace
} // PT namespace
