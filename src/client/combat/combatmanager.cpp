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

#include "client/entity/character/resource/resource.h"

#include "include/effectsmanager.h"

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

      PT_REGISTER_LISTENER(CombatManager, UpdatePlayerResource, "entity.resource.list")
      PT_REGISTER_LISTENER(CombatManager, UpdatePlayerResource, "entity.resource.update")

      if (!entityManager) return Report(PT::Bug,
        "CombatManager: Failed to locate ptEntityManager plugin");

      if (!guiManager) return Report(PT::Bug,
        "CombatManager: Failed to locate GUIManager plugin");

      if (!network) return Report(PT::Bug,
        "CombatManager: Failed to locate Network plugin");

      return true;
    } // end Initialize()

    bool CombatManager::UpdatePlayerResource(iEvent& ev)
    {
      using namespace PT::GUI;
      using namespace PT::GUI::Windows;

      PT::Entity::PlayerEntity* player = PointerLibrary::getInstance()->getPlayer();
      if (!player) return true;

      if (!player->GetResourcesReady())
      {
        using namespace PT::Events;
        EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
        evmgr->AddEvent(&ev);
        return true;
      }

      boost::shared_ptr<PT::Entity::Resources> res = player->GetResources();

      GUIManager* guimanager = PointerLibrary::getInstance()->getGUIManager();
      StatsHUDWindow* statshudWindow = guimanager->GetWindow<StatsHUDWindow>(STATSHUDWINDOW);

      try
      {
        statshudWindow->SetHP(res->Get("Hit Points"), res->GetMax("Hit Points"));

        statshudWindow->SetMP(res->Get("Willpower"), res->GetMax("Willpower"));

        statshudWindow->SetSP(res->Get("Stamina"), res->GetMax("Stamina"));

        int delta = res->GetOld("Hit Points") - res->Get("Hit Points");
        if (delta != 0) Hit(player, delta);
        if (res->Get("Hit Points") <= 0) Die(player);
      }
      catch (PT::Entity::ResourceNotFound&){}

      return true;
    } // end UpdatePlayerResource()

    void CombatManager::Hit(PT::Entity::CharacterEntity* target, int damage)
    {
      using namespace PT::Entity;

      iObjectRegistry* object_reg = PointerLibrary::getInstance()->getObjectRegistry();
      csRef<iEffectsManager> effectsManager = csQueryRegistry<iEffectsManager>(object_reg);

      // Damage is positive, we got hurt.
      if (damage > 0)
      {
        if (effectsManager) effectsManager->CreateEffect("Blood", GetMesh(target));
        target->PlayAnimation("hit", 0.1f, false, true);
      }
      // Damage is negative, we got healed.
      else if (damage < 0)
      {
        // idle, Sit_up, walk, jump, attack, run, levitate, Sit_down, horse_mount, Sit, die,
        //target->PlayAnimation("levitate", 0.1f, false, true);
      }
      else if (damage == 0)
      {
        //effectsManager->CreateEffect("Deflect", GetMesh(target));
        //target->SetAction("deflect");
      }

      Report(PT::Debug, "You %s %d points!",
        damage < 0 ? "healed" : "got hit for", abs(damage));

    } // end Hit()

    void CombatManager::Die(PT::Entity::CharacterEntity* target)
    {
      target->PlayAnimation("die", 0.1f, false, false);
    } // end Die()

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
