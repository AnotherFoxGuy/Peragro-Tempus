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

#ifndef PT_EXCHANGE_H
#define PT_EXCHANGE_H

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
      class TradeWindow;
    }
  }

  namespace Trade
  {
    class Exchange
    {
    private:
      GUI::GUIManager* guimanager;
      GUI::Windows::TradeWindow* tradeWindow;
      void SetTrading(bool value);

    private:
      csRef<Events::EventHandlerCallback> tradeConfirm;
      csRef<Events::EventHandlerCallback> tradeCancel;
      CALLBACK_HANDLER_LISTENERS
      bool ExchangeRequest(iEvent& ev);
      bool ExchangeResponse(iEvent& ev);
      bool ExchangeConfirmResponse(iEvent& ev);
      bool ExchangeOfferAccept(iEvent& ev);
      bool ExchangeCancel(iEvent& ev);
      bool ExchangeOffersList(iEvent& ev);

    private:
      bool OnYesRequest(const CEGUI::EventArgs& args);
      bool OnNoRequest(const CEGUI::EventArgs& args);

    public:
      Exchange ();
      ~Exchange ();

      bool Initialize ();

      void Request(unsigned int entityId);
    };

  } // Trade namespace
} // PT namespace

#endif // PT_EXCHANGE_H
