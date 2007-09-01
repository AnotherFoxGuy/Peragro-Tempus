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

#include "client/reporter/reporter.h"

HUDWindow::HUDWindow (GUIManager* guimanager)
: GUIWindow (guimanager)
{
}

HUDWindow::~HUDWindow ()
{
  delete selectedskill;
}

void HUDWindow::CreateGUIWindow ()
{
  GUIWindow::CreateGUIWindow ("playerhud.xml");
  GUIWindow::CreateGUIWindow ("entityhud.xml");
  GUIWindow::CreateGUIWindow ("skillhud.xml");
  winMgr = cegui->GetWindowManagerPtr ();

  selectedskill = new SelectedSkill();
  selectedskill->SkillId = 0;

  counter = 0;

  // Get the root window
  rootwindow = winMgr->getWindow("PlayerHUD/Frame");

  // Set up the progressmeters
  hpbar   = (CEGUI::ProgressBar*) winMgr->getWindow("PlayerHUD/HP");
  mpbar   = (CEGUI::ProgressBar*) winMgr->getWindow("PlayerHUD/MP");
  xpbar   = (CEGUI::ProgressBar*) winMgr->getWindow("PlayerHUD/XP");

  hpbar->setProgress(1.0f);
  mpbar->setProgress(1.0f);
  xpbar->setProgress(1.0f);

  //Load the skill icon imageset
  vfs->ChDir ("/peragro/skin/");
  cegui->GetImagesetManagerPtr()->createImageset("/peragro/skin/skill.imageset", "Skill");

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
  AddSkill("Heal", 1);
  AddSkill("Energy Bind", 2);
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

void HUDWindow::SetHP (float hp)
{
  hpbar->setProgress(hp);
}

void HUDWindow::SetMP (float mp)
{
  mpbar->setProgress(mp);
}
void HUDWindow::SetName (const char* name)
{
  CEGUI::DefaultWindow* namewin   = (CEGUI::DefaultWindow*) winMgr->getWindow("HUD/Name");

  namewin->setText((CEGUI::String)name);
}

CEGUI::Window* HUDWindow::CreateSkillSlot(CEGUI::Window* parent, const CEGUI::UVector2& position, int id)
{
  // Create a name.
  char uniquename[64];
  sprintf(uniquename, "SkillHUD/quickslot_%d", id+1);
  // Create the slot
  CEGUI::Window* slot = winMgr->createWindow("Peragro/StaticImage", uniquename);
  parent->addChildWindow(slot);
  slot->setPosition(position);
  slot->setSize(CEGUI::UVector2(CEGUI::UDim(0,24.0f), CEGUI::UDim(0,24.0f)));
  slot->subscribeEvent(CEGUI::Window::EventDragDropItemDropped, CEGUI::Event::Subscriber(&HUDWindow::HandleDragDropped, this));

  return slot;
}

CEGUI::Window* HUDWindow::CreateSkillIcon(CEGUI::String skillname, int skillid)
{
  char uniquename[1024];
  counter += 1;
  sprintf(uniquename, "SkillHUD/%d_%d_skillicon", skillid, counter);

  // create a drag/drop item
  CEGUI::DragContainer* skill = static_cast<CEGUI::DragContainer*>(
    winMgr->createWindow("DragContainer", uniquename));
  skill->setPosition(CEGUI::UVector2(CEGUI::UDim(0.0f,0.0f), CEGUI::UDim(0.0f,0.0f)));
  skill->setSize(CEGUI::UVector2(CEGUI::UDim(0.9f,0.0f), CEGUI::UDim(0.9f,0.0f)));
  skill->setHorizontalAlignment(CEGUI::HA_CENTRE);
  skill->setVerticalAlignment(CEGUI::VA_CENTRE);
  skill->setTooltipText(skillname);
  // Set the itemID.
  char itemtypestr[1024];
  sprintf(itemtypestr, "%d", skillid);
  skill->setUserString("skillid" , itemtypestr);

  // set a static image as drag container's contents
  CEGUI::Window* skillIcon = winMgr->createWindow("Peragro/StaticImage");
  skill->addChildWindow(skillIcon);
  skillIcon->setPosition(CEGUI::UVector2(CEGUI::UDim(0.0f,0.0f), CEGUI::UDim(0.0f,0.0f)));
  skillIcon->setSize(CEGUI::UVector2(CEGUI::UDim(1.0f,0.0f), CEGUI::UDim(1.0f,0.0f)));
  skillIcon->setHorizontalAlignment(CEGUI::HA_CENTRE);
  skillIcon->setVerticalAlignment(CEGUI::VA_CENTRE);
  skillIcon->setProperty("FrameEnabled", "False");

  char skillimage[1024];
  sprintf(skillimage, "set:Skill image:%d", skillid);
  skillIcon->setProperty("Image", skillimage);
  // disable to allow inputs to pass through.
  skillIcon->disable();

  skill->subscribeEvent(CEGUI::DragContainer::EventMouseButtonDown, CEGUI::Event::Subscriber(&HUDWindow::HandleSkillSelected, this));
  skill->subscribeEvent(CEGUI::Window::EventDragDropItemDropped, CEGUI::Event::Subscriber(&HUDWindow::HandleDragDroppedOnSkill, this));

  // Set alpha
  skill->setAlpha(0.5f);
  skill->setInheritsAlpha(false);

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
    selectedskill->SkillId     =  atoi( (window->getUserString("skillid")).c_str() ); 
  }
}

