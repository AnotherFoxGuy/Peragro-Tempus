/*
    Copyright (C) 2005 Development Team of Peragro Tempus

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General public License for more details.

    You should have received a copy of the GNU General public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef INV_GUI_H
#define INV_GUI_H

#include <boost/shared_ptr.hpp>

#include "base-gui.h"

#define INVENTORYWINDOW "Inventory/Frame"

class CSIconRenderer;

namespace Common
{
  namespace Inventory
  {
    class Inventory;
  } // Inventory namespace
} // Common namespace

namespace PT
{
  namespace GUI
  {
    namespace Windows
    {
      class InventoryWindow : public GUIWindow
      {
      private:
        CSIconRenderer* rend;

      private:
        bool handleCloseButton(const CEGUI::EventArgs& args);

      public:
        InventoryWindow(GUIManager* guimanager);
        virtual ~InventoryWindow();
        bool Create();
        bool ReloadWindow();

        bool SetInventory(boost::shared_ptr<Common::Inventory::Inventory>);
      };

    } // Windows namespace
  } // GUI namespace
} // PT namespace

#endif // INV_GUI_H
