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

#include "client/gui/gui.h"

#include "CEGUI.h"
#include "CEGUIWindowManager.h"
#include "CEGUILogger.h"

#include "client/network/network.h"
#include "client/gui/guimanager.h"

#include "client/data/skill/skill.h"
#include "client/data/skill/skilldatamanager.h"

#include "client/pointer/pointer.h"
#include "common/reporter/reporter.h"

#include "client/combat/combatmanager.h"

namespace PT
{
  namespace GUI
  {
    namespace Windows
    {
      HUDWindow::HUDWindow (GUIManager* guimanager)
        : GUIWindow (guimanager)
      {
        windowName = HUDWINDOW;
        selectedskill = 0;
      }

      HUDWindow::~HUDWindow ()
      {
        delete selectedskill;
      }

      void HUDWindow::HideWindow()
      {
        winMgr->getWindow("PlayerHUD/Frame")->setVisible(false);
        //winMgr->getWindow("EntityHUD/Frame")->setVisible(false);
        winMgr->getWindow("SkillHUD/Frame")->setVisible(false);
      }

      void HUDWindow::ShowWindow()
      {
        winMgr->getWindow("PlayerHUD/Frame")->setVisible(true);
        //winMgr->getWindow("EntityHUD/Frame")->setVisible(true);
        winMgr->getWindow("SkillHUD/Frame")->setVisible(true);
      }

      /**
       *
       * Function that will update the HP bar in the gui.
       *
       * @param hp The current health.
       * @param maxHP The maximum health.
       *
       */
      void HUDWindow::SetHP(int hp, int maxHp)
      {
        char buf[30];
        float progress;
        if (maxHp == 0) {
          progress = 0.0f;
        } else {
          progress = hp/(float)maxHp;
        }
        hpbar->setProgress(progress);
        snprintf(buf, 29, "           %d / %d", hp, maxHp);
        buf[29] = '\0';

        SetText("PlayerHUD/HPValue", buf);
      }

      void HUDWindow::SetMP (float mp)
      {
        mpbar->setProgress(mp);
      }

      void HUDWindow::SetXP (float xp)
      {
        xpbar->setProgress(xp);
      }

      void HUDWindow::SetSP (float sp)
      {
        spbar->setProgress(sp);
      }

      void HUDWindow::SetName (const char* name)
      {
        try
        {
          CEGUI::DefaultWindow* namewin   = (CEGUI::DefaultWindow*) winMgr->getWindow("HUD/Name");
          namewin->setText((CEGUI::String)name);
        }
        catch(CEGUI::Exception /*&e*/)
        {
        }
      } // end SetName()

      void HUDWindow::SetText (const char* name, const char* text)
      {
        CEGUI::String cname(name);
        CEGUI::String ctext(text);
        try
        {
          CEGUI::DefaultWindow* namewin   = (CEGUI::DefaultWindow*) winMgr->getWindow(cname);
          namewin->setText(ctext);
        }
        catch(CEGUI::Exception /*&e*/)
        {
        }
      } // end SetText()

      CEGUI::Window* HUDWindow::CreateSkillSlot(CEGUI::Window* parent, const CEGUI::UVector2& position, int id)
      {
        // Create a name.
        char uniquename[64];
        snprintf(uniquename, 64, "SkillHUD/quickslot_%d", id+1);
        // Create the slot
        CEGUI::Window* slot = winMgr->createWindow("Peragro/StaticImage", uniquename);
        parent->addChildWindow(slot);
        slot->setPosition(position);
        slot->setSize(CEGUI::UVector2(CEGUI::UDim(0,24.0f), CEGUI::UDim(0,24.0f)));
        slot->subscribeEvent(CEGUI::Window::EventDragDropItemDropped, CEGUI::Event::Subscriber(&HUDWindow::HandleDragDropped, this));

        return slot;
      }