bool HUDWindow::AddSkill(CEGUI::String skillname, int skillid)
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
          freeslot->addChildWindow(CreateSkillIcon(skillname, skillid));
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

  CEGUI::Window* skillframe = winMgr->getWindow("SkillHUD/Frame");

  const KeyEventArgs& keyArgs = static_cast<const KeyEventArgs&>(e);

  CEGUI::Window* slot = 0;
  CEGUI::Window* skill = 0;

  switch (keyArgs.scancode)
  {
  case Key::F1:
    slot = winMgr->getWindow("SkillHUD/quickslot_1");
    if (slot && slot->getChildCount() > 0) skill = slot->getChildAtIdx(0);
    if (skill) SetActiveSkill(skill);
    break;

  case Key::F2:
    slot = winMgr->getWindow("SkillHUD/quickslot_2");
    if (slot && slot->getChildCount() > 0) skill = slot->getChildAtIdx(0);
    if (skill) SetActiveSkill(skill);
    break;

  case Key::F3:
    slot = winMgr->getWindow("SkillHUD/quickslot_3");
    if (slot && slot->getChildCount() > 0) skill = slot->getChildAtIdx(0);
    if (skill) SetActiveSkill(skill);
    break;

  case Key::F4:
    slot = winMgr->getWindow("SkillHUD/quickslot_4");
    if (slot && slot->getChildCount() > 0) skill = slot->getChildAtIdx(0);
    if (skill) SetActiveSkill(skill);
    break;

  case Key::F5:
    slot = winMgr->getWindow("SkillHUD/quickslot_5");
    if (slot && slot->getChildCount() > 0) skill = slot->getChildAtIdx(0);
    if (skill) SetActiveSkill(skill);
    break;

  case Key::F6:
    slot = winMgr->getWindow("SkillHUD/quickslot_6");
    if (slot && slot->getChildCount() > 0) skill = slot->getChildAtIdx(0);
    if (skill) SetActiveSkill(skill);
    break;

  case Key::F7:
    slot = winMgr->getWindow("SkillHUD/quickslot_7");
    if (slot && slot->getChildCount() > 0) skill = slot->getChildAtIdx(0);
    if (skill) SetActiveSkill(skill);
    break;

  case Key::F8:
    slot = winMgr->getWindow("SkillHUD/quickslot_8");
    if (slot && slot->getChildCount() > 0) skill = slot->getChildAtIdx(0);
    if (skill) SetActiveSkill(skill);
    break;

  case Key::F9:
    slot = winMgr->getWindow("SkillHUD/quickslot_9");
    if (slot && slot->getChildCount() > 0) skill = slot->getChildAtIdx(0);
    if (skill) SetActiveSkill(skill);
    break;

  case Key::F10:
    slot = winMgr->getWindow("SkillHUD/quickslot_10");
    if (slot && slot->getChildCount() > 0) skill = slot->getChildAtIdx(0);
    if (skill) SetActiveSkill(skill);
    break;

  default: return false;
  }

  return true;
}
