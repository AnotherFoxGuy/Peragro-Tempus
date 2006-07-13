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

/*================//
// GUIInvWindow   //
//================*/
StatusWindow::StatusWindow(GUIManager* guimanager)
: GUIWindow (guimanager)
{
}

StatusWindow::~StatusWindow()
{
}

bool StatusWindow::handleCloseButton(const CEGUI::EventArgs& args)
{
  winMgr->getWindow("Status/Frame")->setVisible(false);
  //winMgr->getWindow("Chatlog/Frame")->activate();
  return true;
}
CEGUI::Window* StatusWindow::createDragDropSlot(CEGUI::Window* parent, const CEGUI::Point& position)
{
  // Create the slot
  CEGUI::Window* slot = winMgr->createWindow("Peragro/StaticImage");
  parent->addChildWindow(slot);
  slot->setPosition(CEGUI::Absolute, position);
  slot->setSize(CEGUI::Absolute, CEGUI::Size(24.0f, 24.0f));

  return slot;
}

CEGUI::Window* StatusWindow::createItemIcon(CEGUI::String itemname, int itemtype, bool stackable)
{
  char uniquename[1024];
  counter += 1;
  sprintf(uniquename, "%d_%d_icon", itemtype, counter);

  // create a drag/drop item
  CEGUI::DragContainer* item = static_cast<CEGUI::DragContainer*>(
    winMgr->createWindow("DragContainer", uniquename));
  item->setWindowPosition(CEGUI::UVector2(CEGUI::cegui_reldim(0.05f), CEGUI::cegui_reldim(0.05f)));
  item->setWindowSize(CEGUI::UVector2(CEGUI::cegui_reldim(0.9f), CEGUI::cegui_reldim(0.9f)));
  item->setTooltipText(itemname);
  // Set the itemID.
  char itemtypestr[1024];
  sprintf(itemtypestr, "%d", itemtype);
  item->setUserString("itemtype" , itemtypestr);
  // Set wether or not the item is stackable
  if (stackable)
  item->setUserString("stackable" , "true");
  else item->setUserString("stackable" , "false");

  // set a static image as drag container's contents
  CEGUI::Window* itemIcon = winMgr->createWindow("Peragro/StaticImage");
  item->addChildWindow(itemIcon);
  itemIcon->setWindowPosition(CEGUI::UVector2(CEGUI::cegui_reldim(0), CEGUI::cegui_reldim(0)));
  itemIcon->setWindowSize(CEGUI::UVector2(CEGUI::cegui_reldim(1), CEGUI::cegui_reldim(1)));
  //itemIcon->setProperty("Image", "set:Peragro image:CloseButtonNormal");
  char inventoryimage[1024];
  sprintf(inventoryimage, "set:Inventory image:%d", itemtype);
  itemIcon->setProperty("Image", inventoryimage);
  // disable to allow inputs to pass through.
  itemIcon->disable();

  return item;
}

void StatusWindow::AddSkil(const char* skillname, unsigned int skillvalue)
{
  btn = winMgr->getWindow("Status/SkillTab");
  char charskillvalue[10];
  sprintf(charskillvalue, "%d", skillvalue);
  CEGUI::ListboxItem* skillvalueItem = new CEGUI::ListboxTextItem(charskillvalue);
  CEGUI::ListboxItem* skillnameItem = new CEGUI::ListboxTextItem(skillname);

  //skillvalueItem->setSelectionBrushImage((CEGUI::utf8*)"Peragro", (CEGUI::utf8*)"TextSelectionBrush");
  //skillnameItem->setSelectionBrushImage((CEGUI::utf8*)"Peragro", (CEGUI::utf8*)"TextSelectionBrush");

  unsigned int row = ((CEGUI::MultiColumnList*)btn)->addRow();
  ((CEGUI::MultiColumnList*)btn)->setItem(skillnameItem, 0, row);
  ((CEGUI::MultiColumnList*)btn)->setItem(skillvalueItem, 1, row);
  //((CEGUI::MultiColumnList*)btn)->setSelectionMode(CEGUI::MultiColumnList::RowSingle);
}
void StatusWindow::CreateGUIWindow()
{
  GUIWindow::CreateGUIWindow ("status.xml");
  winMgr = cegui->GetWindowManagerPtr ();

  // Get the root window
  rootwindow = winMgr->getWindow("Status/Frame");

  // Create the SkillTab
  CEGUI::MultiColumnList* skilltab = static_cast<CEGUI::MultiColumnList*>(winMgr->createWindow("Peragro/MultiColumnList","Status/SkillTab"));
  skilltab->setWindowPosition(CEGUI::UVector2(CEGUI::cegui_reldim(0), CEGUI::cegui_reldim(0)));
  skilltab->setWindowSize(CEGUI::UVector2(CEGUI::cegui_reldim(1), CEGUI::cegui_reldim(1)));
  CEGUI::String str_skill("Skill");
  CEGUI::String str_value("Value");
  skilltab->addColumn(str_skill,0,0.65f);
  skilltab->addColumn(str_value,1,0.35f);
  skilltab->setSelectionMode(CEGUI::MultiColumnList::RowSingle);
  skilltab->setUserColumnSizingEnabled(false);
  skilltab->setText("Skills");

  // Create the SpellTab
  CEGUI::StaticImage* spelltab = static_cast<CEGUI::StaticImage*>(winMgr->createWindow("Peragro/StaticImage","Status/SpellTab"));
  spelltab->setWindowPosition(CEGUI::UVector2(CEGUI::cegui_reldim(0), CEGUI::cegui_reldim(0)));
  spelltab->setWindowSize(CEGUI::UVector2(CEGUI::cegui_reldim(1), CEGUI::cegui_reldim(1)));
  spelltab->setText("Spells");
  for (int j=0; j<6; j++)
  {
    for (int i=0; i<5; i++)
    {
      createDragDropSlot(spelltab, CEGUI::Point(4.0f+(28*i), 4.0f+(28*j)));
    }
  }

  // Create the QuestTab
  CEGUI::StaticImage* questtab = static_cast<CEGUI::StaticImage*>(winMgr->createWindow("Peragro/StaticImage","Status/QuestTab"));
  questtab->setWindowPosition(CEGUI::UVector2(CEGUI::cegui_reldim(0), CEGUI::cegui_reldim(0)));
  questtab->setWindowSize(CEGUI::UVector2(CEGUI::cegui_reldim(1), CEGUI::cegui_reldim(1)));
  questtab->setText("Quests");

  // Get the tabcontrol.
  CEGUI::TabControl* tabcontrol = static_cast<CEGUI::TabControl*>(winMgr->getWindow("Status/TabControl"));
  tabcontrol->setTabHeight(0.10f);
  tabcontrol->addTab(skilltab);
  tabcontrol->addTab(spelltab);
  tabcontrol->addTab(questtab);


  // Get the frame window
  CEGUI::FrameWindow* frame = static_cast<CEGUI::FrameWindow*>(winMgr->getWindow("Status/Frame"));
  frame->subscribeEvent(CEGUI::FrameWindow::EventCloseClicked, CEGUI::Event::Subscriber(&StatusWindow::handleCloseButton, this));

}
