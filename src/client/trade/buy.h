/*
    Copyright (C) 2008 Development Team of Peragro Tempus

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

#ifndef PT_BUY_H
#define PT_BUY_H

#include <cssysdef.h>

#include "common/event/tradeevent.h"
#include "common/event/eventmanager.h"

struct iEvent;

namespace CEGUI
{
  class EventArgs;
}

namespace PT
{
  namespace GUI
  {
    class GUIManager;
    namespace Windows
    {
      class BuyWindow;
    }
  }

  namespace Trade
  {
    class Buy
    {
    private:
      GUI::GUIManager* guimanager;
      GUI::Windows::BuyWindow* buyWindow;
      void SetTrading(bool value);

    private:
      csRef<Events::EventHandlerCallback> tradeConfirm;
      csRef<Events::EventHandlerCallback> tradeCancel;
      PT_CALLBACK_HANDLER_LISTENERS
      bool TradeConfirmResponse(iEvent& ev);
      bool TradeCancel(iEvent& ev);
      bool TradeOffersList(iEvent& ev);

    public:
      Buy ();
      ~Buy ();

      bool Initialize ();
    };

  } // Trade namespace
} // PT namespace

#endif // PT_BUY_H
