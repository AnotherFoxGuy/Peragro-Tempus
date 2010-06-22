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

#include "client/gui/inventory-gui.h"

#include <CEGUI.h>
#include <CEGUIWindowManager.h>
#include <CEGUILogger.h>

#include "client/pointer/pointer.h"

#include "common/reporter/reporter.h"

#include "client/gui/guimanager.h"

#include "common/inventory/inventory.h"

//Custom windows
#include "common/inventory-cegui/inventory.h"
#include "common/inventory-cegui/object.h"

#include "common/csiconrenderer.h"

namespace PT
{
  namespace GUI
  {
    namespace Windows
    {

      InventoryWindow::InventoryWindow(GUIManager* guimanager)
        : GUIWindow (guimanager)
      {
        windowName = INVENTORYWINDOW;
        iObjectRegistry* obj_reg =
          PointerLibrary::getInstance()->getObjectRegistry();
        vfs =  csQueryRegistry<iVFS> (obj_reg);

        rend = new CSIconRenderer(obj_reg);
      } // end InventoryWindow()

      InventoryWindow::~InventoryWindow()
      {
        delete rend;
      } // end ~InventoryWindow()

      bool InventoryWindow::handleCloseButton(const CEGUI::EventArgs& args)
      {
        winMgr->getWindow("Inventory/Frame")->setVisible(false);
        return true;
      } // end handleCloseButton()

      bool InventoryWindow::SetInventory(boost::shared_ptr<Common::Inventory::Inventory> inventory)
      {
        CEGUI::Inventory* inv = (CEGUI::Inventory*)winMgr->getWindow("Inventory/Bag");
        inv->SetInventory(inventory);
        return true;
      } // end SetInventory()

      bool InventoryWindow::Create()
      {
        ReloadWindow();
        SetupToggleListener("Inventory");
        return true;
      } // end Create()

      bool InventoryWindow::ReloadWindow()
      {

        // Register our own window factories.
        {
          using namespace CEGUI;
          CEGUI::WindowFactoryManager* wfMgr = cegui->GetWindowFactoryManagerPtr();
          wfMgr->addFactory<InventoryFactory>();
          wfMgr->addFactory<ObjectFactory>();
          // Load the scheme.
          vfs->ChDir ("/peragro/art/skins/default/inventory/");
          cegui->GetSchemeManagerPtr ()->create("inventory.scheme");
          vfs->ChDir ("/peragro/art/skins/default/");
        }

        window = GUIWindow::LoadLayout ("client/inventory.layout");
        GUIWindow::AddToRoot(window);
        winMgr = cegui->GetWindowManagerPtr ();

        // Get the frame window
        CEGUI::FrameWindow* frame = static_cast<CEGUI::FrameWindow*>
          (winMgr->getWindow("Inventory/Frame"));
        frame->subscribeEvent(CEGUI::FrameWindow::EventCloseClicked,
          CEGUI::Event::Subscriber(&InventoryWindow::handleCloseButton, this));

        CEGUI::Inventory* inv = (CEGUI::Inventory*)winMgr->getWindow("Inventory/Bag");
        inv->setIconRenderer(rend);

        return true;
      } // end ReloadWindow()

    } // Windows namespace
  } // GUI namespace
} // PT namespace
