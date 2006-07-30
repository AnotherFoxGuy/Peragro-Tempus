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

/*====================//
// SelectCharWindow   //
//====================*/
SelectCharWindow::SelectCharWindow(GUIManager* guimanager)
: GUIWindow (guimanager)
{
}

SelectCharWindow::~SelectCharWindow()
{
}

bool SelectCharWindow::SelectChar(const CEGUI::EventArgs& e) 
{
  btn = winMgr->getWindow("Characters");
  if (((CEGUI::MultiColumnList*)btn)->getSelectedCount() == 0)
    return true;

  CEGUI::ListboxItem* item = ((CEGUI::MultiColumnList*)btn)->getFirstSelectedItem();

  if (!item->isSelected()) return true;

  int own_char_id = atoi(item->getText().c_str());

  CharacterSelectionRequestMessage answer_msg;
  answer_msg.setCharId(own_char_id);
  network->send(&answer_msg);

  GUIWindow::DisableWindow();

  return true;
}


bool SelectCharWindow::NewChar(const CEGUI::EventArgs& e) 
{
  CEGUI::String NewCharName = GetNewCharName();
  if (NewCharName.empty()) return true;
  CharacterCreationRequestMessage answer_msg;
  answer_msg.setName(ptString(NewCharName.c_str(), NewCharName.length()));
  network->send(&answer_msg);
  return true;
}

bool SelectCharWindow::DelChar(const CEGUI::EventArgs& e) 
{
  return true;
}

CEGUI::String SelectCharWindow::GetNewCharName()  
{
  return winMgr->getWindow("CharNewName")->getText();
}


void SelectCharWindow::CreateGUIWindow()
{
  GUIWindow::CreateGUIWindow ("charselect.xml");

  winMgr = cegui->GetWindowManagerPtr ();

  // Get the root window
  rootwindow = winMgr->getWindow("CharSelectUI");

  // Register the button events.
  btn = winMgr->getWindow("CharSelOk");
  btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&SelectCharWindow::SelectChar, this));

  btn = winMgr->getWindow("CharSelNew");
  btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&SelectCharWindow::NewChar, this));

  btn = winMgr->getWindow("Characters");
  CEGUI::String str_id("Id");
  CEGUI::String str_name("Name");
  ((CEGUI::MultiColumnList*)btn)->addColumn(str_id,0,CEGUI::UDim(0.1f,0));
  ((CEGUI::MultiColumnList*)btn)->addColumn(str_name,1,CEGUI::UDim(0.5f,0));
  ((CEGUI::MultiColumnList*)btn)->setSelectionMode(CEGUI::MultiColumnList::RowSingle);

}

void SelectCharWindow::AddCharacter(unsigned int charId, const char* name)
{
  btn = winMgr->getWindow("Characters");
  char charIdstr[10];
  sprintf(charIdstr, "%d", charId);
  CEGUI::ListboxItem* charIdItem = new CEGUI::ListboxTextItem(charIdstr);
  CEGUI::ListboxItem* charNameItem = new CEGUI::ListboxTextItem(name);

  charIdItem->setSelectionBrushImage((CEGUI::utf8*)"Peragro", (CEGUI::utf8*)"TextSelectionBrush");
  charNameItem->setSelectionBrushImage((CEGUI::utf8*)"Peragro", (CEGUI::utf8*)"TextSelectionBrush");

  unsigned int row = ((CEGUI::MultiColumnList*)btn)->addRow();
  ((CEGUI::MultiColumnList*)btn)->setItem(charIdItem, 0, row);
  ((CEGUI::MultiColumnList*)btn)->setItem(charNameItem, 1, row);
  ((CEGUI::MultiColumnList*)btn)->setSelectionMode(CEGUI::MultiColumnList::RowSingle);
}
