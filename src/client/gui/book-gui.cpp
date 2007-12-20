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
#include "client/entity/entitymanager.h"

#include "client/reporter/reporter.h"

#include "client/event/eventmanager.h"
#include "client/event/bookevent.h"

BookWindow::BookWindow (GUIManager* guimanager)
: GUIWindow (guimanager)
{
  iObjectRegistry* obj_reg = PointerLibrary::getInstance()->getObjectRegistry();
  vfs =  csQueryRegistry<iVFS> (obj_reg);

  BookWindow::itemId = -1;
  BookWindow::slotId = -1;
}

BookWindow::~BookWindow ()
{
}

bool BookWindow::OnCloseButton(const CEGUI::EventArgs& args)
{
  rootwindow->setVisible(false);
  return true;
} // end OnCloseButton ()

bool BookWindow::HandleRead(PT::Events::Eventp ev)
{
  using namespace PT::Events;

  BookReadEvent* bookEv = GetBookEvent<BookReadEvent*>(ev);
  if (!bookEv) return false;

  // Set the book title.
  rootwindow->setText(bookEv->title.c_str());

  // Set the book text.
  CEGUI::Window* page1 = winMgr->getWindow("Book/Page1");
  std::string text = bookEv->text;
  if (text.compare("(NULL)") == 0)
  {
    text = "";
  }
  if (text.size() > 330)
  {
    text = text.substr(0,330);
  }
  page1->setText(text.c_str());

  // Set the page number.
  CEGUI::Window* page1nr = winMgr->getWindow("Book/Page1/PageNumber");
  page1nr->setText("1");

  rootwindow->show();

  return true;
} // end HandleRead ()

bool BookWindow::HandleWrite(PT::Events::Eventp ev)
{
  using namespace PT::Events;

  BookWriteEvent* bookEv = GetBookEvent<BookWriteEvent*>(ev);
  if (!bookEv) return false;

  InventoryWindow* inv = guimanager->GetInventoryWindow();
  if (!inv) return true;

  Slot* slot = inv->GetSlot(slotId);
  if (!slot) return true;

  Object* obj = slot->GetObject();
  if (!obj) return true;

  if (obj->GetId() == itemId)
    obj->SetVariationId(bookEv->variationId);

  return true;
} // end HandleWrite ()

void BookWindow::SetBook(unsigned int itemId, unsigned int slotId)
{
  BookWindow::itemId = itemId;
  BookWindow::slotId = slotId;
}// end SetBook ()

void BookWindow::CreateGUIWindow ()
{
  GUIWindow::CreateGUIWindow ("book.xml");

  winMgr = cegui->GetWindowManagerPtr ();

  // Get the root window
  rootwindow = winMgr->getWindow("Book/Frame");
  rootwindow->hide();

  //Register the close event.
  rootwindow->subscribeEvent(CEGUI::FrameWindow::EventCloseClicked, CEGUI::Event::Subscriber(&BookWindow::OnCloseButton, this));

  // Register the button events.
  //btn = winMgr->getWindow("Connect_Button");
  //btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&ConnectWindow::ConnectButtonPressed, this));

  // TODO: this souldn't be here, move to a manager.
  using namespace PT::Events;
  EventHandler<BookWindow>* cbRead = new EventHandler<BookWindow>(&BookWindow::HandleRead, this);
  PointerLibrary::getInstance()->getEventManager()->AddListener("book.read", cbRead);

  EventHandler<BookWindow>* cbWrite = new EventHandler<BookWindow>(&BookWindow::HandleWrite, this);
  PointerLibrary::getInstance()->getEventManager()->AddListener("book.write", cbWrite);

} // end CreateGUIWindow ()