      CEGUI::Window* HUDWindow::CreateSkillIcon(int skillid)
      {
        char uniquename[1024];
        counter += 1;
        snprintf(uniquename, 1024, "SkillHUD/%d_%d_skillicon", skillid, counter);

        // create a drag/drop item
        CEGUI::DragContainer* skill = static_cast<CEGUI::DragContainer*>(
          winMgr->createWindow("DragContainer", uniquename));
        skill->setPosition(CEGUI::UVector2(CEGUI::UDim(0.0f,0.0f), CEGUI::UDim(0.0f,0.0f)));
        skill->setSize(CEGUI::UVector2(CEGUI::UDim(0.9f,0.0f), CEGUI::UDim(0.9f,0.0f)));
        skill->setHorizontalAlignment(CEGUI::HA_CENTRE);
        skill->setVerticalAlignment(CEGUI::VA_CENTRE);
        // Set the itemID.
        char itemtypestr[1024];
        snprintf(itemtypestr, 1024, "%d", skillid);
        skill->setUserString("skillid" , itemtypestr);

        // set a static image as drag container's contents
        CEGUI::Window* skillIcon = winMgr->createWindow("Peragro/StaticImage");
        skill->addChildWindow(skillIcon);
        skillIcon->setPosition(CEGUI::UVector2(CEGUI::UDim(0.0f,0.0f), CEGUI::UDim(0.0f,0.0f)));
        skillIcon->setSize(CEGUI::UVector2(CEGUI::UDim(1.0f,0.0f), CEGUI::UDim(1.0f,0.0f)));
        skillIcon->setHorizontalAlignment(CEGUI::HA_CENTRE);
        skillIcon->setVerticalAlignment(CEGUI::VA_CENTRE);
        skillIcon->setProperty("FrameEnabled", "False");

        // disable to allow inputs to pass through.
        skillIcon->disable();

        skill->subscribeEvent(CEGUI::DragContainer::EventMouseButtonDown, CEGUI::Event::Subscriber(&HUDWindow::HandleSkillSelected, this));
        skill->subscribeEvent(CEGUI::Window::EventDragDropItemDropped, CEGUI::Event::Subscriber(&HUDWindow::HandleDragDroppedOnSkill, this));

        // Set alpha
        skill->setAlpha(0.5f);
        skill->setInheritsAlpha(false);

        PT::Data::SkillDataManager* skillmgr = PointerLibrary::getInstance()->getSkillDataManager();
        PT::Data::Skill* skilld = skillmgr->GetSkillById(skillid);
        if (skilld)
        {
          skill->setTooltipText(skilld->GetName().c_str());
          skillIcon->setProperty("Image", skilld->GetIconName().c_str());
        }


        return skill;
      }

      bool HUDWindow::HandleSkillSelected(const CEGUI::EventArgs& args)
      {
        using namespace CEGUI;

        const DragDropEventArgs& ddea = static_cast<const DragDropEventArgs&>(args);

        SetActiveSkill(ddea.window);

        return true;
      }

      bool HUDWindow::HandleDragDropped(const CEGUI::EventArgs& args)
      {
        using namespace CEGUI;

        const DragDropEventArgs& ddea = static_cast<const DragDropEventArgs&>(args);

        ddea.window->setProperty("FrameColours", "tl:FFFFFFFF tr:FFFFFFFF bl:FFFFFFFF br:FFFFFFFF");
        ddea.window->addChildWindow(ddea.dragDropItem);
        return true;
      }

      bool HUDWindow::HandleDragDroppedOnSkill(const CEGUI::EventArgs& args)
      {
        using namespace CEGUI;

        const DragDropEventArgs& ddea = static_cast<const DragDropEventArgs&>(args);

        ddea.window->getParent()->addChildWindow(ddea.dragDropItem);

        // Remove the previos skill.
        winMgr->destroyWindow(ddea.window);
        return true;
      }

      int HUDWindow::GetActiveSkillId()
      {
        return selectedskill->SkillId;
      }

      void HUDWindow::SetActiveSkill(CEGUI::Window* window)
      {
        // See if its actually a skill in the slot.
        if (window->isUserStringDefined("skillid"))
        {
          // Reset the alpha on the previous window.
          CEGUI::Window* oldwindow = selectedskill->SkillWindow;
          if (oldwindow) oldwindow->setAlpha(0.5f);

          // Set the new skill active.
          window->setAlpha(1.0f);

          // Set the new window as the selected one
          selectedskill->SkillId = atoi((window->getUserString("skillid")).
                                        c_str());

          PointerLibrary::getInstance()->getCombatManager()->AttackRequest(
                                         selectedskill->SkillId);
        }
      }

      bool HUDWindow::AddSkill(int skillid)
      {
        CEGUI::Window* skillframe = winMgr->getWindow("SkillHUD/Frame");

        CEGUI::Window* freeslot = 0;
        unsigned int i = 0;

        while ((freeslot == 0) && (i < skillframe->getChildCount()))
        {
          CEGUI::Window* slot = skillframe->getChildAtIdx(i);
          i += 1;
          if (slot)
          {
            if (strcmp( "SkillHUD/Drag",slot->getName().c_str() ) )
            {
              if ((slot->getChildCount() < 1 ))
              {
                Report(PT::Debug, "AddSkill: slot %s is empty: Item added to slot.", slot->getName().c_str());
                freeslot = slot;
                freeslot->addChildWindow(CreateSkillIcon(skillid));
                return true;
              }
            }
          }
        }

        Report(PT::Error, "AddSkill: quickbar is full!");

        return false;
      }

