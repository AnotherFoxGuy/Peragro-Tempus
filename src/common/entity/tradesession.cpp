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

#include "tradesession.h"
#include "tradepeer.h"

#include "common/util/ptstring.h"

const char* TradeSession::sendRequest(TradePeer* peer)
{
  if (peer2->getSession() != 0)
  {
    return "is Busy";
  }
  peer2 = peer;
  return 0;
}

void TradeSession::sendResponse(const ptString& error)
{
  if (error == ptString::Null)
  {
    request_accepted = true;
  }
  else
  {
    peer1->setSession(0);
    peer2->setSession(0);
    delete this;
  }
}
