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

#ifndef SELL_GUI_H
#define SELL_GUI_H

#include "base-gui.h"

#include "client/gui/common/inventory.h"

#define SELLWINDOW "SellWindow/Frame"

namespace PT
{
  namespace GUI
  {
    namespace Windows
    {
    class SellWindow : public GUIWindow
    {
    private:
      PT::GUI::Windows::DragDrop* dragdrop;
      PT::Data::ItemDataManager* itemDataManager;

      csArray<Slot*> upperslots;
      csArray<Slot*> lowerslots;

    private:
      bool OnCloseButton(const CEGUI::EventArgs& args);
      bool OnAccept(const CEGUI::EventArgs& args);

    public:
      SellWindow(GUIManager* guimanager);
      virtual ~SellWindow();
      bool Create();
      bool ReloadWindow();

    public:
      bool AddItem(unsigned int itemid, unsigned int slotid);
      void CancelTrade();
      void AcceptTrade();

    };
    }
  }
}

#endif // SELL_GUI_H
