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

#include "client/gui/book-gui.h"
#include "client/gui/gui.h"

#include "CEGUI.h"
#include "CEGUIWindowManager.h"
#include "CEGUILogger.h"

#include "client/pointer/pointer.h"
#include "client/network/network.h"
#include "client/gui/guimanager.h"

#include "common/reporter/reporter.h"

#include "common/event/eventmanager.h"
#include "common/event/bookevent.h"

namespace PT
{
  namespace GUI
  {
    namespace Windows
    {
      BookWindow::BookWindow(PT::GUI::GUIManager* guimanager)
        : GUIWindow (guimanager)
      {
        windowName = BOOKWINDOW;
        iObjectRegistry* obj_reg =
          PointerLibrary::getInstance()->getObjectRegistry();
        vfs =  csQueryRegistry<iVFS> (obj_reg);

        BookWindow::itemId = -1;
        BookWindow::slotId = -1;
      } // end BookWindow()

      BookWindow::~BookWindow()
      {
      } // end ~BookWindow()

      bool BookWindow::OnCloseButton(const CEGUI::EventArgs& args)
      {
        HideWindow();
        return true;
      } // end OnCloseButton()

      bool BookWindow::HandleRead(iEvent& ev)
      {
        using namespace PT::Events;

        // Set the book title.
        SetName(PT::Events::BookHelper::GetTitle(&ev).c_str());
        // Set the book text.
        CEGUI::Window* page1 = winMgr->getWindow("Book/Page1");
        std::string text = PT::Events::BookHelper::GetText(&ev);
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

        ShowWindow();

        return true;
      } // end HandleRead()

      bool BookWindow::HandleWrite(iEvent& ev)
      {
        using namespace PT::Events;

        unsigned int variation = -1;
        ev.Retrieve("variation", variation);

        InventoryWindow* inv =
          guimanager->GetWindow<InventoryWindow>(INVENTORYWINDOW);
        if (!inv) return true;

        Slot* slot = inv->GetSlot(slotId);
        if (!slot) return true;

        Object* obj = slot->GetObject();
        if (!obj) return true;

        if (obj->GetId() == itemId)
        obj->SetVariationId(variation);

        return true;
      } // end HandleWrite()

      void BookWindow::SetBook(unsigned int itemId, unsigned int slotId)
      {
        BookWindow::itemId = itemId;
        BookWindow::slotId = slotId;
      }// end SetBook()

      bool BookWindow::Create()
      {
        ReloadWindow();
        return true;
      } // end Create()

      bool BookWindow::ReloadWindow()
      {
        using namespace PT::Events;

        // Load the layout and parent it to the root window.
        window = GUIWindow::LoadLayout ("client/book.layout");
        GUIWindow::AddToRoot(window);

        //Register the close event.
        //rootwindow->subscribeEvent(CEGUI::FrameWindow::EventCloseClicked,
        //  CEGUI::Event::Subscriber(&BookWindow::OnCloseButton, this));

        // Register the button events.
        //btn = winMgr->getWindow("Connect_Button");
        //btn->subscribeEvent(CEGUI::PushButton::EventClicked,
        //  CEGUI::Event::Subscriber(&ConnectWindow::ConnectButtonPressed, this));

        // TODO: this souldn't be here, move to a manager.
        PT_SETUP_HANDLER
        PT_REGISTER_LISTENER(BookWindow, HandleRead, "book.read")
        PT_REGISTER_LISTENER(BookWindow, HandleWrite, "book.write")

        return true;
      } // end ReloadWindow()

    } // Windows namespace
  } // GUI namespace
} // PT namespace

