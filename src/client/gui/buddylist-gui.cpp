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

#include <CEGUI.h>
#include <CEGUIWindowManager.h>
#include <CEGUILogger.h>

#include "client/network/network.h"
#include "client/gui/guimanager.h"


/*================//
// GUIBuddyWindow //
//================*/
BuddyWindow::BuddyWindow(GUIManager* guimanager)
: GUIWindow (guimanager)
{
}

BuddyWindow::~BuddyWindow()
{
}

bool BuddyWindow::handleCloseButton(const CEGUI::EventArgs& args)
{
  winMgr->getWindow("BuddyList/Frame")->setVisible(false);
  return true;
}

bool BuddyWindow::OnRootKeyDown(const CEGUI::EventArgs& e)
{
    using namespace CEGUI;
    const KeyEventArgs& keyArgs = static_cast<const KeyEventArgs&>(e);

    CEGUI::Window* buddylist = winMgr->getWindow("BuddyList/Frame");
    if (!buddylist) return false;

    switch (keyArgs.scancode)
    {
    case Key::Insert:
      buddylist->isVisible() ? buddylist->setVisible(false) : buddylist->setVisible(true);
      break;

    default: return false;
    }
    return true;
}

void BuddyWindow::AddPlayer(const char* name)
{
  btn = winMgr->getWindow("BuddyList/SkillTab");
  CEGUI::ListboxItem* nameItem = new CEGUI::ListboxTextItem(name);

  unsigned int row = ((CEGUI::MultiColumnList*)btn)->addRow();
  ((CEGUI::MultiColumnList*)btn)->setItem(nameItem, 0, row);
}

void BuddyWindow::RemovePlayer(const char* name)
{
  btn = winMgr->getWindow("BuddyList/SkillTab");

  CEGUI::ListboxItem* nameItem = ((CEGUI::MultiColumnList*)btn)->findListItemWithText(name,0);
  if (!nameItem)
  {
    printf("BuddyWindow: ERROR Couldn't find player %s in buddylist!\n", name);
    return;
  }
  nameItem->setAutoDeleted(true);
  unsigned int row = ((CEGUI::MultiColumnList*)btn)->getItemRowIndex(nameItem);
  ((CEGUI::MultiColumnList*)btn)->removeRow(row);
}

void BuddyWindow::CreateGUIWindow()
{
  GUIWindow::CreateGUIWindow ("buddylist.xml");
  winMgr = cegui->GetWindowManagerPtr ();

  // Get the root window
  rootwindow = winMgr->getWindow("BuddyList/Frame");

  // Create the UserTab
  CEGUI::MultiColumnList* usertab = static_cast<CEGUI::MultiColumnList*>(winMgr->createWindow("Peragro/MultiColumnList","BuddyList/SkillTab"));
  usertab->setPosition(CEGUI::UVector2(CEGUI::UDim(0,0), CEGUI::UDim(0,0)));
  usertab->setSize(CEGUI::UVector2(CEGUI::UDim(1,0), CEGUI::UDim(1,0)));
  CEGUI::String name("Name");
  usertab->addColumn(name,0,CEGUI::UDim(0.9f,0));
  usertab->setSelectionMode(CEGUI::MultiColumnList::RowSingle);
  usertab->setUserColumnSizingEnabled(false);
  usertab->setProperty("Font", "Commonwealth-8");
  usertab->setText(" Players");

  // Get the tabcontrol.
  CEGUI::TabControl* tabcontrol = static_cast<CEGUI::TabControl*>(winMgr->getWindow("BuddyList/TabControl"));
  tabcontrol->setTabHeight(CEGUI::UDim(0.10f,0));
  tabcontrol->addTab(usertab);

  // Get the frame window
  CEGUI::FrameWindow* frame = static_cast<CEGUI::FrameWindow*>(winMgr->getWindow("BuddyList/Frame"));
  frame->subscribeEvent(CEGUI::FrameWindow::EventCloseClicked, CEGUI::Event::Subscriber(&BuddyWindow::handleCloseButton, this));

  // Key for buddylist.
  btn = winMgr->getWindow("Root");
  btn->subscribeEvent(CEGUI::Window::EventKeyDown, CEGUI::Event::Subscriber(&BuddyWindow::OnRootKeyDown, this));

}