      bool HUDWindow::OnRootKeyDown(const CEGUI::EventArgs& e)
      {
        using namespace CEGUI;

        const KeyEventArgs& keyArgs = static_cast<const KeyEventArgs&>(e);

        CEGUI::Window* slot = 0;
        CEGUI::Window* skill = 0;

        switch (keyArgs.scancode)
        {
        case Key::F1:
          slot = winMgr->getWindow("SkillHUD/quickslot_1");
          if (slot && slot->getChildCount() > 0) skill = slot->getChildAtIdx(0);
          if (skill) SetActiveSkill(skill);
          return true;
          break;

        case Key::F2:
          slot = winMgr->getWindow("SkillHUD/quickslot_2");
          if (slot && slot->getChildCount() > 0) skill = slot->getChildAtIdx(0);
          if (skill) SetActiveSkill(skill);
          return true;
          break;

        case Key::F3:
          slot = winMgr->getWindow("SkillHUD/quickslot_3");
          if (slot && slot->getChildCount() > 0) skill = slot->getChildAtIdx(0);
          if (skill) SetActiveSkill(skill);
          return true;
          break;

        case Key::F4:
          slot = winMgr->getWindow("SkillHUD/quickslot_4");
          if (slot && slot->getChildCount() > 0) skill = slot->getChildAtIdx(0);
          if (skill) SetActiveSkill(skill);
          return true;
          break;

        case Key::F5:
          slot = winMgr->getWindow("SkillHUD/quickslot_5");
          if (slot && slot->getChildCount() > 0) skill = slot->getChildAtIdx(0);
          if (skill) SetActiveSkill(skill);
          return true;
          break;

        case Key::F6:
          slot = winMgr->getWindow("SkillHUD/quickslot_6");
          if (slot && slot->getChildCount() > 0) skill = slot->getChildAtIdx(0);
          if (skill) SetActiveSkill(skill);
          return true;
          break;

        case Key::F7:
          slot = winMgr->getWindow("SkillHUD/quickslot_7");
          if (slot && slot->getChildCount() > 0) skill = slot->getChildAtIdx(0);
          if (skill) SetActiveSkill(skill);
          return true;
          break;

        case Key::F8:
          slot = winMgr->getWindow("SkillHUD/quickslot_8");
          if (slot && slot->getChildCount() > 0) skill = slot->getChildAtIdx(0);
          if (skill) SetActiveSkill(skill);
          return true;
          break;

        case Key::F9:
          slot = winMgr->getWindow("SkillHUD/quickslot_9");
          if (slot && slot->getChildCount() > 0) skill = slot->getChildAtIdx(0);
          if (skill) SetActiveSkill(skill);
          return true;
          break;

        case Key::F10:
          slot = winMgr->getWindow("SkillHUD/quickslot_10");
          if (slot && slot->getChildCount() > 0) skill = slot->getChildAtIdx(0);
          if (skill) SetActiveSkill(skill);
          return true;
          break;

        default: return false;
        }

        return false;
      }

      bool HUDWindow::Create()
      {
        ReloadWindow();
        return true;
      }

      bool HUDWindow::ReloadWindow()
      {
        window = GUIWindow::LoadLayout ("client/playerhud.xml");
        GUIWindow::AddToRoot(window);
        window = GUIWindow::LoadLayout ("client/entityhud.xml");
        GUIWindow::AddToRoot(window);
        window = GUIWindow::LoadLayout ("client/skillhud.xml");
        GUIWindow::AddToRoot(window);
        //GUIWindow::CreateGUIWindow ("playerhud.xml");
        //GUIWindow::CreateGUIWindow ("entityhud.xml");
        //GUIWindow::CreateGUIWindow ("skillhud.xml");
        winMgr = cegui->GetWindowManagerPtr ();

        selectedskill = new SelectedSkill();
        selectedskill->SkillId = 0;

        counter = 0;

        // Set up the progressmeters
        hpbar   = (CEGUI::ProgressBar*) winMgr->getWindow("PlayerHUD/HP");
        mpbar   = (CEGUI::ProgressBar*) winMgr->getWindow("PlayerHUD/MP");
        xpbar   = (CEGUI::ProgressBar*) winMgr->getWindow("PlayerHUD/XP");
        spbar   = (CEGUI::ProgressBar*) winMgr->getWindow("PlayerHUD/SP");

        hpbar->setProgress(1.0f);
        mpbar->setProgress(1.0f);
        xpbar->setProgress(1.0f);
        spbar->setProgress(1.0f);

            //TODO
        //Load the skill icon imageset
        vfs->ChDir ("/peragro/art/skins/default/");
        cegui->GetImagesetManagerPtr()->createImageset("/peragro/art/skins/default/images/skill.imageset", "Skill");

        // Create the skill holders.
        CEGUI::Window* skillframe = winMgr->getWindow("SkillHUD/Frame");
        for (int i=0; i<10; i++)
        {
          CreateSkillSlot(skillframe, CEGUI::UVector2(CEGUI::UDim(0,2.0f+(28*i)), CEGUI::UDim(0,2.0f)), i);
        }

        // F1-F6 keys for quickbar.
        btn = winMgr->getWindow("Root");
        btn->subscribeEvent(CEGUI::Window::EventKeyDown, CEGUI::Event::Subscriber(&HUDWindow::OnRootKeyDown, this));

        // test.
        AddSkill(1);
        AddSkill(2);
        return true;
      }
    }
  }
}
