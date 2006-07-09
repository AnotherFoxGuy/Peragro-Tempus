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

/*=================//
//    HUDWindow   //
//=================*/
HUDWindow::HUDWindow (GUIManager* guimanager)
: GUIWindow (guimanager)
{
}

HUDWindow::~HUDWindow ()
{
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
  vfs = guimanager->GetClient()->getVFS ();
  vfs->ChDir ("/peragro/skin/");
  cegui->GetImagesetManagerPtr()->createImageset("/peragro/skin/skill.imageset", "Skill");

  // Create the skill holders.
  CEGUI::Window* skillframe = winMgr->getWindow("SkillHUD/Frame");
  for (int i=0; i<6; i++)
  {
    CreateSkillSlot(skillframe, CEGUI::Point(4.0f+(35*i), 4.0f));
  }

  // test.
  AddSkill("Heal", 1);
  AddSkill("Energy Bind", 2);

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
  CEGUI::StaticText* namewin   = (CEGUI::StaticText*) winMgr->getWindow("HUD/Name");

  namewin->setText((CEGUI::String)name);
}
/*=================//
//    SkillHUD     //
//=================*/
CEGUI::Window* HUDWindow::CreateSkillSlot(CEGUI::Window* parent, const CEGUI::Point& position)
{
  // Create the slot
  CEGUI::Window* slot = winMgr->createWindow("Peragro/StaticImage");
  parent->addChildWindow(slot);
  //slot->setWindowPosition(position);
  slot->setPosition(CEGUI::Absolute, position);
  slot->setSize(CEGUI::Absolute, CEGUI::Size(31.0f, 31.0f));

  return slot;
}

CEGUI::Window* HUDWindow::CreateSkillIcon(CEGUI::String skillname, int skillid)
{
  char uniquename[1024];
  counter += 1;
  sprintf(uniquename, "%d_%d_skillicon", skillid, counter);

  // create a drag/drop item
  CEGUI::DragContainer* skill = static_cast<CEGUI::DragContainer*>(
    winMgr->createWindow("DragContainer", uniquename));
  skill->setWindowPosition(CEGUI::UVector2(CEGUI::cegui_reldim(0.05f), CEGUI::cegui_reldim(0.05f)));
  skill->setWindowSize(CEGUI::UVector2(CEGUI::cegui_reldim(0.9f), CEGUI::cegui_reldim(0.9f)));
  skill->setTooltipText(skillname);
  // Set the itemID.
  char itemtypestr[1024];
  sprintf(itemtypestr, "%d", skillid);
  skill->setUserString("skillid" , itemtypestr);

  // set a static image as drag container's contents
  CEGUI::Window* skillIcon = winMgr->createWindow("Peragro/StaticImage");
  skill->addChildWindow(skillIcon);
  skillIcon->setWindowPosition(CEGUI::UVector2(CEGUI::cegui_reldim(0), CEGUI::cegui_reldim(0)));
  skillIcon->setWindowSize(CEGUI::UVector2(CEGUI::cegui_reldim(1), CEGUI::cegui_reldim(1)));
  char skillimage[1024];
  sprintf(skillimage, "set:Skill image:%d", skillid);
  skillIcon->setProperty("Image", skillimage);
  // disable to allow inputs to pass through.
  skillIcon->disable();

  skill->subscribeEvent(CEGUI::Window::EventMouseClick, CEGUI::Event::Subscriber(&HUDWindow::HandleSkillSelected, this));

  // Set alpha
  skill->setAlpha(0.5f);
  skill->setInheritsAlpha(false);

  return skill;
}

bool HUDWindow::HandleSkillSelected(const CEGUI::EventArgs& args)
{
  using namespace CEGUI;

  const DragDropEventArgs& ddea = static_cast<const DragDropEventArgs&>(args);

  // Reset the alpha on the previous window.
  CEGUI::Window* window = selectedskill->SkillWindow;
  if (window) window->setAlpha(0.5f);

  // Set the new skill active.
  ddea.window->setAlpha(1.0f);

  // Set the new window as the selected one
  selectedskill->SkillWindow = ddea.window;
  selectedskill->SkillId     =  atoi( (ddea.window->getUserString("skillid")).c_str() ); 

  return true;
}

int HUDWindow::GetActiveSkillId() 
{ 
  return selectedskill->SkillId; 
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
    if (strcmp( "SkillHUD/Drag",slot->getName().c_str() ) )
    {
      if ((slot->getChildCount() < 1 ))
      {
        printf("slot %s is empty: Item added to slot\n", slot->getName().c_str());
        freeslot = slot;
        freeslot->addChildWindow(CreateSkillIcon(skillname, skillid));
        return true;
      }
    }
  }
  printf("skillframe is full!\n");
  return false;
}